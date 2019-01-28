from AthenaCommon.Constants import ERROR, DEBUG
from AthenaCommon.CFElements import seqAND, parOR
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

def setMinimalCaloSetup() :
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
    svcMgr+=TrigCaloDataAccessSvc()
    svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR


########################
## ALGORITHMS
########################

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
    algo=T2CaloEgamma_ReFastAlgo("FastCaloAlgo")
    algo.RoIs=inputEDM
    algo.ClustersName="L2CaloClusters" # defalut value, added for debugging
    algo.OutputLevel=OutputLevel
    return algo


####################################
##### SEQUENCES
####################################

def fastCaloRecoSequence(InViewRoIs):
    from TrigT2CaloCommon.CaloDef import algoL2Egamma
    fastCaloAlg = algoL2Egamma(OutputLevel=DEBUG,inputEDM=InViewRoIs)
    fastCaloInViewSequence = seqAND( 'fastCaloInViewSequence', [fastCaloAlg] )
    sequenceOut = fastCaloAlg.ClustersName
    return (fastCaloInViewSequence, sequenceOut)


def fastCaloEVCreator():
    InViewRoIs="EMCaloRoIs"     
    fastCaloViewsMaker = EventViewCreatorAlgorithm( "fastCaloViewsMaker", OutputLevel=DEBUG)
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.RoIsLink = "initialRoI"
    fastCaloViewsMaker.InViewRoIs = InViewRoIs
    fastCaloViewsMaker.Views = "EMCaloViews"
    fastCaloViewsMaker.ViewNodeName = "fastCaloInViewSequence"
    return (fastCaloViewsMaker, InViewRoIs)


def createFastCaloSequence(EMRoIDecisions):
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()
    # connect to RoIs
    fastCaloViewsMaker.InputMakerInputDecisions =  [ EMRoIDecisions ]         
    fastCaloViewsMaker.InputMakerOutputDecisions = [ EMRoIDecisions + "IMOUTPUT"]

    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs)
     
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, sequenceOut)


 
def HLTCaloCellRecoSequence(RoIs):
    CellsClusters = 'CellsClusters'
    algo1= algoHLTCaloCell(RoIs, outputEDM=CellsClusters, RoIMode=True, OutputLevel=DEBUG)
    algo2= algoHLTTopoCluster(inputEDM=CellsClusters, OutputLevel=DEBUG) 
    RecoSequence = parOR("ClusterRecoSequence", [algo1, algo2])
    print algo2
    for tool in algo2.ClusterMakerTools:
        print tool

    return RecoSequence
