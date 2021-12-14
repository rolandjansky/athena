# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from copy import deepcopy
from AthenaCommon.Logging import logging
_msg = logging.getLogger('AthConfigFlags')

class CfgFlag(object):
    __slots__ = ['_value','_setDef']

    def __init__(self,default):
        if default is None:
            raise RuntimeError("Default value of a flag must not be None")
        self.set(default)
        return


    def set(self,value):
        if callable(value):
            self._value=None
            self._setDef=value
        else:
            self._value=value
            self._setDef=None
        return

    def get(self,flagdict=None):
        if self._value is not None:
            return deepcopy(self._value)

        #Have to call the method to obtain the default value, and then reuse it in all next accesses
        if flagdict.locked():
            # optimise future reads, drop possibility to update this flag ever
            self._value=self._setDef(flagdict)
            self._setDef=None
            return deepcopy(self._value)
        else:
            #use function for as long as the flags are not locked
            return deepcopy(self._setDef(flagdict))

    def __repr__(self):
        if self._value is not None:
            return repr(self._value)
        else:
            return "[function]"


class FlagAddress(object):
    def __init__(self, f, name):
        if isinstance(f, AthConfigFlags):
            self._flags = f
            self._name = name

        elif isinstance(f, FlagAddress):
            self._flags = f._flags
            self._name  = f._name+"."+name

    def __getattr__(self, name):
        # the logic it implements is as follows:
        # full flag name is formed from the path + name passed as an argument
        # first try if the flags is available (most frequent case)
        # if not see if the path+name is used in one of the flags of longer name (having more pieces)
        # if not try dynamic flags loading
        # after basically above stops are repeated

        merged = self._name + "." + name
        _msg.debug("Flags addr __getattr__ %s", merged )
        if self._flags.hasFlag( merged ):
            _msg.debug("Flags addr __getattr__ there %s", merged )
            return self._flags._get( merged )

        if self._flags.hasCategory( merged ): # the flag name is not complete yet
            _msg.debug("Flags addr __getattr__ category there %s", merged )
            return FlagAddress( self, name )

        _msg.debug("Flags addr __getattr__ need dynaload %s", merged  )
        self._flags._loadDynaFlags( merged )

        if self._flags.hasCategory( merged ): # the flag name is not complete yet
            _msg.debug("Flags addr __getattr__ category there after dynaload %s", merged )
            return FlagAddress( self, name )

        if self._flags.hasFlag( merged ):
            return self._flags._get( merged )

        raise RuntimeError( "No such flag: {}  The name is likely incomplete.".format(merged) )

    def __setattr__( self, name, value ):
        if name.startswith("_"):
            return object.__setattr__(self, name, value)
        merged = self._name + "." + name

        if not self._flags.hasFlag( merged ): # flag ismisisng, try loading dynamic ones
            self._flags._loadDynaFlags( merged )

        if not self._flags.hasFlag( merged ):
            raise RuntimeError( "No such flag: {}  The name is likely incomplete.".format(merged) )
        return self._flags._set( merged, value )

    def __cmp__(self, other):
        raise RuntimeError( "No such flag: "+ self._name+".  The name is likely incomplete." )
    __eq__ = __cmp__
    __ne__ = __cmp__
    __lt__ = __cmp__
    __le__ = __cmp__
    __gt__ = __cmp__
    __ge__ = __cmp__


    def __nonzero__(self):
        raise RuntimeError( "No such flag: "+ self._name+".  The name is likely incomplete." )

    def __bool__(self):
        raise RuntimeError( "No such flag: "+ self._name+".  The name is likely incomplete." )



class AthConfigFlags(object):

    def __init__(self):        
        self._flagdict=dict()
        self._locked=False
        self._dynaflags = dict()
        self._loaded    = set() # dynamic dlags that were loaded
        self._hash = None

    def athHash(self):
        if self._hash is None:
            raise RuntimeError("Cannot calculate hash of unlocked flag container")
        else:
            return self._hash

    def __hash__(self):
        raise DeprecationWarning("__hash__ method in AthConfigFlags is deprecated. Probably called from function decorator, use AccumulatorCache decorator instead.")

    def _calculateHash(self):
        return hash(str(self._flagdict.items()))

    def __getattr__(self, name):
        _msg.debug("AthConfigFlags __getattr__ %s", name )
        if name in self._flagdict:
            return self._get(name)
        return FlagAddress(self, name)

    def __setattr__(self, name, value):
        if name.startswith("_"):
            return object.__setattr__(self, name, value)

        _msg.debug("AthConfigFlags __getattr__ %s", name )
        if name in self._flagdict:
            return self._set(name, value)
        raise RuntimeError( "No such flag: "+ name+". The name is likely incomplete." )


    def addFlag(self,name,setDef):
        self._tryModify()
        if name in self._flagdict:
            raise KeyError("Duplicated flag name: {}".format( name ))
        self._flagdict[name]=CfgFlag(setDef)
        return

    def addFlagsCategory(self, path, generator, prefix=False):
        """
        The path is the beginning of the flag name (e.g. "X" for flags generated with name "X.*").
        The generator is a function that returns a flags container, the flags have to start with the same path.
        When the prefix is True the flags created by the generator are prefixed by "path".

        Supported calls are then:
         addFlagsCategory("A", g) - where g is function creating flags  is f.addFlag("A.x", someValue)
         addFlagsCategory("A", g, True) - when flags are defined in g like this: f.addFalg("x", somevalue),
        The latter option allows to share one generator among flags that are later loaded in different paths.
        """
        self._tryModify()
        _msg.debug("Adding flag category %s", path)
        self._dynaflags[path] = (generator, prefix)

    def needFlagsCategory(self, name):
        """ public interface for _loadDynaFlags """
        self._loadDynaFlags( name )

    def _loadDynaFlags(self, name):
        """
        loads the flags of the form "A.B.C" first attempting the path "A" then "A.B" and then "A.B.C"
        """

        def __load_impl( flagBaseName ):
            if flagBaseName in self._loaded:
                _msg.debug("Flags %s already loaded",flagBaseName  )
                return
            if flagBaseName in self._dynaflags:
                _msg.debug("Dynamically loading the flags under %s", flagBaseName )
                # Retain locked status and hash
                isLocked = self._locked
                myHash = self._hash
                self._locked = False
                generator, prefix = self._dynaflags[flagBaseName]
                self.join( generator(), flagBaseName if prefix else "" )
                self._locked = isLocked
                self._hash = myHash
                del self._dynaflags[flagBaseName]
                self._loaded.add(flagBaseName)

        pathfrags = name.split('.')
        for maxf in range(1, len(pathfrags)+1):
            __load_impl( '.'.join(pathfrags[:maxf]) )

    def loadAllDynamicFlags(self):
        """Force load all the dynamic flags """
        while len(self._dynaflags) != 0:
            # Need to convert to a list since _loadDynaFlags may change the dict.
            for prefix in list(self._dynaflags.keys()):
                self._loadDynaFlags( prefix )

    def hasCategory(self, name):
        path = name+'.'
        for f in self._flagdict.keys():
            if f.startswith(path):
                return True
        for c in self._dynaflags.keys():
            if c.startswith(path):
                return True
        return False

    def hasFlag(self, name):
        if name in self._flagdict:
            return True
        _msg.debug("Flag %s absent, possibly not loaded yet?", name )
        return False

    def _set(self,name,value):
        self._tryModify()
        if name in self._flagdict:
            self._flagdict[name].set(value)
            return
        errString="No flag with name \'{}\' found".format( name )
        from difflib import get_close_matches
        closestMatch=get_close_matches(name,self._flagdict.keys(),1)
        if len(closestMatch)>0:
            errString+=". Did you mean \'{}\'?".format(  closestMatch[0] )
        raise KeyError(errString)

    def _get(self,name):
        if name in self._flagdict:
            return self._flagdict[name].get(self)

        errString="No flag with name \'{}\' found".format( name )
        from difflib import get_close_matches
        closestMatch=get_close_matches(name,self._flagdict.keys(),1)
        if len(closestMatch)>0:
            errString+=". Did you mean \'{}\'?".format( closestMatch[0] )
        raise KeyError(errString)

    def __call__(self,name):
        return self._get(name)

    def lock(self):
        if(not self._locked):
            self._locked = True
            self._hash = self._calculateHash()
        return

    def locked(self):
        return self._locked

    def _tryModify(self):
        if self._locked:
            raise RuntimeError("Attempt to modify locked flag container")
        else:
            # if unlocked then invalidate hash
            self._hash = None

    def clone(self):
        #return an unlocked copy of self
        cln = AthConfigFlags()
        cln._flagdict = deepcopy(self._flagdict)
        return cln


    def cloneAndReplace(self,subsetToReplace,replacementSubset):
        """
        This is to replace subsets of configuration flags like

        Example:
        newflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')
        """

        def _copyFunction(obj):            
            return obj if self.locked() else deepcopy(obj) # if flags are locked we can reuse containers, no need to deepcopy

        _msg.info("cloning flags and replacing %s by %s", subsetToReplace, replacementSubset)

        self._loadDynaFlags( subsetToReplace )
        self._loadDynaFlags( replacementSubset )

        if not subsetToReplace.endswith("."):
            subsetToReplace+="."
            pass
        if not replacementSubset.endswith("."):
            replacementSubset+="."
            pass

        #Sanity check: Don't replace a by a
        if (subsetToReplace == replacementSubset):
            raise RuntimeError("Can not replace flags {} with themselves".format(subsetToReplace))


        replacedNames=set()
        replacementNames=set()
        newFlagDict=dict()
        for (name,flag) in self._flagdict.items():
            if name.startswith(subsetToReplace):
                replacedNames.add(name[len(subsetToReplace):]) #Remember replaced flag for the check later
            elif name.startswith(replacementSubset):
                subName=name[len(replacementSubset):]
                replacementNames.add(subName) # remember replacement name
                #Move the flag to the new name:

                newFlagDict[subsetToReplace+subName] = _copyFunction(flag)
                pass
            else:
                newFlagDict[name] = _copyFunction(flag) #All other flags are simply copied
                pass
            #End loop over flags
            pass

        #Last sanity check: Make sure that the replaced section still contains the same names:
        if not replacementNames.issuperset(replacedNames):
            _msg.error(replacedNames)
            _msg.error(replacementNames)
            raise RuntimeError("Attempt to replace incompatible flags subsets: distinct flag are "
                               + repr(replacementNames - replacedNames))
        newFlags = AthConfigFlags()
        newFlags._flagdict = newFlagDict

        for k,v in self._dynaflags.items(): # cant just assign the dicts because then they are shared when loading
            newFlags._dynaflags[k] = _copyFunction(v)
        newFlags._hash = None
        
        if self._locked:
            newFlags.lock()
        return newFlags



    def join(self, other, prefix=''):
        """
        Merges two flag containers
        When the prefix is passed each flag from the "other" is prefixed by "prefix."
        """
        self._tryModify()

        for (name,flag) in other._flagdict.items():
            fullName = prefix+"."+name if prefix != "" else name
            if fullName in self._flagdict:
                raise KeyError("Duplicated flag name: {}".format( fullName ) )
            self._flagdict[fullName]=flag

        for (name,loader) in other._dynaflags.items():
            fullName = prefix+"."+name if prefix != "" else name
            if fullName in self._dynaflags:
                raise KeyError("Duplicated dynamic flags name: {}".format( fullName ) )
            _msg.debug("Joining dynamic flags with %s", fullName)
            self._dynaflags[fullName] = loader
        return

    def dump(self, pattern=".*", evaluate=False):
        import re
        compiled = re.compile(pattern)
        print("{:40} : {}".format( "Flag Name","Value" ) )
        for name in sorted(self._flagdict):
            if compiled.match(name):
                if evaluate:
                    print("{:40} : {}".format( name, self._flagdict[name].get(self) ) )
                else:
                    print("{:40} : {}".format( name, repr(self._flagdict[name] ) ) )

        if len(self._dynaflags) == 0:
            return
        print("Flag categories that can be loaded dynamically")
        print("{:25} : {:>30} : {}".format( "Category","Generator name", "Defined in" ) )
        for name,gen_and_prefix in sorted(self._dynaflags.items()):
            if compiled.match(name):
                print("{:25} : {:>30} : {}".format( name, gen_and_prefix[0].__name__, '/'.join(gen_and_prefix[0].__code__.co_filename.split('/')[-2:]) ) )


    def initAll(self):
        """
        Mostly a self-test method
        """
        for n,f in list(self._flagdict.items()):
            f.get(self)
        return


    def getArgumentParser(self):
        """
        Scripts calling AthConfigFlags.fillFromArgs can extend this parser, and pass their version to fillFromArgs
        """
        import argparse
        parser= argparse.ArgumentParser()
        parser.add_argument("-d","--debug", default=None, help="attach debugger (gdb) before run, <stage>: conf, init, exec, fini")
        parser.add_argument("--evtMax", type=int, default=None, help="Max number of events to process")
        parser.add_argument("--skipEvents", type=int, default=None, help="Number of events to skip")
        parser.add_argument("--filesInput", default=None, help="Input file(s)")
        parser.add_argument("-l", "--loglevel", default=None, help="logging level (ALL, VERBOSE, DEBUG,INFO, WARNING, ERROR, or FATAL")
        parser.add_argument("--configOnly", type=str, default=None, help="Stop after configuration phase (may not be respected by all diver scripts)")
        parser.add_argument("--threads", type=int, default=0, help="Run with given number of threads")

        return parser

    # parser argument must be an ArgumentParser returned from getArgumentParser()
    def fillFromArgs(self, listOfArgs=None, parser=None):
        """
        Used to set flags from command-line parameters, like ConfigFlags.fillFromArgs(sys.argv[1:])
        """
        import sys

        self._tryModify()

        if parser is None:
            parser = self.getArgumentParser()
        (args,leftover)=parser.parse_known_args(listOfArgs or sys.argv[1:])

        #First, handle athena.py-like arguments:

        if args.debug:
            from AthenaCommon.Debugging import DbgStage
            if args.debug not in DbgStage.allowed_values:
                raise ValueError("Unknown debug stage, allowed values {}".format(DbgStage.allowed_values))
            self.Exec.DebugStage=args.debug

        if args.evtMax:
            self.Exec.MaxEvents=args.evtMax

        if args.skipEvents:
            self.Exec.SkipEvents=args.skipEvents

        if args.filesInput:
            self.Input.Files=args.filesInput.split(",")

        if args.loglevel:
            from AthenaCommon import Constants
            if hasattr(Constants,args.loglevel):
                self.Exec.OutputLevel=getattr(Constants,args.loglevel)
            else:
                raise ValueError("Unknown log-level, allowed values are ALL, VERBOSE, DEBUG,INFO, WARNING, ERROR, FATAL")
        
        if args.threads:
            self.Concurrency.NumThreads = args.threads

        #All remaining arguments are assumed to be key=value pairs to set arbitrary flags:


        for arg in leftover:
            #Safety check on arg: Contains exactly one '=' and left side is a valid flag
            argsplit=arg.split("=")
            if len(argsplit)!=2:
                raise ValueError("Can't interpret argument {}, expected a key=value format".format( arg ) )

            key=argsplit[0].strip()

            if not self.hasFlag(key):
                self._loadDynaFlags( '.'.join(key.split('.')[:-1]) ) # for a flag A.B.C dymanic flags from category A.B
            if not self.hasFlag(key):
                raise KeyError("{} is not a known configuration flag".format( key ) )

            value=argsplit[1].strip()

            try:
                exec("type({})".format( value ) )
            except (NameError, SyntaxError): #Can't determine type, assume we got an un-quoted string
                value="\"{}\"".format( value )

            #Arg looks good enough, just exec it:
            argToExec="self.{}={}".format( key, value )

            exec(argToExec)
            pass
        return args



