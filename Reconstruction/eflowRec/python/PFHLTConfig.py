# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


#---------------------------------------------------------------------------------#
# Tracking geometry & conditions
def TrackingGeoCfg(inputFlags):
    result = ComponentAccumulator()

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

def PFExtrapolatorCfg(flags):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    result = ComponentAccumulator()
    extrapolator = result.popToolsAndMerge(InDetExtrapolatorCfg(flags, "InDetTrigExtrapolator"))
    result.setPrivateTools(
        CompFactory.Trk.ParticleCaloExtensionTool(
            "HLTPF_ParticleCaloExtension",
            Extrapolator=extrapolator,
        )
    )
    return result

def PFTrackExtensionCfg(flags, tracktype, tracksin):
    """ Get the track-to-calo extension after a preselection

    Returns the component accumulator, the preselected track collection and the extension cache
    """
    result = ComponentAccumulator()
    pretracks_name = f"HLTPFPreselTracks_{tracktype}"
    cache_name = f"HLTPFtrackExtensionCache_{tracktype}"

    result.addEventAlgo(CompFactory.PFTrackPreselAlg(
        f"HLTPFTrackPresel_{tracktype}",
        InputTracks=tracksin,
        OutputTracks=pretracks_name,
        TrackSelTool=CompFactory.InDet.InDetTrackSelectionTool(
            CutLevel="TightPrimary",
            minPt=500.0,
        )
    ))
    result.addEventAlgo(CompFactory.Trk.PreselCaloExtensionBuilderAlg(
        f"HLTPFTrackExtension_{tracktype}",
        ParticleCaloExtensionTool=result.popToolsAndMerge(PFExtrapolatorCfg(flags)),
        InputTracks=pretracks_name,
        OutputCache=cache_name,
    ))

    return result, pretracks_name, cache_name

def MuonCaloTagCfg(flags, tracktype, tracksin, extcache, cellsin):
    """ Create the muon calo tagging configuration
    
    Return the component accumulator and the tracks with muons removed
    """
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    result = ComponentAccumulator()
    extrapolator = result.popToolsAndMerge(InDetExtrapolatorCfg(flags, "InDetTrigExtrapolator"))
    output_tracks = f"PFMuonCaloTagTracks_{tracktype}"

    result.addEventAlgo(
        CompFactory.PFTrackMuonCaloTaggingAlg(
            f"PFTrackMuonCaloTaggingAlg_{tracktype}",
            InputTracks = tracksin,
            InputCaloExtension = extcache,
            InputCells = cellsin,
            OutputTracks = output_tracks,
            MinPt = flags.Trigger.FSHad.PFOMuonRemovalMinPt,
            MuonScoreTool = CompFactory.CaloMuonScoreTool(
                CaloMuonEtaCut=3,
                ParticleCaloCellAssociationTool = CompFactory.Rec.ParticleCaloCellAssociationTool(
                    ParticleCaloExtensionTool = result.popToolsAndMerge(PFExtrapolatorCfg(flags)),
                    CaloCellContainer="",
                )
            ),
            LooseTagTool=CompFactory.CaloMuonTag("LooseCaloMuonTag", TagMode="Loose"),
            TightTagTool=CompFactory.CaloMuonTag("TightCaloMuonTag", TagMode="Tight"),
            DepositInCaloTool=CompFactory.TrackDepositInCaloTool(
                ExtrapolatorHandle=extrapolator,
                ParticleCaloCellAssociationTool=CompFactory.Rec.ParticleCaloCellAssociationTool(
                    ParticleCaloExtensionTool=result.popToolsAndMerge(PFExtrapolatorCfg(flags)),
                    CaloCellContainer=""
                ),
                ParticleCaloExtensionTool=result.popToolsAndMerge(PFExtrapolatorCfg(flags)),
            )
        ),
        primary=True,
    )
    return result, output_tracks

def MuonIsoTagCfg(flags, tracktype, tracksin, verticesin, extcache, clustersin):
    """ Create the muon iso tagging configuration
    
    Return the component accumulator and the tracks with muons removed
    """
    result = ComponentAccumulator()
    output_tracks = f"PFMuonIsoTagTracks_{tracktype}"
    result.addEventAlgo(
        CompFactory.PFTrackMuonIsoTaggingAlg(
            f"PFTrackMuonIsoTaggingalg_{tracktype}",
            InputTracks = tracksin,
            InputClusters = clustersin,
            InputVertices = verticesin,
            OutputTracks = output_tracks,
            MinPt = flags.Trigger.FSHad.PFOMuonRemovalMinPt,
            TrackIsoTool = CompFactory.xAOD.TrackIsolationTool(
                TrackParticleLocation=tracksin,
                VertexLocation="",
            ),
            CaloIsoTool = CompFactory.xAOD.CaloIsolationTool(
                ParticleCaloExtensionTool=result.popToolsAndMerge(PFExtrapolatorCfg(flags)),
                InputCaloExtension=extcache,
                ParticleCaloCellAssociationTool="",
                saveOnlyRequestedCorrections=True,
            )
        ),
        primary=True,
    )
    return result, output_tracks

#---------------------------------------------------------------------------------#
# PFlow track selection
def HLTPFTrackSelectorCfg(inputFlags,tracktype,tracksin,verticesin,clustersin,cellsin=None):
    from eflowRec import PFOnlineMon
    result = ComponentAccumulator()

    muon_mode = inputFlags.Trigger.FSHad.PFOMuonRemoval
    if muon_mode == "None":
        tracks = tracksin
        extension_cache=""
    else:
        ext_acc, pretracks, extension_cache = PFTrackExtensionCfg(
            inputFlags, tracktype, tracksin
        )
        result.merge(ext_acc)
        if muon_mode == "Calo":
            if cellsin is None:
                raise ValueError("Cells must be provided for the 'Calo' muon mode!")
            tag_acc, tracks = MuonCaloTagCfg(
                inputFlags, tracktype, tracksin, extension_cache, cellsin
            )
        elif muon_mode == "Iso":
            tag_acc, tracks = MuonIsoTagCfg(
                inputFlags, tracktype, tracksin, verticesin, extension_cache, clustersin
            )
        else:
            raise ValueError(f"Invalid muon removal mode '{muon_mode}'")
        result.merge(tag_acc)

    result.addEventAlgo(
        CompFactory.PFTrackSelector(
            f"PFTrackSelector_{tracktype}",
            trackExtrapolatorTool = CompFactory.eflowTrackCaloExtensionTool(
                "HLTPF_eflowTrkCaloExt",
                TrackCaloExtensionTool=result.popToolsAndMerge(PFExtrapolatorCfg(inputFlags)),
                PFParticleCache = extension_cache,
            ),
            trackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool(
                CutLevel="TightPrimary",
                minPt = 500.0,
            ),
            electronsName="",
            muonsName="",
            tracksName=tracksin,
            VertexContainer=verticesin,
            eflowRecTracksOutputName=f"eflowRecTracks_{tracktype}",
            MonTool = PFOnlineMon.getMonTool_PFTrackSelector(),
        ),
        primary=True,
    )

    return result

def getHLTPFMomentCalculatorTool(inputFlags):
    result = ComponentAccumulator()
    from eflowRec.PFCfg import getPFMomentCalculatorTool

    if inputFlags.PF.useClusterMoments:
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
    else:
        MomentsNames = ["CENTER_MAG"]
    PFMomentCalculatorTool = result.popToolsAndMerge(getPFMomentCalculatorTool(inputFlags,MomentsNames))
    result.setPrivateTools(PFMomentCalculatorTool)
    return result

def PFCfg(inputFlags, tracktype="", clustersin=None, calclustersin=None, tracksin=None, verticesin=None, cellsin=None):

    result=ComponentAccumulator()

    # Set defaults for the inputs
    if clustersin is None:
        clustersin=inputFlags.eflowRec.RawClusterColl
    if calclustersin is None:
        calclustersin=inputFlags.eflowRec.CalClusterColl
    if tracksin is None:
        tracksin = inputFlags.eflowRec.TrackColl
    if verticesin is None:
        verticesin = inputFlags.eflowRec.VertexColl

    result.merge(TrackingGeoCfg(inputFlags))
    calogeocfg = CaloGeoAndNoiseCfg(inputFlags)
    result.merge(calogeocfg)

    selcfg = HLTPFTrackSelectorCfg(inputFlags, tracktype, tracksin, verticesin, clustersin, cellsin)
    PFTrackSelector = selcfg.getPrimary()

    # Add monitoring tool
    from eflowRec import PFOnlineMon
    monTool = PFOnlineMon.getMonTool_PFTrackSelector()
    PFTrackSelector.MonTool = monTool

    result.merge( selcfg )

    #---------------------------------------------------------------------------------#
    # PFlowAlgorithm -- subtraction steps


    from eflowRec.PFCfg import getPFClusterSelectorTool
    from eflowRec.PFCfg import getPFCellLevelSubtractionTool,getPFRecoverSplitShowersTool

    result.addEventAlgo(
        CompFactory.PFAlgorithm(
            f"PFAlgorithm_HLT{tracktype}",
            PFClusterSelectorTool = getPFClusterSelectorTool(
                clustersin,
                calclustersin,
                f"PFClusterSelectorTool_HLT{tracktype}",
            ),
            SubtractionToolList = [
                getPFCellLevelSubtractionTool(
                    inputFlags,
                    f"PFCellLevelSubtractionTool_HLT{tracktype}",
                ),
                getPFRecoverSplitShowersTool(
                    inputFlags,
                    f"PFRecoverSplitShowersTool_HLT{tracktype}"
                )
            ],
            BaseToolList = [
                result.popToolsAndMerge(getHLTPFMomentCalculatorTool(inputFlags)),
            ],
            MonTool = PFOnlineMon.getMonTool_PFAlgorithm(),
            eflowRecTracksInputName = PFTrackSelector.eflowRecTracksOutputName,
            eflowRecClustersOutputName = f"eflowRecClusters_{tracktype}",
            PFCaloClustersOutputName = f"PFCaloCluster_{tracktype}",
            eflowCaloObjectsOutputName = f"eflowCaloObjects_{tracktype}",
        )
    )

    #---------------------------------------------------------------------------------#
    # PFO creators here

    from eflowRec.PFCfg import getChargedPFOCreatorAlgorithm,getNeutralPFOCreatorAlgorithm
    result.addEventAlgo(getChargedPFOCreatorAlgorithm(
        inputFlags,
        f"HLT_{tracktype}ChargedParticleFlowObjects",
        f"eflowCaloObjects_{tracktype}"
    ))
    result.addEventAlgo(getNeutralPFOCreatorAlgorithm(
        inputFlags,
        f"HLT_{tracktype}NeutralParticleFlowObjects",
        f"eflowCaloObjects_{tracktype}"
    ))    
    
    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    #cfgFlags.Input.Files=["myESD.pool.root"]
    cfgFlags.Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
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
    tccfg = CaloTopoClusterCfg(cfgFlags)
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
