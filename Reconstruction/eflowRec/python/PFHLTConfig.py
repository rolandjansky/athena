# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


#---------------------------------------------------------------------------------#
# Tracking geometry & conditions
def TrackingGeoCfg(inputFlags):
    result = ComponentAccumulator()

    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

    # Something builds muon stations -- extrapolator?
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc, geom_svc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))

    return result

#---------------------------------------------------------------------------------#
# Calo geometry & conditions
def CaloGeoAndNoiseCfg(inputFlags):
    result = ComponentAccumulator()
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg

    result.merge(LArGMCfg(inputFlags))
    result.merge(TileGMCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    # Schedule total noise cond alg
    result.merge(CaloNoiseCondAlgCfg(inputFlags,"totalNoise"))

    return result

#---------------------------------------------------------------------------------#
# PFlow track selection
def getPFTrackSelector(inputFlags,tracksin,verticesin):

    from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapcfg = AtlasExtrapolatorCfg(inputFlags)
    extrapolator = extrapcfg.popPrivateTools()

    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator=extrapolator)

    from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool(TrackCaloExtensionTool=pcExtensionTool)

    from eflowRec.eflowRecConf import PFTrackSelector
    PFTrackSelector=PFTrackSelector("PFTrackSelector_HLT")
    PFTrackSelector.electronsName = ""
    PFTrackSelector.muonsName = ""
    PFTrackSelector.tracksName = tracksin
    PFTrackSelector.VertexContainer = verticesin
    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionTool("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool
    
    return PFTrackSelector

def getPFClusterSelectorTool(clustersin,calclustersin):

    from eflowRec.eflowRecConf import PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorTool("PFClusterSelectorTool")
    PFClusterSelectorTool.clustersName = clustersin
    PFClusterSelectorTool.calClustersName = calclustersin

    return PFClusterSelectorTool

def getPFCellLevelSelectionTool():

    from eflowRec.eflowRecConf import PFCellLevelSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionTool("PFCellLevelSubtractionTool")

    from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss

    PFCellLevelSubtractionTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss()
    PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

    from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
    MatchingTool = PFTrackClusterMatchingTool("CalObjBldMatchingTool")
    MatchingTool.OutputLevel=1
    MatchingTool_Pull_02 = PFTrackClusterMatchingTool("MatchingTool_Pull_02")
    MatchingTool_Pull_02.OutputLevel=1
    MatchingTool_Pull_015 = PFTrackClusterMatchingTool("MatchingTool_Pull_015")
    MatchingTool_Pull_015.OutputLevel=1

    MatchingTool_Pull_015.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Pull_015.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Pull_015.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Pull_015.MatchCut = 0.15*0.15 # float
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_015 = MatchingTool_Pull_015

    MatchingTool_Pull_02.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Pull_02.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Pull_02.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Pull_02.MatchCut = 0.2*0.2 # float
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = MatchingTool_Pull_02

    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = MatchingTool

    return PFCellLevelSubtractionTool

def getPFRecoverSplitShowersTool():
    from eflowRec.eflowRecConf import PFRecoverSplitShowersTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersTool("PFRecoverSplitShowersTool")

    from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
    PFRecoverSplitShowersTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")
    PFRecoverSplitShowersTool.useUpdated2015ChargedShowerSubtraction = False

    from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
    MatchingTool_Recover = PFTrackClusterMatchingTool()
    MatchingTool_Recover.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool_Recover.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool_Recover.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool_Recover.MatchCut = 0.2*0.2 # float
    PFRecoverSplitShowersTool.PFTrackClusterMatchingTool = MatchingTool_Recover

    return PFRecoverSplitShowersTool

def getPFMomentCalculatorTool():

    from eflowRec.eflowRecConf import PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorTool("PFMomentCalculatorTool")

    from CaloRec.CaloRecConf import CaloClusterMomentsMaker
    PFClusterMomentsMaker = CaloClusterMomentsMaker("PFClusterMomentsMaker")

    from AthenaCommon.SystemOfUnits import deg
    PFClusterMomentsMaker.MaxAxisAngle = 20*deg
    PFClusterMomentsMaker.WeightingOfNegClusters = False
    PFClusterMomentsMaker.MinBadLArQuality = 4000
    PFClusterMomentsMaker.TwoGaussianNoise = True
    PFClusterMomentsMaker.MomentsNames = [
       "FIRST_PHI" 
       ,"FIRST_ETA"
       ,"SECOND_R" 
       ,"SECOND_LAMBDA"
       ,"DELTA_PHI"
       ,"DELTA_THETA"
       ,"DELTA_ALPHA" 
       ,"CENTER_X"
       ,"CENTER_Y"
       ,"CENTER_Z"
       ,"CENTER_MAG"
       ,"CENTER_LAMBDA"
       ,"LATERAL"
       ,"LONGITUDINAL"
       ,"FIRST_ENG_DENS" 
       ,"ENG_FRAC_EM" 
       ,"ENG_FRAC_MAX" 
       ,"ENG_FRAC_CORE" 
       ,"FIRST_ENG_DENS" 
       ,"SECOND_ENG_DENS"
       ,"ISOLATION"
       ,"EM_PROBABILITY"
       ,"ENG_POS"
       ,"ENG_BAD_CELLS"
       ,"N_BAD_CELLS"
       ,"BADLARQ_FRAC"
       ,"AVG_LAR_Q"
       ,"AVG_TILE_Q"
       ,"SIGNIFICANCE"
    ]

    PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

    from eflowRec.eflowRecConf import PFClusterCollectionTool
    PFClusterCollectionTool_default = PFClusterCollectionTool("PFClusterCollectionTool")

    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool_default

    return PFMomentCalculatorTool

def PFCfg(inputFlags):

    result=ComponentAccumulator()

    result.merge(TrackingGeoCfg(inputFlags))
    calogeocfg = CaloGeoAndNoiseCfg(inputFlags)
    result.merge(calogeocfg)

    result.addEventAlgo( getPFTrackSelector(inputFlags,
                                            inputFlags.eflowRec.TrackColl,
                                            inputFlags.eflowRec.VertexColl) )

    #---------------------------------------------------------------------------------#
    # PFlowAlgorithm -- subtraction steps

    from eflowRec.eflowRecConf import PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm_HLT")
    PFAlgorithm.PFClusterSelectorTool = getPFClusterSelectorTool(inputFlags.eflowRec.RawClusterColl,
                                                                 inputFlags.eflowRec.CalClusterColl)

    PFAlgorithm.SubtractionToolList = [
        getPFCellLevelSelectionTool(),
        getPFRecoverSplitShowersTool(),
        ]

    pfmoments = getPFMomentCalculatorTool()
    if not inputFlags.eflowRec.DoClusterMoments:
        pfmoments.CaloClusterMomentsMaker.MomentsNames = ["CENTER_MAG"]
    PFAlgorithm.BaseToolList = [pfmoments]

    result.addEventAlgo(PFAlgorithm)

    #---------------------------------------------------------------------------------#
    # PFO creators here

    from eflowRec.eflowRecConf import PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm")
    PFOChargedCreatorAlgorithm.PFOOutputName="HLTChargedParticleFlowObjects"

    result.addEventAlgo( PFOChargedCreatorAlgorithm )

    from eflowRec.eflowRecConf import PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm")
    PFONeutralCreatorAlgorithm.PFOOutputName="HLTNeutralParticleFlowObjects"
    PFONeutralCreatorAlgorithm.DoClusterMoments=inputFlags.eflowRec.DoClusterMoments

    result.addEventAlgo( PFONeutralCreatorAlgorithm )

    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    #cfgFlags.Input.Files=["myESD.pool.root"]
    cfgFlags.Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    #
    cfgFlags.Calo.TopoCluster.doTopoClusterLocalCalib = False
    #
    cfgFlags.addFlag("eflowRec.TrackColl","InDetTrackParticles")
    cfgFlags.addFlag("eflowRec.VertexColl","PrimaryVertices")
    cfgFlags.addFlag("eflowRec.RawClusterColl","CaloTopoClusters")
    cfgFlags.addFlag("eflowRec.CalClusterColl","CaloCalTopoClustersNew")
    cfgFlags.addFlag("eflowRec.DoClusterMoments",False)
    #
    # Try to get around TRT alignment folder problem in MC
    cfgFlags.GeoModel.Align.Dynamic = False
    #
    cfgFlags.lock()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    cfg=MainServicesSerialCfg() 

    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    tccfg = CaloTopoClusterCfg(cfgFlags)
    tcalg = tccfg.getPrimary()
    tcalg.ClustersOutputName = "CaloCalTopoClustersNew"
    cfg.merge(tccfg)
    cfg.addEventAlgo(tcalg,sequenceName="AthAlgSeq")

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    cfg.merge(PFCfg(cfgFlags))

    cfg.printConfig(summariseProps=True)

    outputlist = [
        "xAOD::CaloClusterContainer#CaloCalTopoClusters*",
        "xAOD::CaloClusterAuxContainer#*CaloCalTopoClusters*Aux.",
        "xAOD::PFOContainer#*ParticleFlowObjects",
        "xAOD::PFOAuxContainer#*ParticleFlowObjectsAux."
        ]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(cfgFlags,"xAOD",ItemList=outputlist))
    from pprint import pprint
    pprint( cfg.getEventAlgo("OutputStreamxAOD").ItemList )


    cfg.getService("StoreGateSvc").Dump = True

    cfg.run(10)
