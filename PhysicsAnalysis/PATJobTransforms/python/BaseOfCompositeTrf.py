#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyJobTransformsCore.trf import JobTransform
from PATJobTransforms.ArgDicTools import DefaultConfigFromSysArgv

BasicCommonSteps=["any","none","first"]

class BaseOfCompositeTrf( JobTransform ):
    def __init__(self,inputDic,ProdSysKeys,name,authors,help,lastInChain=True):
        JobTransform.__init__(self, name=name, authors=authors, help=help, lastInChain=lastInChain)

        if not isinstance(inputDic,dict):
            raise TypeError("inputDic has %s but should be a dictionary." %type(inputDic))

        #Help?
        if '-h' in inputDic or '--help' in inputDic:
            print "\nThousands of parameter combinations can be generated dynamically by this transform."
            print "See: https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecoTrf"
            print "\nThis design has one exception: ProdSys (Tier1), which can only use keys:"
            print ProdSysKeys
            print "\nTo bypass an argument in positional mode, give value: 'NONE'."
            print "Please use CmdToPositional.py to translate a key=val command into a positional one.\n\n"

        #Default configuration?
        if inputDic.has_key('defaultFromPositionalValues'):
            inputDic=DefaultConfigFromSysArgv(ProdSysKeys,inputDic)

        self.inDic=inputDic
        self.ValidSubStepsDic={}
        self.ValidSubStepsRunFcts={}
        return

    def ConfigureCommonInternalSubSteps(self):
        from PATJobTransforms.Configuration import ConfigDic
            
        #Loop over all inputDic keys
        for key in self.inDic.keys():
            newKey,subStep=self.GetKeyAndSubStep(key)
            if not ConfigDic.has_key(newKey):
                raise RuntimeError("key '%s' is not defined in ConfigDic"%newKey)

            #Set subSteps from above if specified, otherwise use default value from ConfigDic
            subSteps=[subStep]
            if subStep==None:
                subSteps=[]
                tmpSteps=ConfigDic[newKey].subSteps
                for t in tmpSteps:
                    if BasicCommonSteps.__contains__(t) or self.ValidSubStepsDic.keys().__contains__(t):
                        subSteps.append(t)

            meth="update"
            #meth="create" <--- problematic, leave it off (OK)                
            
            #Now configure the sub-step(s) for this key
            for step in subSteps:
                if step=="any":
                    self.AddToAllDics(newKey,self.inDic[key],meth)
                elif step=="none":
                    #these are typically for Reco_trf only configuration (e.g. DBRelease)
                    ConfigDic[newKey](self,self.inDic)
                elif step=="first":
                    #handle this later, when the input/output configuration is known.
                    pass
                elif self.ValidSubStepsDic.has_key(step):
                    self.AddToDic(self.ValidSubStepsDic[step],newKey,self.inDic[key],meth)
                else:
                    raise RuntimeError("Unexpected sub-step '%s' for key '%s'"%(step,newKey))

        return

    def CommonSpecialFinalConfig(self):
        #These are semi-hacks... please minimize the stuff here!
        if self.inDic.has_key('skipEvents'):
            skip=self.inDic['skipEvents']
            print "INFO skipEvents is special. It is only added to the first sub-step(s)."
            firstSteps=self.GetFirstSubStep()
            if firstSteps==None:
                raise RuntimeError("skipEvents requested, but can't find which sub-step it should be added to.")

            for step in firstSteps:
                self.AddToDic(self.ValidSubStepsDic[step],'skipEvents',skip,"create")
                print "skipEvents=%i added to %s."%(int(skip),step)

        #Remove duplicate outputs, giving precedence to the first running step of the substeps list from ConfigDic 
        from PATJobTransforms.Configuration import ConfigDic
        for argKey in ConfigDic.keys():
            argObj=ConfigDic[argKey]
            if hasattr(argObj,"isOutput") and hasattr(argObj,"subSteps"):
                steps=argObj.subSteps
                alreadyProduced=False
                for step in steps:
                    if self.SubStepIsExecuted(step) and self.ValidSubStepsDic[step].has_key(argKey):
                        if alreadyProduced:
                            self.ValidSubStepsDic[step].pop(argKey)
                        else:
                            alreadyProduced=True
        return

    def hasOutput(self,dic):
        from PATJobTransforms.Configuration import ConfigDic
        for key in dic.keys():
            if ConfigDic.has_key(key):
                if hasattr(ConfigDic[key],"isOutput"):
                    return True
                else:
                    pass
            else:
                raise RuntimeError("key %s is not defined in ConfigDic"%key)
            pass
        return False
    
    def hasInput(self,dic):
        from PATJobTransforms.Configuration import ConfigDic
        for key in dic.keys():
            if ConfigDic.has_key(key):
                if hasattr(ConfigDic[key],"isInput"):
                    return True
                else:
                    pass
            else:
                raise RuntimeError("key %s is not defined in ConfigDic"%key)
            pass
        return False
 
    def GetKeyAndSubStep(self,key):
        #This method is to translate e.g. key=preExec_r2e into newKey=preExec,step=r2e 
        for step in self.ValidSubStepsDic.keys():
            #is this key only for a specific sub-step?
            if key.endswith("_"+step):
                ind=len(key)-len(step)-1
                newKey=key[:ind]
                print "%s will be only passed to step %s as %s"%(key,step,newKey)
                return newKey,step
            pass        
        #At this point, key is to be used by any sub-step 
        return key,None

    def AddToDic(self,dic,key,val,meth):
        if not dic.has_key(key):
            dic[key]=val
        else:
            if meth=="update":
                newVal=val+",,"+dic[key]
                dic[key]=newVal
            else:
                raise RuntimeError("Key '%s' is already defined in the current dictionary and cannot be overwritten."%(key))
            return

    def AddToAllDics(self,newKey,val,meth):
        for key in self.ValidSubStepsDic.keys():
            dic=self.ValidSubStepsDic[key]
            self.AddToDic(dic,newKey,val,meth)
        return

    def GetFirstSubStep(self):
        raise RuntimeError("Virtual method GetFirstSubStep() needs to be implemented in concrete composite transform.")

    def AddNewSubStep(self,key,runSubStepFct=None):
        if self.ValidSubStepsDic.has_key(key):
            raise RuntimeError("Sub-step '%s' is already defined.")
        self.ValidSubStepsDic[key]={}
        self.ValidSubStepsRunFcts[key]=runSubStepFct
        return self.ValidSubStepsDic[key]

    def SubStepIsExecuted(self,stepKey):
        if self.ValidSubStepsRunFcts.has_key(stepKey):
            if self.ValidSubStepsRunFcts[stepKey]!=None:
                return self.ValidSubStepsRunFcts[stepKey]()
            pass
        return False

    def Print(self):
        for key in self.ValidSubStepsDic.keys():
            print "----------------------------"
            print "key"
            print self.ValidSubStepsDic[key]
            print "----------------------------"
        return

    def getPotentialArgs(self):
        from PyJobTransformsCore.trfutil import AddValidItemToList,OverlapLists
        trfSteps=self.ValidSubStepsDic.keys()
        AddValidItemToList(BasicCommonSteps,trfSteps)
        trfArgs=[]

        from PATJobTransforms.Configuration import ConfigDic
        for key in ConfigDic.keys():
            if hasattr(ConfigDic[key],"subSteps"):
                keySteps=ConfigDic[key].subSteps
                if OverlapLists(trfSteps,keySteps):
                    trfArgs.append(key)
                    pass
                pass
            else:
                trfArgs.append(key)
                pass
            pass
        return trfArgs

