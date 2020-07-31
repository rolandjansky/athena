# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


#---------------------------------------------------------------------------------#
# Tracking geometry & conditions
def TrackingGeoCfg(inputFlags):
    result = ComponentAccumulator()

    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

    # Something builds muon stations -- extrapolator?
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    result.merge(TrackingGeometrySvcCfg(inputFlags))

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
def getHLTPFTrackSelector(inputFlags,tracksin,verticesin):

    from eflowRec.PFCfg import getPFTrackSelectorAlgorithm
    PFTrackSelector = getPFTrackSelectorAlgorithm(inputFlags,"PFTrackSelector_HLT",False)
    PFTrackSelector.electronsName = ""
    PFTrackSelector.muonsName = ""
    PFTrackSelector.tracksName = tracksin
    PFTrackSelector.VertexContainer = verticesin

    return PFTrackSelector

def getHLTPFMomentCalculatorTool(inputFlags):

    from eflowRec.PFCfg import getPFMomentCalculatorTool
    MomentsNames = [
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
    PFMomentCalculatorTool = getPFMomentCalculatorTool(inputFlags,MomentsNames)
    
    return PFMomentCalculatorTool

def PFCfg(inputFlags):

    result=ComponentAccumulator()

    result.merge(TrackingGeoCfg(inputFlags))
    calogeocfg = CaloGeoAndNoiseCfg(inputFlags)
    result.merge(calogeocfg)

    PFTrackSelector = getHLTPFTrackSelector(inputFlags,
                                            inputFlags.eflowRec.TrackColl,
                                            inputFlags.eflowRec.VertexColl)

    # Add monitoring tool
    from eflowRec import PFOnlineMon
    monTool = PFOnlineMon.getMonTool_PFTrackSelector()
    PFTrackSelector.MonTool = monTool

    result.addEventAlgo( PFTrackSelector )

    #---------------------------------------------------------------------------------#
    # PFlowAlgorithm -- subtraction steps

    PFAlgorithm=CompFactory.PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm_HLT")
    from eflowRec.PFCfg import getPFClusterSelectorTool
    PFAlgorithm.PFClusterSelectorTool = getPFClusterSelectorTool(inputFlags.eflowRec.RawClusterColl,
                                                                 inputFlags.eflowRec.CalClusterColl,"PFClusterSelectorTool_HLT")

    from eflowRec.PFCfg import getPFCellLevelSubtractionTool,getPFRecoverSplitShowersTool
    
    PFAlgorithm.SubtractionToolList = [
        getPFCellLevelSubtractionTool(inputFlags,"PFCellLevelSubtractionTool_HLT"),
        getPFRecoverSplitShowersTool(inputFlags,"PFRecoverSplitShowersTool_HLT"),
        ]

    pfmoments = getHLTPFMomentCalculatorTool(inputFlags)
    if not inputFlags.PF.useClusterMoments:
        pfmoments.CaloClusterMomentsMaker.MomentsNames = ["CENTER_MAG"]
    PFAlgorithm.BaseToolList = [pfmoments]

    monTool = PFOnlineMon.getMonTool_PFAlgorithm()
    PFAlgorithm.MonTool = monTool

    result.addEventAlgo(PFAlgorithm)

    #---------------------------------------------------------------------------------#
    # PFO creators here

    from eflowRec.PFCfg import getChargedPFOCreatorAlgorithm,getNeutralPFOCreatorAlgorithm
    result.addEventAlgo(getChargedPFOCreatorAlgorithm(inputFlags,"HLTChargedParticleFlowObjects"))
    result.addEventAlgo(getNeutralPFOCreatorAlgorithm(inputFlags,"HLTNeutralParticleFlowObjects"))    
    
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

    #PF flags
    cfgFlags.PF.useUpdated2015ChargedShowerSubtraction = True
    cfgFlags.PF.addClusterMoments = False
    cfgFlags.PF.useClusterMoments = False
    
    #
    # Try to get around TRT alignment folder problem in MC
    cfgFlags.GeoModel.Align.Dynamic = False
    #
    cfgFlags.lock()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg=MainServicesCfg(cfgFlags) 

    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    tccfg = CaloTopoClusterCfg(cfgFlags,doLCCalib=True)
    tcalg = tccfg.getPrimary()
    tcalg.ClustersOutputName = "CaloCalTopoClustersNew"
    cfg.merge(tccfg)
    cfg.addEventAlgo(tcalg,sequenceName="AthAlgSeq")

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    cfg.merge(PFCfg(cfgFlags))

    cfg.printConfig()# (summariseProps=True)

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

    histSvc = CompFactory.THistSvc(Output = ["EXPERT DATAFILE='expert-monitoring.root', OPT='RECREATE'"])
    cfg.addService(histSvc)

    cfg.getService("StoreGateSvc").Dump = True

    cfg.run(10)
