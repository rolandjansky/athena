# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

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
        # the logic it implemets is as follows:
        # full flag name is formed from the path + name passsed as an argument
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
            self.dump("Trig.*")
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

    def __init__(self,inputflags=None):


        if inputflags:
            self._flagdict=inputflags
        else:
            self._flagdict=dict()
        self._locked=False
        self._dynaflags = dict()
        self._loaded    = set() # dynamic dlags that were loaded

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
        raise RuntimeError( "No such flag: "+ name+".  The name is likely incomplete." )


    def addFlag(self,name,setDef=None):
        if (self._locked):
            raise RuntimeError("Attempt to add a flag to an already-locked container")

        if name in self._flagdict:
            raise KeyError("Duplicated flag name: {}".format( name ))
        self._flagdict[name]=CfgFlag(setDef)
        return

    def addFlagsCategory(self, path, generator, prefix=False):
        """
        The path is the beginning of the flag name, that is, if it is "X" the flags generated by the generator should be "X.*"
        The generator is a function that returns flags container, the flags have to start with the same path
        When the prefix is set to True the flags created by the generator when added are prefixed by the "path"

        Supported calls are then:
         addFlagsCategory("A", g) - where g is function creating flags  is f.addFlag("A.x", someValue)
         addFlagsCategory("A", g, True) - when flags are defined in g like this: f.addFalg("x", somevalue),
        The later option allows to share one generation function among the flags that are later loaded in a different paths.
        """
        self._dynaflags[path] = (generator, prefix)

    def needFlagsCategory(self, name):
        """ public interface for _loadDynaFlags """
        self._loadDynaFlags( name )

    def _loadDynaFlags(self, name):
        """
        loads the flags of the form "A.B.C" first attemprintg the path "A" then "A.B" and then "A.B.C"
        """
        def __load_impl( flagBaseName ):
            if flagBaseName in self._loaded:
                _msg.debug("Flags %s already loaded",flagBaseName  )
                return
            if flagBaseName in self._dynaflags:
                _msg.debug("Dynamically loading the flags under %s", flagBaseName )
                isLocked = self._locked
                self._locked = False
                generator, prefix = self._dynaflags[flagBaseName]
                self.join( generator(), flagBaseName if prefix else "" )
                self._locked = isLocked
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
        if (self._locked):
            raise RuntimeError("Attempt to set a flag of an already-locked container")
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
        self._locked=True
        return

    def locked(self):
        return self._locked


    def clone(self):
        #return and unlocked copy of self
        return AthConfigFlags(deepcopy(self._flagdict))


    def cloneAndReplace(self,subsetToReplace,replacementSubset):
        #This is to replace subsets of configuration flags like
        #egamamaFlags.GSF by egamma.TrigGSFFlavor1
        #self.dump()
        _msg.info("cloning flags and replacing {} by {}".format( subsetToReplace, replacementSubset ) )

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

                newFlagDict[subsetToReplace+subName]=deepcopy(flag)
                pass
            else:
                newFlagDict[name]=deepcopy(flag) #All other flags are simply copied
                pass
            #End loop over flags
            pass

        #Last sanity check: Make sure that teh replaced section still contains teh same names:
        if (replacedNames!=replacementNames):
            _msg.error(replacedNames)
            _msg.error(replacementNames)
            raise RuntimeError("Attempt to replace incompatible subsets: None matching flag names are "
                               + repr(replacedNames ^ replacementNames ))
        newFlags = AthConfigFlags(newFlagDict)
        newFlags._dynaflags = deepcopy(self._dynaflags)
        return newFlags



    def join(self, other, prefix=''):
        """
        Merges two flag containers
        When the prefix is passed each flag from the "other" is prefixed by "prefix."
        """
        if (self._locked):
            raise RuntimeError("Attempt to join with and already-locked container")

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

    def dump(self, pattern=".*"):
        import re
        compiled = re.compile(pattern)
        print("{:40} : {}".format( "Flag Name","Value" ) )
        for name in sorted(self._flagdict):
            if compiled.match(name):
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
        parser.add_argument("-d","--debug",default=None,help="attach debugger (gdb) before run, <stage>: conf, init, exec, fini")
        parser.add_argument("--evtMax",type=int,default=None,help="Max number of events to process")
        parser.add_argument("--skipEvents",type=int,default=None,help="Number of events to skip")
        parser.add_argument("--filesInput",default=None,help="Input file(s)")
        parser.add_argument("-l", "--loglevel",default=None,help="logging level (ALL, VERBOSE, DEBUG,INFO, WARNING, ERROR, or FATAL")
        return parser

    # parser argument must be an ArgumentParser returned from getArgumentParser()
    def fillFromArgs(self,listOfArgs=None,parser=None):
        """
        Used to set flags from command-line parameters, like ConfigFlags.fillFromArgs(sys.argv[1:])
        """
        import sys
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
            except NameError: #Can't determine type, assume we got an un-quoted string
                value="\"{}\"".format( value )

            #Arg looks good enough, just exec it:
            argToExec="self.{}={}".format( key, value )

            exec(argToExec)
            pass
        return



import unittest
class TestFlagsSetup(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag("Atest", True)
        self.flags.addFlag("A.One", True)
        self.flags.addFlag("A.B.C", False)
        self.flags.addFlag("A.dependentFlag", lambda prevFlags: ["FALSE VALUE", "TRUE VALUE"][prevFlags.A.B.C] )

class TestAccess(TestFlagsSetup):
    def runTest(self):
        print("""... Test access""")
        self.assertFalse( self.flags.A.B.C, "Can't read A.B.C flag")
        self.flags.A.B.C = True
        self.assertTrue( self.flags.A.B.C, "Flag value not changed")


class TestWrongAccess(TestFlagsSetup):
    def runTest(self):
        print("""... Acess to the flag that are missnames should give an exception""")
        with self.assertRaises(RuntimeError):
            print(".... test printout {}".format( self.flags.A is True ))
            print(".... test printout {}".format( self.flags.A.B == 6 ))



class TestDependentFlag(TestFlagsSetup):
    def runTest(self):
        print("""... The dependent flags will use another flag value to establish its own value""")
        self.flags.A.B.C= True
        self.flags.lock()
        self.assertEqual(self.flags.A.dependentFlag, "TRUE VALUE", " dependent flag setting does not work")

class TestFlagsSetupDynamic(TestFlagsSetup):
    def setUp(self):
        super(TestFlagsSetupDynamic, self).setUp()

        def theXFlags():
            nf = AthConfigFlags()
            nf.addFlag("a", 17)
            nf.addFlag("b", 55)
            nf.addFlag("c", "Hello")
            return nf


        def theZFlags():
            nf = AthConfigFlags()
            nf.addFlag("Z.A", 7)
            nf.addFlag("Z.B", True)
            nf.addFlag("Z.C.setting", 99)
            nf.addFlagsCategory( 'Z.Xclone1', theXFlags, prefix=True )
            nf.addFlagsCategory( 'Z.Xclone2', theXFlags, prefix=True )
            return nf

        def theTFlags():
            nf = AthConfigFlags()
            nf.addFlag("T.Abool", False)
            return nf

        self.flags.addFlagsCategory( 'Z', theZFlags )
        self.flags.addFlagsCategory( 'X', theXFlags, prefix=True )
        self.flags.addFlagsCategory( 'T', theTFlags )
        print("... Setup of dynamic flags loading test")
        self.flags.dump()
        print("")

class TestDynamicFlagsDump(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dump with unloaded flags works""")
        self.flags.dump()
        print("")

class TestDynamicFlagsRead(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dynamic flags reading works""")
        self.assertEqual( self.flags.X.a, 17, "dynamically loaded flags have wrong value")
        print("")
        self.assertEqual( self.flags.Z.A, 7, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.Xclone1.b, 55, "dynamically loaded flags have wrong value")
        self.flags.Z.Xclone2.b = 56
        self.assertEqual( self.flags.Z.Xclone2.b, 56, "dynamically loaded flags have wrong value")
        self.flags.dump()
        print("")

class TestDynamicFlagsSet(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dynamic flags setting works""")
        self.flags.Z.A = 15
        self.flags.Z.Xclone1.a = 20
        self.flags.X.a = 30
        self.assertEqual( self.flags.Z.Xclone1.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.X.a, 30, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.A, 15, "dynamically loaded flags have wrong value")
        self.flags.dump()
        print("")

class TestOverwriteFlags(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if overwiting works""")
        self.flags.Z.Xclone1.a = 20
        self.flags.X.a = 30
        copyf = self.flags.cloneAndReplace( "X", "Z.Xclone1")
        self.assertEqual( copyf.X.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( copyf.T.Abool, False, "The flags clone does not have dynamic flags")
        copyf.dump()
        print("")

class TestDynamicDependentFlags(unittest.TestCase):
    def runTest(self):
        print("""... Check if dynamic dependent flags work""")
        flags = AthConfigFlags()
        flags.addFlag("A", True)
        flags.addFlag("B", lambda prevFlags: 3 if prevFlags.A is True else 10 )
        flags.addFlag("C", lambda prevFlags: 'A' if prevFlags.A is True else 'B' )
        assert flags.B == 3
        flags.A = False
        assert flags.B == 10
        flags.A = True
        flags.lock()
        assert flags.C == 'A'
        print("")

class flagsFromArgsTest(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag('Exec.OutputLevel',3) #Global Output Level
        self.flags.addFlag('Exec.MaxEvents',-1)
        self.flags.addFlag("Exec.SkipEvents",0)
        self.flags.addFlag("Exec.DebugStage","")
        self.flags.addFlag('Input.Files',[])
        self.flags.addFlag('detA.flagB',0)

    def runTest(self):
        argline="-l VERBOSE --debug exec --evtMax=10 --skipEvents=3 --filesInput=bla1.data,bla2.data detA.flagB=7"
        print ("Interpreting arguments:")
        print (argline)
        self.flags.fillFromArgs(argline.split())
        self.assertEqual(self.flags.Exec.OutputLevel,1,"Failed to set output level from args")
        self.assertEqual(self.flags.Exec.MaxEvents,10,"Failed to set MaxEvents from args")
        self.assertEqual(self.flags.Exec.SkipEvents,3,"Failed to set SkipEvents from args")
        self.assertEqual(self.flags.Exec.DebugStage,"exec","Failed to set DebugStage from args")
        self.assertEqual(self.flags.Input.Files,["bla1.data","bla2.data"],"Failed to set FileInput from args")
        self.assertEqual(self.flags.detA.flagB,7,"Failed to set arbitrary from args")



if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(TestAccess())
    suite.addTest(TestWrongAccess())
    suite.addTest(TestDependentFlag())
    suite.addTest(TestDynamicFlagsDump())
    suite.addTest(TestDynamicFlagsRead())
    suite.addTest(TestDynamicFlagsSet())
    suite.addTest(TestOverwriteFlags())
    suite.addTest(TestDynamicDependentFlags())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)
