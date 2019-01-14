from AthenaCommon.Constants import ERROR, DEBUG
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

def setMinimalCaloSetup() :

  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
    svcMgr+=TrigCaloDataAccessSvc()
    svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR


def algoHLTCaloCell(inputEDM='FSRoI', outputEDM='CellsClusters', RoIMode=True, OutputLevel=ERROR) :
   setMinimalCaloSetup();
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
   algo=HLTCaloCellMaker("HLTCaloCellMaker");
   algo.RoIs=inputEDM
   algo.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
   algo.OutputLevel=OutputLevel
   algo.CellsName=outputEDM
   return algo;

def algoHLTTopoCluster(inputEDM="CellsClusters", OutputLevel=ERROR) :
   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
   algo = TrigCaloClusterMakerMT_topo(doMoments=True, doLC=False, cells=inputEDM)
   algo.OutputLevel=OutputLevel
   return algo

def algoL2Egamma(inputEDM="EMRoIs",OutputLevel=ERROR):
   setMinimalCaloSetup();
   from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
   algo=T2CaloEgamma_ReFastAlgo("testReFastAlgo")
   algo.RoIs=inputEDM
   algo.OutputLevel=OutputLevel
   return algo

def createFastCaloSequence():

     InViewRoIs="EMCaloRoIs"

     from TrigT2CaloCommon.CaloDef import algoL2Egamma
     fastCaloInViewSequence = seqAND( 'fastCaloInViewSequence', [algoL2Egamma(OutputLevel=DEBUG,inputEDM=InViewRoIs)] )
     fastCaloViewsMaker = EventViewCreatorAlgorithm( "fastCaloViewsMaker", OutputLevel=DEBUG)
     fastCaloViewsMaker.ViewFallThrough = True
     fastCaloViewsMaker.InputMakerInputDecisions =  [ "FilteredEMRoIDecisions" ]
     fastCaloViewsMaker.RoIsLink = "initialRoI"
     fastCaloViewsMaker.InViewRoIs = InViewRoIs
     fastCaloViewsMaker.Views = "EMCaloViews"
     fastCaloViewsMaker.ViewNodeName = "fastCaloInViewSequence"
     fastCaloViewsMaker.InputMakerOutputDecisions = [ "L2CaloLinks"]

     fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
     return fastCaloSequence
