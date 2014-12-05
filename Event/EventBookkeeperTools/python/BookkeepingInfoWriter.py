# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
    if not itemIsInList(key,dicofdic.keys()):
        dic={}
        dic["StreamName"] = StreamName
        dic["Alg"] = None
        dicofdic[key]=dic
    return

def removeFromDic(key,dic):
    if itemIsInList(key,dic.keys()):
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
                self.msg.debug("Succesfully added algorithm '%s'"%name)


            else:
                self.msg.warning("BookkeepingWriterBase cannot get algorithm '%s'. This alg will not be bookkept."%name)
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
        self.msg.info("SkimDecisionsContainer name: '%s'"%self.SkimDecisionsContainerName)
        self.sg = PyAthena.py_svc("StoreGateSvc")
        return BookkeepingWriterBase.initialize(self)

    def executeDic(self,sdc,dic,prefix):
        for key in dic.keys():
            sd = PyAthena.SkimDecision()
            sd.setName(prefix+"_"+key)
            try:
                sd.setIsAccepted(dic[key]["Alg"].filterPassed())
            except:
                self.msg.error("problem with filterPassed() for alg %s..."%key)

            sdc.push_back(sd)
            ROOT.SetOwnership (sd, False)
            self.msg.debug("Skim %s: %i (%s)"%(key,sd.isAccepted(),prefix))
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




class CutBookkeepersWriter(BookkeepingWriterBase):

    __doMC=False
    __MCEventInfoCollectionName='McEventInfo'
    __cycle='cycle'
    __cycleNumber=0


    def __init__ ( self, name="CutBookkeepersWriter", **kw ) :
        BookkeepingWriterBase.__init__(self,name,**kw)
        self.OutputCollectionName="CutBookkeepersContainer"
        self.ParentStreamName="N/A" #in principle this should be configured at python level
        return

    def initialize(self):
        self.evtCount=0
        self.evtWeightedCount=0
        if self.ParentStreamName=="N/A":
            self.msg.warning("parent stream name was not configured. It will be: '%s'."%(self.ParentStreamName))
        else:
            self.msg.info("parent stream name is: '%s'"%(self.ParentStreamName))
            pass
        self.metadata = PyAthena.py_svc("StoreGateSvc/MetaDataStore")
        self.msg.info("OutputCollectionName is '%s'"%self.OutputCollectionName)
        self.ebList=[]
        self.algList=[]
        self.sg = PyAthena.py_svc("StoreGateSvc")
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc")
            return StatusCode.Failure

        return BookkeepingWriterBase.initialize(self)

    def initDic(self,dic,prefix):
        for key in dic.keys():
            self.algList.append(dic[key]["Alg"])
            self.ebList.append(PyAthena.xAOD.CutBookkeeper_v1())
            i=len(self.ebList)-1

            #name convention could be of type cyclenumber_streamname_logic_filter(_cut)
            # For the time being we settle by logic_filter
            self.ebList[i].setName(prefix+"_"+key)
            self.ebList[i].setDescription("Number of events accepted by filter %s"%key)
            self.ebList[i].setLogic(prefix)
            self.ebList[i].setOutputStream(dic[key]["StreamName"])
            self.ebList[i].setNAcceptedEvents(0)
            self.ebList[i].setCycle(self.getCycleNumber())
            self.ebList[i].setInputStream(self.ParentStreamName)
            self.ebList[i].setSumOfEventWeights(0.0)
            self.ebc.push_back(self.ebList[i])
        return

    def execute(self):
        if self.evtCount==0:
            #could this be done in initialize?
            self.ebc    = PyAthena.xAOD.CutBookkeeperContainer_v1()
            self.ebcAux = PyAthena.xAOD.CutBookkeeperAuxContainer_v1()
            self.ebc.setStore(self.ebcAux)
            #first the total event count
            self.ebTot = PyAthena.xAOD.CutBookkeeper_v1()
            self.ebTot.setName("AllEvents")
            self.ebTot.setDescription("Number of events processed.")
            self.ebTot.setNAcceptedEvents(0)
            self.ebTot.setSumOfEventWeights(0.0)
            self.ebTot.setSumOfEventWeightsSquared(0.0)
            self.ebc.push_back(self.ebTot)
            #then the filter algs
            self.initDic(self._RequireAlgDic,"Require")
            self.initDic(self._AcceptAlgDic,"Accept")
            self.initDic(self._VetoAlgDic,"Veto")
            self.initDic(self._OtherAlgDic,"Other")
            self.metadata.record(self.ebc,self.OutputCollectionName)
            self.metadata.record(self.ebcAux,self.OutputCollectionName+"Aux.")
            #sanity check...
            if len(self.ebList) != len(self.algList):
                raise IndexError("ebList and algList must have the same lenght. There must be a problem...")
            pass

        self.evtCount+=1

        myMCEventInfo = None

        #don't try and find this collection in real data
        if True == self.__doMC:
            try:
                myMCEventInfo = self.sg.retrieve('EventInfo', self.__MCEventInfoCollectionName)
            except LookupError:
                if self.evtCount <100:
                    self.msg.warning( 'McEventInfoCollection with name ' +self.__MCEventInfoCollectionName+ ' not found' )

        #for samples without event weights (e.g pythia, real data) set the weight to 1.0 so we get a sensible number
        #of weighted events (in fact the same number as unweighted events).
        #Two ways this can happen...

        # - the McEventInfo collection was not in the pool file

        if myMCEventInfo:
            myEventType = myMCEventInfo.event_type()
            mcNLOWeight = myEventType.mc_event_weight();
        else:
            mcNLOWeight = 1.0

        # - the McEventInfo collection was in the pool file, but all weights are zero (e.g. in pythia).

        if 0.0 == mcNLOWeight:
            mcNLOWeight = 1.0

        self.evtWeightedCount+=mcNLOWeight

        self.ebTot.setNAcceptedEvents(self.evtCount)
        self.ebTot.setSumOfEventWeights(self.evtWeightedCount)
        i=0
        while i < len(self.ebList):
            try:
                if self.algList[i].filterPassed():
                    self.ebList[i].addNAcceptedEvents(1)
                    self.ebList[i].addSumOfEventWeights(mcNLOWeight)
            except:
                self.msg.error("problem with alg filterPassed()...")
            i+=1
        return StatusCode.Success

    def finalize(self):
        self.msg.info("#----------------------------------------------------")
        self.msg.info("Summary of CutBookkeepers")
        try:
            self.msg.info("%s: %i"%(self.ebTot.getDescription(),self.ebTot.getNAcceptedEvents()))
            for eb in self.ebList:
                self.msg.info("%s: %i"%(eb.getDescription(),eb.getNAcceptedEvents()))
        except:
            self.msg.warning("Not able to print a summary! It's probably because zero events were processed.")
        self.msg.info("#----------------------------------------------------")
        return StatusCode.Success

    def setDoMC(self,doMC):
        self.__doMC=doMC

    def setMCEventInfoCollectionName(self,MCEventInfoCollectionName):
        self.__MCEventInfoCollectionName=MCEventInfoCollectionName


    def setCycle(self,cycle):
        self.__cycle+=str(cycle)
        self.__cycleNumber=cycle

    def getCycle(self):
        cycle =self.__cycle
        return cycle

    def getCycleNumber(self):
        cycleNum =self.__cycleNumber
        return cycleNum





class CutCycleWriter( PyAthena.Alg ):

    __cycleNumber=0

    def __init__ ( self, name="CutCycleWriter", **kw ) :
        kw['name'] = name
        super(CutCycleWriter,self).__init__(**kw)
        self.OutputName   = kw.get('OutputName', "ProcessingCycle")
        self.CurrentCycle = kw.get('CurrentCycle', 0)
        return

    def initialize(self):
        self.doneWriting = False
        self.metadata = PyAthena.py_svc("StoreGateSvc/MetaDataStore")
        if not self.metadata:
            self.msg.error("Could not retrieve StoreGateSvc/MetaDataStore")
            return StatusCode.Failure
        # Build the output name
        self.OutputName += str(self.CurrentCycle)
        self.msg.info("OutputName is '%s'"%self.OutputName)
        return StatusCode.Success

    def execute(self):
        if not self.doneWriting:
            #could this be done in initialize?
            #cycleCounter = int(self.CurrentCycle)
            # cycleCounter = PyAthena.IOVMetaDataContainer()
            # cycleCounter = PyAthena.EventBookkeeperCollection()
            cycleCounter = PyAthena.xAOD.CutBookkeeperContainer_v1()
            self.metadata.record(cycleCounter,self.OutputName)
            self.metadata.dump()
            self.doneWriting = True
            pass
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success
