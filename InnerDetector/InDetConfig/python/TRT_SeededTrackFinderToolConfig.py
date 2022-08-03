# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_SeededTrackFinderTool package

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def TRT_SeededTrackFinder_ATLCfg(flags, name='InDetTRT_SeededTrackMaker', InputCollections=[], **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags)

    #
    # --- TRT seeded back tracking tool
    #
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    InDetPatternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetPatternPropagator"))
    acc.addPublicTool(InDetPatternPropagator)
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdator_xkCfg
    InDetPatternUpdator = acc.popToolsAndMerge(KalmanUpdator_xkCfg(flags, name="InDetPatternUpdator"))
    acc.addPublicTool(InDetPatternUpdator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)

    from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiCombinatorialTrackFinder_xkCfg
    InDetSiComTrackFinder = acc.popToolsAndMerge(SiCombinatorialTrackFinder_xkCfg(flags))
    kwargs.setdefault("CombinatorialTrackFinder", InDetSiComTrackFinder)

    if flags.InDet.Tracking.ActivePass.usePixel and flags.InDet.Tracking.ActivePass.useSCT:
        from InDetConfig.SiDetElementsRoadToolConfig import SiDetElementsRoadMaker_xk_TRT_Cfg
        InDetTRT_SeededSiRoadMaker = acc.popToolsAndMerge(SiDetElementsRoadMaker_xk_TRT_Cfg(flags))
        acc.addPublicTool(InDetTRT_SeededSiRoadMaker)
        kwargs.setdefault("RoadTool", InDetTRT_SeededSiRoadMaker)

    #
    # --- decide which TRT seed space point finder to use
    #
    from InDetConfig.TRT_SeededSpacePointFinderToolConfig import TRT_SeededSpacePointFinder_ATLCfg
    InDetTRT_SeededSpacePointFinder = acc.popToolsAndMerge(TRT_SeededSpacePointFinder_ATLCfg(flags, InputCollections=InputCollections))
    acc.addPublicTool(InDetTRT_SeededSpacePointFinder)
    kwargs.setdefault("SeedTool", InDetTRT_SeededSpacePointFinder)

    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.ActivePass.SecondarynHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.ActivePass.SecondarynHolesGapMax)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.ActivePass.SecondaryXi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.ActivePass.SecondaryXi2maxNoAdd)
    kwargs.setdefault("SearchInCaloROI", False)
    if kwargs["SearchInCaloROI"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer","InDetCaloClusterROIPhiRZ12GeV")
    else:
        kwargs.setdefault("EMROIPhiRZContainer","")
    kwargs.setdefault("ConsistentSeeds", True)
    kwargs.setdefault("BremCorrection", False)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("nWClustersMin", 0)

    InDetTRT_SeededTrackTool = CompFactory.InDet.TRT_SeededTrackFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededTrackTool)
    return acc
