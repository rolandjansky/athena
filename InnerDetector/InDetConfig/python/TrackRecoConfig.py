# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def BCM_ZeroSuppressionCfg(flags, name="InDetBCM_ZeroSuppression", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name=name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def PixelClusterizationCfg(flags, name = "InDetPixelClusterization", **kwargs) :
    acc = ComponentAccumulator()
    merged_pixels_tool = acc.getPrimaryAndMerge(MergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.getPrimaryAndMerge(PixelGangedAmbiguitiesFinderCfg(flags))

    # Region selector tools for Pixel
    from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
    RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "PixelRDOs")
    kwargs.setdefault("ClustersName", "PixelClusters")
    kwargs.setdefault("RegSelTool", RegSelTool_Pixel)

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name=name, **kwargs))
    return acc
##------------------------------------------------------------------------------
def PixelClusterizationPUCfg(flags, name="InDetPixelClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationCfg(flags, name="InDetSCT_Clusterization", **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False))

    #### Clustering tool ######
    InDetClusterMakerTool = acc.getPrimaryAndMerge(ClusterMakerToolCfg(flags))
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool( name           = "InDetSCT_ClusteringTool",
                                                                    globalPosAlg   = InDetClusterMakerTool,
                                                                    conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    kwargs.setdefault("clusteringTool", InDetSCT_ClusteringTool)
    kwargs.setdefault("DataObjectName", 'SCT_RDOs') ##InDetKeys.SCT_RDOs()
    kwargs.setdefault("ClustersName", 'SCT_Clusters') ##InDetKeys.SCT_Clusters()
    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name=name, **kwargs))

    return acc

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationPUCfg(flags, name="InDetSCT_ClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters")  #flags.InDetKeys.SCT_PU_Clusters
    return SCTClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
def PixelGangedAmbiguitiesFinderCfg(flags) :
    acc = ComponentAccumulator()

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    acc.merge(PixelGeometryCfg(flags))

    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder, primary=True)
    return acc

##------------------------------------------------------------------------------
def MergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      kwargs.setdefault("globalPosAlg", acc.getPrimaryAndMerge(ClusterMakerToolCfg(flags)) )

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
      kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)) ) 

      # Enable duplcated RDO check for data15 because duplication mechanism was used.
      if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
          kwargs.setdefault("CheckDuplicatedRDO", True )

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "InDetMergedPixelsTool", **kwargs)
     
      acc.addPublicTool(InDetMergedPixelsTool, primary=True)
      return acc

##------------------------------------------------------------------------------
def ClusterMakerToolCfg(flags, name="InDetClusterMakerTool", **kwargs) :
    from PixelConditionsAlgorithms.PixelConditionsConfig import (PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg, PixelDeadMapCondAlgCfg, 
                                                                 PixelOfflineCalibCondAlgCfg, PixelCablingCondAlgCfg, PixelReadoutSpeedAlgCfg)

    acc = ComponentAccumulator()
    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge( PixelConfigCondAlgCfg(flags))
    acc.merge(PixelDeadMapCondAlgCfg(flags))
    acc.merge( PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
    acc.merge(PixelCablingCondAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = name, **kwargs)

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )    
    acc.addPublicTool(SCTLorentzAngleTool)
    acc.addPublicTool(InDetClusterMakerTool, primary=True)
    return acc


def TrackToVertexCfg(flags, name="TrackToVertex", **kwargs):
    result = ComponentAccumulator()
    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs["Extrapolator"] = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg

    result.merge(BeamSpotCondAlgCfg(flags))
    result.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return result

def TrackParticleCreatorToolCfg(flags, name="TrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.popToolsAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
        result.addPublicTool(TrackSummaryTool)
        kwargs["TrackSummaryTool"] = TrackSummaryTool
    p_expr = flags.InDet.perigeeExpression
    kwargs.setdefault("BadClusterID", flags.InDet.pixelClusterBadClusterID)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", flags.InDet.KeepFirstParameters)
    kwargs.setdefault(
        "PerigeeExpression",
        p_expr if p_expr != "Vertex" else "BeamLine")
    result.addPublicTool(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs), primary = True)
    return result

def TrackCollectionCnvToolCfg(flags, name="TrackCollectionCnvTool", TrackParticleCreator = None):
    result = ComponentAccumulator()
    if TrackParticleCreator is None:
        TrackParticleCreator = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(
        name,
        TrackParticleCreator=TrackParticleCreator,
    ))
    return result

def TrackParticleCnvAlgCfg(flags, name="TrackParticleCnvAlg", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("xAODContainerName", OutputTrackParticleContainer)
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", OutputTrackParticleContainer)
    if "TrackParticleCreator" not in kwargs:
        kwargs["TrackParticleCreator"] = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    if "TrackCollectionCnvTool" not in kwargs:
        kwargs["TrackCollectionCnvTool"] = result.popToolsAndMerge(TrackCollectionCnvToolCfg(
            flags,
            TrackParticleCreator=kwargs["TrackParticleCreator"],
        ))
    
    if flags.InDet.doTruth:
        if not kwargs.get("TrackTruthContainerName", None):
            kwargs.setdefault("AddTruthLink", False)
        else:
            kwargs.setdefault("AddTruthLink", True)
            if "MCTruthClassifier" not in kwargs:
                from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
                kwargs["MCTruthClassifier"] = result.popToolsAndMerge(MCTruthClassifierCfg(flags))
    else:
        kwargs.setdefault("AddTruthLink", False)
    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def TrackRecoCfg(flags):
    """Configures complete ID tracking """
    result = ComponentAccumulator()

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    result.merge( PixelGeometryCfg(flags))

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    result.merge( SCT_GeometryCfg(flags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    result.merge(TRT_GeometryCfg(flags))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(flags))

    #TODO move these to a more appropriate place

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import (PixelChargeCalibCondAlgCfg, PixelOfflineCalibCondAlgCfg, PixelDistortionAlgCfg)
    result.merge(PixelChargeCalibCondAlgCfg(flags))
    result.merge(PixelOfflineCalibCondAlgCfg(flags))
    result.merge(PixelDistortionAlgCfg(flags))
    from InDetConfig.TrackingCommonConfig import PixelClusterNnCondAlgCfg, PixelClusterNnWithTrackCondAlgCfg
    result.merge(PixelClusterNnCondAlgCfg(flags))    
    result.merge(PixelClusterNnWithTrackCondAlgCfg(flags))

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    result.addPublicTool(PixelLorentzAngleTool(flags))
    result.addPublicTool(result.popToolsAndMerge(PixelLorentzAngleCfg(flags)))

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    result.addPublicTool(result.popToolsAndMerge(SCT_LorentzAngleCfg(flags)))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    result.merge(PixelHitDiscCnfgAlgCfg(flags))
    if flags.Input.Format == "BS":
        from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
        result.merge(PixelRawDataProviderAlgCfg(flags))
        from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTRawDataProviderCfg, SCTEventFlagWriterCfg
        result.merge(SCTRawDataProviderCfg(flags))
        result.merge(SCTEventFlagWriterCfg(flags))
        from InDetConfig.TRTPreProcessing import TRTRawDataProviderCfg
        result.merge(TRTRawDataProviderCfg(flags))

    # up to here
    # needed for brem/seeding, TODO decided if needed here
    if flags.Detector.GeometryLAr:
        from LArBadChannelTool.LArBadChannelConfig import LArBadFebCfg
        result.merge(LArBadFebCfg(flags))
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        result.merge(CaloRecoCfg(flags,doLCCalib=True))
        from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
        result.merge(egammaTopoClusterCopierCfg(flags))
        from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
        result.merge(CaloClusterROI_SelectorCfg(flags))


    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    result.merge(InDetRecPreProcessingSiliconCfg(flags))
    from InDetConfig.TrackingSiPatternConfig import TrackingSiPatternCfg
    result.merge(TrackingSiPatternCfg(flags, [], "ResolvedTracks", "SiSPSeededTracks"))


    # TRT extension
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    result.merge(TRTPreProcessingCfg(flags))
    from InDetConfig.TRTExtensionConfig import NewTrackingTRTExtensionCfg
    result.merge(NewTrackingTRTExtensionCfg(flags, SiTrackCollection = "ResolvedTracks", ExtendedTrackCollection = "ExtendedTracks", ExtendedTracksMap = "ExtendedTracksMap", doPhase=False))
    # TODO add followup algs
    result.merge(TrackParticleCnvAlgCfg(flags, TrackContainerName="ExtendedTracks"))

    if flags.InDet.doVertexFinding:
        from InDetConfig.VertexFindingConfig import primaryVertexFindingCfg
        result.merge(primaryVertexFindingCfg(flags))

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TrackParticleContainer#InDetTrackParticles", "xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."]
    toESD = []
    result.merge(addToESD(flags, toAOD+toESD))
    result.merge(addToAOD(flags, toAOD))
    return result

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.Detector.GeometryPixel   = True 
    ConfigFlags.Detector.GeometrySCT   = True
    ConfigFlags.InDet.doPixelClusterSplitting = True
    ConfigFlags.InDet.doSiSPSeededTrackFinder = True

    # TODO add these flags in future
#    ConfigFlags.addFlag('InDet.doAmbiSolving', True)
#    ConfigFlags.addFlag('InDet.useHolesFromPattern', False)
#    ConfigFlags.addFlag('InDet.holeSearchInGX2Fit', True)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
 
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    #######################################################################
    #################### Additional Configuration  ########################
    if "EventInfo" not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        top_acc.merge(EventInfoCnvAlgCfg(ConfigFlags))


    top_acc.merge(TrackRecoCfg(ConfigFlags))
    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("TrackingSiPatternConfig.pkl", "wb"))
    import sys
    
    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        if sc.isFailure():
            sys.exit(-1)
