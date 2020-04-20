# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import six

from copy import deepcopy
from AthenaCommon.Logging import logging
class CfgFlag(object):
    __slots__ = ['_value','_setDef']

    def __init__(self,default):
        if default is None:
            raise RuntimeError("Default value of a flag must not be None")
        if callable(default):
            self._value=None
            self._setDef=default
        else:
            self._value=default
            self._setDef=None
        return


    def set(self,value):
        self._value=value
        return
    
    def get(self,flagdict=None):
        if self._value is None:
            #Have to call the method to obtain the default value
            self._value=self._setDef(flagdict)
        return deepcopy(self._value) 

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
        merged = self._name + "." + name
        if self._flags.hasFlag( merged ):
            return self._flags._get( merged )
        return FlagAddress( self, name )

    def __setattr__( self, name, value ):
        if name.startswith("_"):
            return object.__setattr__(self, name, value)
        merged = self._name + "." + name
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
        self._msg = logging.getLogger('AthConfigFlags')
        
        if inputflags:
            self._flagdict=inputflags
        else:
            self._flagdict=dict()
        self._locked=False
        self._dynaflags = dict()

    def __getattr__(self, name):
        return FlagAddress(self, name)

    def addFlag(self,name,setDef=None):
        if (self._locked):
            raise RuntimeError("Attempt to add a flag to an already-locked container")

        if name in self._flagdict:
            raise KeyError("Duplicated flag name: {}".format( name ))
        
        self._flagdict[name]=CfgFlag(setDef)
        return
    
    def addFlagsCategory(self, name, generator):
        self._dynaflags[name] = generator

    def needFlagsCategory(self, name):
        self._loadDynaFlags( name )
    
    def _loadDynaFlags(self, name):
        flagBaseName = name.split('.')[0]
        if flagBaseName in self._dynaflags:
            self._msg.debug("dynamically loading the flag {}".format( flagBaseName) )
            isLocked = self._locked
            self._locked = False
            self.join( self._dynaflags[flagBaseName]() )
            self._locked = isLocked
            del self._dynaflags[flagBaseName]

    def loadAllDynamicFlags(self):
        # Need to convert to a list since _loadDynaFlags may change the dict.
        for prefix in list(self._dynaflags.keys()):
            self._loadDynaFlags( prefix )

    def hasFlag(self, name):        
        if name in self._flagdict: 
            return True
        self._msg.debug("Flag not loaded yet {}".format( name ) )
        self._loadDynaFlags( name.split('.')[0] )
        return name in self._flagdict

    def _set(self,name,value):

        if (self._locked):
            raise RuntimeError("Attempt to set a flag of an already-locked container")
        self._loadDynaFlags( name )
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
        self._loadDynaFlags( name )
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
        self.dump()
        self._msg.info("cloning flags and replacing {} by {}".format( subsetToReplace, replacementSubset ) )

        self._loadDynaFlags( subsetToReplace )
        
        if not subsetToReplace.endswith("."):
            subsetToReplace+="."
            pass
        if not replacementSubset.endswith("."):
            replacementSubset+="."
            pass

        #Sanity check: Don't replace a by a 
        if (subsetToReplace == replacementSubset):
            raise RuntimeError("Called cloneAndReplace with identical strings")
                
        replacedNames=set()
        replacementNames=set()
        newFlagDict=dict()
        for (name,flag) in self._flagdict.items():
            if name.startswith(subsetToReplace):
                replacedNames.add(name[len(subsetToReplace):]) #Add the
                pass #Do not copy stuff thats going to be replaced
            elif name.startswith(replacementSubset):
                subName=name[len(replacementSubset):]
                replacementNames.add(subName)
                #Move the flag to the new name:
                
                newFlagDict[subsetToReplace+subName]=deepcopy(flag) 
                #If we lock the container right away, we don't need to deepcopy
                pass
            else: 
                newFlagDict[name]=deepcopy(flag) #All other flags are simply copied
                #If we lock the container right away, we don't need to deepcopy
                pass
            #End loop over flags
            pass

        #Last sanity check: Make sure that teh replaced section still contains teh same names:
        if (replacedNames!=replacementNames):
            self._msg.error(replacedNames)
            self._msg.error(replacementNames)
            raise RuntimeError("Attempt to replace incompatible subsets: None matching flag names are " 
                               + repr(replacedNames ^ replacementNames ))
        newFlags = AthConfigFlags(newFlagDict)
        newFlags._dynaflags = deepcopy(self._dynaflags)
        newFlags.dump()
        return newFlags



    def join(self, other, prefix=''):
        if (self._locked):
            raise RuntimeError("Attempt to join with and already-locked container")

        for (name,flag) in six.iteritems(other._flagdict):
            fullName = prefix+"."+name if prefix != "" else name
            if fullName in self._flagdict:
                raise KeyError("Duplicated flag name: {}".format( fullName ) )
            self._flagdict[fullName]=flag

        for (name,loader) in six.iteritems(other._dynaflags):
            if prefix+"."+name in self._dynaflags:
                raise KeyError("Duplicated dynamic flags name: {}".format( name ) )
            self.join( loader(), name )

        return

    def dump(self):
        print("{:40} : {}".format( "Flag Name","Value" ) )
        for name in sorted(self._flagdict):
            print("{:40} : {}".format( name, repr(self._flagdict[name] ) ) )

        if len(self._dynaflags) == 0:
            return
        print("Flag categories that can be loaded dynamically")
        print("{:25} : {:>30} : {}".format( "Category","Generator name", "Defined in" ) )
        for name,gen in sorted(six.iteritems(self._dynaflags)):
            print("{:25} : {:>30} : {}".format( name, gen.__name__, '/'.join(six.get_function_code(gen).co_filename.split('/')[-2:]) ) )


    def initAll(self): #Mostly a self-test method
        for n,f in list(self._flagdict.items()):
            f.get(self)
        return


    def fillFromArgs(self,listOfArgs=None):
        """
        Expects a list of strings of key=value pairs representing configuration flags. 
        Used to set flags from command-line parameters, like ConfigFlags.fillFromArgs(sys.argv[1:])
        """
        
        if listOfArgs is None:
            import sys
            listOfArgs=sys.argv[1:]

        for arg in listOfArgs:
            #Safety check on arg: Contains exactly one '=' and left side is a valid flag
            argsplit=arg.split("=")
            if len(argsplit)!=2:
                raise ValueError("Can't interpret argument {}, expected a key=value format".format( arg ) )

            key=argsplit[0].strip()
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
        self.flags.addFlag("A", True)
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
            nf.addFlag("X.a", 17)
            nf.addFlag("X.b", 55)
            nf.addFlag("X.c", "Hello")
            return nf

        def theZFlags():
            nf = AthConfigFlags()
            nf.addFlag("Z.A", 7)
            nf.addFlag("Z.B", True)
            nf.addFlag("Z.C.setting", 99)
            nf.addFlagsCategory( 'Z', theXFlags )
            nf.addFlagsCategory( 'Z.clone', theXFlags )
            return nf

        def theTFlags():
            nf = AthConfigFlags()
            nf.addFlag("T.Abool", False)
            return nf

        self.flags.addFlagsCategory( 'Z', theZFlags )
        self.flags.addFlagsCategory( 'X', theXFlags )
        self.flags.addFlagsCategory( 'T', theTFlags )
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
        self.flags.dump()
        print("")
        self.assertEqual( self.flags.Z.A, 7, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.X.b, 55, "dynamically loaded flags have wrong value")
        self.flags.dump()        
        print("")

class TestDynamicFlagsSet(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dynamic flags setting works""")
        self.flags.Z.A = 15
        self.flags.Z.X.a = 20
        self.flags.X.a = 30
        self.assertEqual( self.flags.Z.X.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.X.a, 30, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.A, 15, "dynamically loaded flags have wrong value")
        self.flags.dump()        
        print("")

class TestOverwriteFlags(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if overwiting works""")
        self.flags.Z.clone.X.a = 20
        self.flags.X.a = 30
        copyf = self.flags.cloneAndReplace( "X", "Z.clone.X")
        self.assertEqual( copyf.X.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( copyf.T.Abool, False, "The flags clone does not have dynamic flags")
        copyf.dump()
        print("")

if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(TestAccess())
    suite.addTest(TestWrongAccess())
    suite.addTest(TestDependentFlag())
    suite.addTest(TestDynamicFlagsDump())
    suite.addTest(TestDynamicFlagsRead())
    suite.addTest(TestDynamicFlagsSet())
    suite.addTest(TestOverwriteFlags())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)

