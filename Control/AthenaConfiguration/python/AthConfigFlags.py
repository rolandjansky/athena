# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
            #print "AutoConfiguring a config flag"
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
        raise RuntimeError( "No such a flag: "+ self._name+". Likely the name is incomplete " )

    def __nonzero__(self):
        raise RuntimeError( "No such a flag: "+ self._name+". Likely the name is incomplete " )
        


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
            raise KeyError("Duplicated flag name: %s" % name)
        
        self._flagdict[name]=CfgFlag(setDef)
        return
    
    def addFlagsCategory(self, name, generator):
        self._dynaflags[name] = generator

    def needFlagsCategory(self, name):
        self._loadDynaFlags( name )
    
    def _loadDynaFlags(self, name):
        flagBaseName = name.split('.')[0]
        if flagBaseName in self._dynaflags:
            print "dynamically loading the flag", flagBaseName
            isLocked = self._locked
            self._locked = False
            self.join( self._dynaflags[flagBaseName]() )
            self._locked = isLocked
            del self._dynaflags[flagBaseName]
            self.dump()

    def loadAllDynamicFlags(self):
        for prefix in self._dynaflags.keys():
            self._loadDynaFlags( prefix )

    def hasFlag(self, name):        
        if name in self._flagdict: 
            return True
        self._loadDynaFlags( name.split('.')[0] )
        return name in self._flagdict

    def _set(self,name,value):

        if (self._locked):
            raise RuntimeError("Attempt to set a flag of an already-locked container")
        self._loadDynaFlags( name )
        if name in self._flagdict:
            self._flagdict[name].set(value)
            return
        errString="No flag with name \'%s\' found" % name
        from difflib import get_close_matches
        closestMatch=get_close_matches(name,self._flagdict.keys(),1)
        if len(closestMatch)>0:
            errString+=". Did you mean \'%s\'?" %  closestMatch[0] 
        raise KeyError(errString)
        
    def _get(self,name):        
        self._loadDynaFlags( name )
        if name in self._flagdict:
            return self._flagdict[name].get(self)

        errString="No flag with name \'%s\' found" % name
        from difflib import get_close_matches
        closestMatch=get_close_matches(name,self._flagdict.keys(),1)
        if len(closestMatch)>0:
            errString+=". Did you mean \'%s\'?" %  closestMatch[0] 
        raise KeyError(errString)

    def __call__(self,name):
        return self.get(name)

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
            print replacedNames
            print replacementNames
            raise RuntimeError("Attempt to replace incompatible subsets: None matching flag names are " 
                               + repr(replacedNames ^ replacementNames ))

        return AthConfigFlags(newFlagDict)



    def join(self,other):
         if (self._locked):
            raise RuntimeError("Attempt to join with and already-locked container")
         for (name,flag) in other._flagdict.iteritems():
             if name in self._flagdict:
                 raise KeyError("Duplicated flag name: %s" % name)
             self._flagdict[name]=flag
         return

    def dump(self):
        print  "%-40.40s : %s" % ("Flag Name","Value")
        for name in sorted(self._flagdict):
            print "%-40.40s : %s" % (name,repr(self._flagdict[name]))

        if len(self._dynaflags) == 0:
            return
        print "Flag categories that can be loaded dynamically"        
        print  "%-15.15s : %30s : %s" % ("Category","Generator name", "Defined in" )
        for name,gen in sorted(self._dynaflags.iteritems()):
            print "%-15.15s : %30s : %s" % (name, gen.func_name, '/'.join(gen.func_code.co_filename.split('/')[-2:]))


    def initAll(self): #Mostly a self-test method
        for n,f in self._flagdict.items():
            f.get(self)
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
        self.assertFalse( self.flags.A.B.C, "Can't read A.B.C flag")
        self.flags.A.B.C = True
        self.assertTrue( self.flags.A.B.C, "Flag value not changed")


class TestWrongAccess(TestFlagsSetup):    
    def runTest(self):
        """ access to the flags below should give an exception"""
        with self.assertRaises(RuntimeError):
            print self.flags.A is True
            print self.flags.A.B == 6 
            


class TestDependentFlag(TestFlagsSetup):
    def runTest(self):
        """ The dependent flags will use another flag value to establish its own value"""
        self.flags.A.B.C= True
        self.flags.lock()
        self.assertEqual(self.flags.A.dependentFlag, "TRUE VALUE", " dependent flag setting does not work")

class TestFlagsSetupDynamic(TestFlagsSetup):
    def setUp(self):
        super(TestFlagsSetupDynamic, self).setUp()
        def theZFlags():
            nf = AthConfigFlags()
            nf.addFlag("Z.A", 7)
            nf.addFlag("Z.B", True)
            nf.addFlag("Z.C.setting", 99)
            return nf
        self.flags.addFlagsCategory( 'Z', theZFlags )        
        self.flags.dump()


class TestDynamicFlagsRead(TestFlagsSetupDynamic):
    def runTest(self):
        """ Check if dynaic flags loading works """
        self.assertEqual( self.flags.Z.A, 7, "dynamically loaded flags have wrong value")
        self.flags.dump()        

class TestDynamicFlagsSet(TestFlagsSetupDynamic):
    def runTest(self):
        """ Check if dynaic flags loading works """
        self.flags.Z.A = 15
        self.assertEqual( self.flags.Z.A, 15, "dynamically loaded flags have wrong value")
        self.flags.dump()        


if __name__ == "__main__":
    unittest.main()
