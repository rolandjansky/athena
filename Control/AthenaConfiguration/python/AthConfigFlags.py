# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from copy import deepcopy

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
        


class AthConfigFlags(object):
    def __init__(self,flagdict=dict()):
        self._flagdict=flagdict
        self._locked=False


    def addFlag(self,name,setDef=None):
        if (self._locked):
            raise RuntimeError("Attempt to add a flag to an already-locked container")

        if name in self._flagdict:
            raise KeyError("Duplicated flag name: %s" % name)
        
        self._flagdict[name]=CfgFlag(setDef)
        return


    def set(self,name,value):
        if (self._locked):
            raise RuntimeError("Attempt to set a flag of an already-locked container")
        try:
            self._flagdict[name].set(value)
        except KeyError:
            errString="No flag with name \'%s\' found" % name
            from difflib import get_close_matches
            closestMatch=get_close_matches(name,self._flagdict.keys(),1)
            if len(closestMatch)>0:
                errString+=". Did you mean \'%s\'?" %  closestMatch[0] 
            raise KeyError(errString)

    def get(self,name):
        try:
            return self._flagdict[name].get(self)
        except KeyError:
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
         for (name,flag) in other._flagdict:
             if name in self._flagdict:
                 raise KeyError("Duplicated flag name: %s" % name)
             self._flagdict[name]=flag
         return

    def dump(self):
        print  "%40.40s : %s" % ("Flag Name","Value")
        for name,flag in self._flagdict.iteritems():
            print "%40.40s : %s" % (name,repr(flag))

    def initAll(self): #Mostly a self-test method
        for n,f in self._flagdict.items():
            f.get(self)
        return
