# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Define fuctions to configure Space Point Finder

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# from IOVDbSvc.IOVDbSvcConfig import addFolders,addFoldersSplitOnline

def InDetSiElementPropertiesTableCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg", **kwargs))
    return acc

def InDetSiTrackerSpacePointFinderCfg(flags, **kwargs):
    if flags.InDetFlags.doSLHC:
        kwargs.setdefault("SCTGapParameter", 0.0015)
    
    if flags.InDetFlags.doCosmics or flags.InDetFlags.doBeamHalo:
        kwargs.setdefault("StripLengthTolerance", 0.05)
        kwargs.setdefault("UsePerpendicularProjection", True)

    InDetSiSpacePointMakerTool = CompFactory.InDet.SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool", **kwargs)
        
    acc = ComponentAccumulator()
    acc.addPublicTool(InDetSiSpacePointMakerTool) ## I decided to merge it
    kwargs.update(
        SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
        PixelsClustersName     = flags.InDetKeys.PixelClusters,
        SCT_ClustersName       = flags.InDetKeys.SCT_Clusters,
        SpacePointsPixelName   = flags.InDetKeys.PixelSpacePoints,
        SpacePointsSCTName     = flags.InDetKeys.SCT_SpacePoints,
        SpacePointsOverlapName = flags.InDetKeys.OverlapSpacePoints,
        ProcessPixels          = flags.DetFlags.haveRIO.pixel_on,
        ProcessSCTs            = flags.DetFlags.haveRIO.SCT_on,
        ProcessOverlaps        = flags.DetFlags.haveRIO.SCT_on)

    if flags.InDetFlags.doDBMstandalone:
        kwargs.update(
            OverlapLimitEtaMax = 5.0,
            OverlapLimitEtaMin = 0)

    if flags.InDetFlags.doCosmics:
        kwargs.update(
            ProcessOverlaps      = False,
            OverrideBeamSpot     = True,
            VertexZ              = 0,
            VertexX              = 0,
            VertexY              = 99999999,
            OverlapLimitOpposite = 5)  
    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name = "InDetSiTrackerSpacePointFinder", **kwargs))
    return acc

''' ### Py:ConfigurableDb WARNING : Class InDet.PRD_MultiTruthMakerSi not in database 

def InDetPRD_MultiTruthMakerSiCfg(flags, **kwargs):
    InDet.PRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMakerSi
    InDetPRD_MultiTruthMakerSi = InDet.PRD_MultiTruthMakerSi(name = "InDetPRD_MultiTruthMakerSi")
    kwargs.update(
        PixelClusterContainerName   = flags.InDetKeys.PixelClusters,
        SCTClusterContainerName     = flags.InDetKeys.SCT_Clusters,
        TRTDriftCircleContainerName = "",
        SimDataMapNamePixel         = flags.InDetKeys.PixelSDOs,
        SimDataMapNameSCT           = flags.InDetKeys.SCT_SDOs,
        SimDataMapNameTRT           = "",
        TruthNamePixel              = flags.InDetKeys.PixelClustersTruth,
        TruthNameSCT                = flags.InDetKeys.SCT_ClustersTruth,
        TruthNameTRT                = "")
    
    if (not flags.DetFlags.haveRIO.pixel_on or not flags.InDetFlags.doPixelPRDFormation):
        kwargs.update(
            PixelClusterContainerName = "",
            SimDataMapNamePixel       = "",
            TruthNamePixel            = "")
    if (not flags.DetFlags.haveRIO.SCT_on or not InDetFlags.doSCT_PRDFormation):
        kwargs.update(
            SCTClusterContainerName   = "",
            SimDataMapNameSCT         = "",
            TruthNameSCT              = "")
    
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.InDet.PRD_MultiTruthMakerSi(name = "InDetPRD_MultiTruthMakerSi", **kwargs))
    return acc

def InDetPRD_MultiTruthMakerSiPUCfg(flags, **kwargs):
    kwargs.update(
        PixelClusterContainerName   = flags.InDetKeys.PixelPUClusters,
        SCTClusterContainerName     = flags.InDetKeys.SCT_PU_Clusters,
        TRTDriftCircleContainerName = "",
        SimDataMapNamePixel         = flags.InDetKeys.PixelPUSDOs,
        SimDataMapNameSCT           = flags.InDetKeys.SCT_PU_SDOs,
        SimDataMapNameTRT           = "",
        TruthNamePixel              = flags.InDetKeys.PixelPUClustersTruth,
        TruthNameSCT                = flags.InDetKeys.SCT_PU_ClustersTruth,
        TruthNameTRT                = "",
        OutputLevel                 = VERBOSE)
    
    if (not flags.DetFlags.haveRIO.pixel_on or not flags.InDetFlags.doPixelPRDFormation):
        kwargs.update(
            PixelClusterContainerName = "",
            SimDataMapNamePixel       = "",
            TruthNamePixel            = "")
    if (not flags.DetFlags.haveRIO.SCT_on or not InDetFlags.doSCT_PRDFormation):
        kwargs.update(
            SCTClusterContainerName   = "",
            SimDataMapNameSCT         = "",
            TruthNameSCT              = "")
    
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.InDet.PRD_MultiTruthMakerSiPU(name = "InDetPRD_MultiTruthMakerSi", **kwargs))
    return acc '''

if __name__ == "__main__":
    import IDTestFlags
    flags = IDTestFlags.IDTestFlags_q221()
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)
    if flags.InDetFlags.doSpacePointFormation:
        acc.merge(InDetSiElementPropertiesTableCondAlgCfg(flags))
        acc.merge(InDetSiTrackerSpacePointFinderCfg(flags))
    '''
    if flags.InDetFlags.doPRDFormation and flags.InDetFlags.doSpacePointFormation:
        if flags.InDetFlags.doTruth:
            acc.merge(InDetPRD_MultiTruthMakerSiCfg(flags))
            acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(flags))
    '''

    acc.printConfig()
    #print(acc.getPublicTool("InDetSiSpacePointMakerTool"))
    


