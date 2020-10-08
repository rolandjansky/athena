# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def BCM_ZeroSuppressionCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression("InDetBCM_ZeroSuppression", **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def PixelClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    sub_acc = MergedPixelsToolCfg(flags, **kwargs)
    merged_pixels_tool = sub_acc.getPrimary()
    acc.merge(sub_acc)
    sub_acc = PixelGangedAmbiguitiesFinderCfg(flags)
    ambi_finder=sub_acc.getPrimary()
    acc.merge(sub_acc)

    acc.addEventAlgo( CompFactory.InDet.PixelClusterization(   name         = "InDetPixelClusterization",
                                                    clusteringTool          = merged_pixels_tool,
                                                    gangedAmbiguitiesFinder = ambi_finder,
                                                    DataObjectName          = "PixelRDOs",
                                                    ClustersName            = "PixelClusters" ))
    return acc
##------------------------------------------------------------------------------
def PixelClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetPixelClusterizationPU")
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False))

    #### Clustering tool ######
    accbuf = ClusterMakerToolCfg(flags)
    InDetClusterMakerTool = accbuf.getPrimary()
    acc.merge(accbuf)
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(    name              = "InDetSCT_ClusteringTool",
                                                            globalPosAlg     = InDetClusterMakerTool,
                                                            conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(    name                    = "InDetSCT_Clusterization",
                                                    clusteringTool          = InDetSCT_ClusteringTool,
                                                    DataObjectName          = 'SCT_RDOs', ##InDetKeys.SCT_RDOs(),
                                                    ClustersName            = 'SCT_Clusters', ##InDetKeys.SCT_Clusters(),
                                                    conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged  ) )

    return acc


##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetSCT_ClusterizationPU")
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters") #flags.InDetKeys.SCT_PU_Clusters
    return SCTClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
def PixelGangedAmbiguitiesFinderCfg(flags) :
    acc = ComponentAccumulator()
    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder, primary=True)
    return acc


##------------------------------------------------------------------------------
def MergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      accbuf = ClusterMakerToolCfg(flags)
      InDetClusterMakerTool = accbuf.getPrimary()
      kwargs.setdefault("globalPosAlg", InDetClusterMakerTool )
      acc.merge(accbuf)

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from InDetConfig.InDetRecToolConfig import PixelConditionsSummaryToolCfg
      accbuf = PixelConditionsSummaryToolCfg(flags)
      conditionssummarytool = accbuf.popPrivateTools()
      kwargs.setdefault("PixelConditionsSummaryTool", conditionssummarytool ) 
      acc.merge(accbuf)

      # Enable duplcated RDO check for data15 because duplication mechanism was used.
      if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
          kwargs.setdefault("CheckDuplicatedRDO", True )

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "InDetMergedPixelsTool", **kwargs)
     
      acc.addPublicTool(InDetMergedPixelsTool, primary=True)
      return acc
##------------------------------------------------------------------------------
def ClusterMakerToolCfg(flags, **kwargs) :
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg

    acc = ComponentAccumulator()
    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge( PixelChargeCalibCondAlgCfg(flags))
    acc.merge( PixelConfigCondAlgCfg(flags))

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = "InDetClusterMakerTool", **kwargs)

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )    
    acc.addPublicTool(SCTLorentzAngleTool)
    acc.addPublicTool(InDetClusterMakerTool, primary=True)
    return acc

