# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def ITk_BCM_ZeroSuppressionCfg(flags, name="ITk_BCM_ZeroSuppression", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name=name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def ITkPixelClusterizationCfg(flags, name = "ITkPixelClusterization", **kwargs) :
    acc = ComponentAccumulator()
    merged_pixels_tool = acc.getPrimaryAndMerge(ITkMergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.getPrimaryAndMerge(ITkPixelGangedAmbiguitiesFinderCfg(flags, **kwargs))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "ITkPixelRDOs")
    kwargs.setdefault("ClustersName", "ITkPixelClusters")
    kwargs.setdefault("AmbiguitiesMap", "ITkPixelClusterAmbiguitiesMap")

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name=name, **kwargs))
    return acc
##------------------------------------------------------------------------------
def ITkPixelClusterizationPUCfg(flags, name="ITkPixelClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "ITkPixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "ITkPixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "ITkPixelClusterAmbiguitiesMapPU")
    return ITkPixelClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def ITkStripClusterizationCfg(flags, name="ITkStripClusterization", **kwargs) :
    acc = ComponentAccumulator()

    # Need to get ITkStrip_ConditionsSummaryTool for e.g. ITkStripClusteringTool
    from InDetConfig.ITkRecToolConfig import ITkStripConditionsSummaryToolCfg
    ITkStripConditionsSummaryTool = acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags))

    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge( ITkStripLorentzAngleCfg(flags) )

    #### Clustering tool ######
    ITkClusterMakerTool = acc.getPrimaryAndMerge(ITkClusterMakerToolCfg(flags))
    ITkStripClusteringTool = CompFactory.InDet.SCT_ClusteringTool( name           = "ITkStripClusteringTool",
                                                                   globalPosAlg   = ITkClusterMakerTool,
                                                                   conditionsTool = ITkStripConditionsSummaryTool,
                                                                   LorentzAngleTool = ITkStripLorentzAngleTool,
                                                                   useRowInformation = True,
                                                                   SCTDetEleCollKey = "ITkStripDetectorElementCollection")

    kwargs.setdefault("clusteringTool", ITkStripClusteringTool)
    kwargs.setdefault("DataObjectName", 'ITkStripRDOs')
    kwargs.setdefault("ClustersName", 'ITkStripClusters')
    kwargs.setdefault("SCT_FlaggedCondData", "ITkStripFlaggedCondData")
    kwargs.setdefault("conditionsTool", ITkStripConditionsSummaryTool)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name=name, **kwargs))

    return acc

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def ITkStripClusterizationPUCfg(flags, name="ITkStripClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "ITkStrip_PU_RDOs" )
    kwargs.setdefault("ClustersName", "ITkStrip_PU_Clusters")
    return ITkStripClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
def ITkPixelGangedAmbiguitiesFinderCfg(flags, **kwargs) :
    acc = ComponentAccumulator()

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
    acc.merge( ITkPixelGeometryCfg(flags))

    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")

    ITkPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "ITkPixelGangedAmbiguitiesFinder", **kwargs)
    acc.addPublicTool( ITkPixelGangedAmbiguitiesFinder, primary=True )
    return acc

##------------------------------------------------------------------------------
def ITkMergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      kwargs.setdefault("globalPosAlg", acc.getPrimaryAndMerge(ITkClusterMakerToolCfg(flags)))

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
      ITkPixelConditionsSummary = acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags))
      kwargs.setdefault("PixelConditionsSummaryTool", ITkPixelConditionsSummary)

      kwargs.setdefault("PixelDetEleCollKey","ITkPixelDetectorElementCollection")

      ITkMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "ITkMergedPixelsTool", **kwargs)
      acc.addPublicTool(ITkMergedPixelsTool, primary=True)
      return acc

##------------------------------------------------------------------------------
def ITkClusterMakerToolCfg(flags, name="ITkClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (ITkPixelChargeCalibCondAlgCfg, ITkPixelConfigCondAlgCfg, ITkPixelDeadMapCondAlgCfg,
                                                                 ITkPixelOfflineCalibCondAlgCfg)
    from PixelCabling.PixelCablingConfigNew import ITkPixelCablingSvcCfg
    #ITkPixelCablingCondAlgCfg + ITkPixelReadoutSpeedAlgCfg needed?

    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    acc.merge(ITkPixelDeadMapCondAlgCfg(flags))
    acc.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    acc.merge(ITkPixelOfflineCalibCondAlgCfg(flags))
    acc.merge(ITkPixelCablingSvcCfg(flags))
    #acc.merge(PixelCablingCondAlgCfg(flags))
    #acc.merge(PixelReadoutSpeedAlgCfg(flags))

    from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
    ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge(ITkStripLorentzAngleCfg(flags))

    kwargs.setdefault("PixelChargeCalibCondData", "ITkPixelChargeCalibCondData")
    kwargs.setdefault("PixelCablingSvc",acc.getService("ITkPixelCablingSvc"))
    kwargs.setdefault("PixelLorentzAngleTool", ITkPixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", ITkStripLorentzAngleTool)
    kwargs.setdefault("PixelOfflineCalibData", "")

    ITkClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = name, **kwargs)
    acc.addPublicTool(ITkClusterMakerTool, primary=True)
    return acc


def ITkTrackToVertexCfg(flags, name="ITkTrackToVertex", **kwargs):
    result = ComponentAccumulator()
    if "Extrapolator" not in kwargs:
        #Run 3 uses
        #from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg 
        from InDetConfig.ITkRecToolConfig import ITkExtrapolatorCfg
        kwargs["Extrapolator"] = result.getPrimaryAndMerge(ITkExtrapolatorCfg(flags))
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg

    result.merge(BeamSpotCondAlgCfg(flags))
    result.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return result

def ITkTrackParticleCreatorToolCfg(flags, name="ITkTrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        kwargs["TrackToVertex"] = result.popToolsAndMerge(ITkTrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolSharedHitsCfg
        kwargs["TrackSummaryTool"] = result.popToolsAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags))
    p_expr = flags.ITk.perigeeExpression
    kwargs.setdefault("BadClusterID", flags.ITk.pixelClusterBadClusterID)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", flags.ITk.KeepFirstParameters)
    kwargs.setdefault("PerigeeExpression", p_expr if p_expr != "Vertex" else "BeamLine")
    kwargs.setdefault("DoITk", True)
    result.addPublicTool(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs), primary=True)
    return result

def ITkTrackCollectionCnvToolCfg(flags, name="ITkTrackCollectionCnvTool", ITkTrackParticleCreator = None, **kwargs):
    result = ComponentAccumulator()

    ITkTrackParticleCreator = result.getPrimaryAndMerge(ITkTrackParticleCreatorToolCfg(flags))

    kwargs.setdefault("TrackParticleCreator", ITkTrackParticleCreator)
    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(name, **kwargs))
    return result

def ITkTrackParticleCnvAlgCfg(flags, name="ITkTrackParticleCnvAlg", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("xAODContainerName", OutputTrackParticleContainer)
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", OutputTrackParticleContainer)
    if "TrackParticleCreator" not in kwargs:
        kwargs["TrackParticleCreator"] = result.getPrimaryAndMerge(ITkTrackParticleCreatorToolCfg(flags))
    if "TrackCollectionCnvTool" not in kwargs:
        kwargs["TrackCollectionCnvTool"] = result.popToolsAndMerge(ITkTrackCollectionCnvToolCfg(
            flags,
            TrackParticleCreator=kwargs["TrackParticleCreator"],
        ))

    if flags.ITk.doTruth:
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

def ITkTrackRecoCfg(flags):
    """Configures complete ID tracking """
    result = ComponentAccumulator()

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
    result.merge( ITkPixelGeometryCfg(flags))

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
    result.merge( ITkStripGeometryCfg(flags))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(flags))

    #TODO move these to a more appropriate place

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (ITkPixelChargeCalibCondAlgCfg, ITkPixelOfflineCalibCondAlgCfg, ITkPixelDistortionAlgCfg)
    result.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    result.merge(ITkPixelOfflineCalibCondAlgCfg(flags))
    result.merge(ITkPixelDistortionAlgCfg(flags))

    from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleTool, ITkPixelLorentzAngleCfg
    result.addPublicTool(ITkPixelLorentzAngleTool(flags))
    result.addPublicTool(result.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags)))

    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
    result.addPublicTool(result.popToolsAndMerge(ITkStripLorentzAngleCfg(flags)))

    #Needed for ITk?
    #from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    #result.merge(PixelHitDiscCnfgAlgCfg(flags))
    if flags.Input.Format == "BS":
        from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
        result.merge(PixelRawDataProviderAlgCfg(flags))
        from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTRawDataProviderCfg, SCTEventFlagWriterCfg
        result.merge(SCTRawDataProviderCfg(flags))
        result.merge(SCTEventFlagWriterCfg(flags))

    # up to here
    # needed for brem/seeding, TODO decided if needed here
    # commented for now
    '''
    from LArBadChannelTool.LArBadChannelConfig import LArBadFebCfg
    result.merge(LArBadFebCfg(flags))
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    result.merge(CaloRecoCfg(flags,doLCCalib=True))
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result.merge(egammaTopoClusterCopierCfg(flags))
    from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
    result.merge(CaloClusterROI_SelectorCfg(flags))
    '''

    from InDetConfig.ITkSiliconPreProcessing import ITkRecPreProcessingSiliconCfg
    result.merge(ITkRecPreProcessingSiliconCfg(flags))
    from InDetConfig.ITkTrackingSiPatternConfig import ITkTrackingSiPatternCfg
    result.merge(ITkTrackingSiPatternCfg(flags, [], "ResolvedTracks", "SiSPSeededTracks"))
    result.merge(ITkTrackParticleCnvAlgCfg(flags, TrackContainerName="ResolvedTracks"))

    if flags.ITk.doVertexFinding:
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
    ConfigFlags.ITk.doPixelClusterSplitting = True
    ConfigFlags.ITk.doSiSPSeededTrackFinder = True

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


    top_acc.merge(ITkTrackRecoCfg(ConfigFlags))
    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("ITkTrackingSiPatternConfig.pkl", "wb"))
    import sys

    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        if sc.isFailure():
            sys.exit(-1)
