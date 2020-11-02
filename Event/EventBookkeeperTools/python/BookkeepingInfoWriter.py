# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (DESY), February 2009

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import ROOT

def itemIsInList(item,aList):
    """
    Helper function to check if a certain item is part of a list.
    """
    # Make sure aList is a list
    if not isinstance(aList, list):
        raise TypeError("BookkeepingInfoWriter.itemIsInList() does not support aList of type %s"%type(aList))
    isInList=False
    for i in aList:
        if i==item:
            isInList=True
            pass
        pass
    return isInList


def addToDic(key,StreamName,dicofdic):
    if not itemIsInList(key,list(dicofdic.keys())):
        dic={}
        dic["StreamName"] = StreamName
        dic["Alg"] = None
        dicofdic[key]=dic
    return

def removeFromDic(key,dic):
    if itemIsInList(key,list(dic.keys())):
        dic.pop(key)
    return


class BookkeepingWriterBase( PyAthena.Alg ):
    def __init__ ( self, name, **kw ) :
        kw['name'] = name
        super(BookkeepingWriterBase,self).__init__(**kw)
        self._RequireAlgDic={}
        self._AcceptAlgDic={}
        self._VetoAlgDic={}
        self._OtherAlgDic={}

    def reset(self):
        self._RequireAlgDic={}
        self._AcceptAlgDic={}
        self._VetoAlgDic={}
        self._OtherAlgDic={}

    def addRequireAlg(self,algName,StreamName=""):
        addToDic(algName,StreamName,self._RequireAlgDic)
        return

    def addAcceptAlg(self,algName,StreamName=""):
        addToDic(algName,StreamName,self._AcceptAlgDic)
        return

    def addVetoAlg(self,algName,StreamName=""):
        addToDic(algName,StreamName,self._VetoAlgDic)
        return

    def addOtherAlg(self,algName,StreamName=""):
        addToDic(algName,StreamName,self._OtherAlgDic)
        return

    def removeRequireAlg(self,algName):
        removeFromDic(algName,self._RequireAlgDic)
        return

    def removeAcceptAlg(self,algName):
        removeFromDic(algName,self._AcceptAlgDic)
        return

    def removeVetoAlg(self,algName):
        removeFromDic(algName,self._VetoAlgDic)
        return

    def removeOtherAlg(self,algName):
        removeFromDic(algName,self._OtherAlgDic)
        return

    def initializeDic(self,dic):
        for name in dic.keys():
            alg=PyAthena.py_alg(name,iface='Algorithm')
            if alg:
                pass
            else:
                alg=PyAthena.py_alg(name,iface='IAlgorithm')

            if alg:
                dic[name]["Alg"]=alg
                self.msg.debug("Succesfully added algorithm '%s'", name)


            else:
                self.msg.warning("BookkeepingWriterBase cannot get algorithm '%s'. This alg will not be bookkept.", name)
                dic.pop(name)
        return

    def initialize(self):
        self.initializeDic(self._RequireAlgDic)
        self.initializeDic(self._AcceptAlgDic)
        self.initializeDic(self._VetoAlgDic)
        self.initializeDic(self._OtherAlgDic)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

#---------------------------------------------------------
class SkimDecisionsWriter(BookkeepingWriterBase):
    def __init__ ( self, name="SkimDecisionsWriter", **kw ) :
        BookkeepingWriterBase.__init__(self,name,**kw)
        self.SkimDecisionsContainerName="SkimDecisionsContainer"
        return

    def initialize(self):
        self.msg.info("SkimDecisionsContainer name: '%s'", self.SkimDecisionsContainerName)
        self.sg = PyAthena.py_svc("StoreGateSvc")
        return BookkeepingWriterBase.initialize(self)

    def executeDic(self,sdc,dic,prefix):
        for key in dic.keys():
            sd = PyAthena.SkimDecision()
            sd.setName(prefix+"_"+key)
            try:
                sd.setIsAccepted(dic[key]["Alg"].filterPassed())
            except Exception:
                self.msg.error("problem with filterPassed() for alg %s...", key)

            sdc.push_back(sd)
            ROOT.SetOwnership (sd, False)
            self.msg.debug("Skim %s: %i (%s)", key, sd.isAccepted(), prefix)
        return

    def execute(self):
        sdc = PyAthena.SkimDecisionCollection()
        self.executeDic(sdc,self._RequireAlgDic,"Require")
        self.executeDic(sdc,self._AcceptAlgDic,"Accept")
        self.executeDic(sdc,self._VetoAlgDic,"Veto")
        self.executeDic(sdc,self._OtherAlgDic,"Other")
        self.sg.record(sdc,self.SkimDecisionsContainerName)
        ROOT.SetOwnership (sdc, False)
        return StatusCode.Success
