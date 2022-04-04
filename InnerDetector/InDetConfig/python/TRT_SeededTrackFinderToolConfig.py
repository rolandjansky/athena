# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_SeededTrackFinderTool package

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def TRT_SeededTrackFinder_ATLCfg(flags, name='InDetTRT_SeededTrackMaker', InputCollections=[], **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    #
    # --- TRT seeded back tracking tool
    #
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    InDetPatternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetPatternPropagator"))
    from InDetConfig.TrackingCommonConfig import InDetPatternUpdatorCfg
    InDetPatternUpdator = acc.getPrimaryAndMerge(InDetPatternUpdatorCfg())
    from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiCombinatorialTrackFinder_xkCfg
    InDetSiComTrackFinder = acc.popToolsAndMerge(SiCombinatorialTrackFinder_xkCfg(flags))

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

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("SeedTool", InDetTRT_SeededSpacePointFinder)
    kwargs.setdefault("CombinatorialTrackFinder", InDetSiComTrackFinder)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.ActivePass.SecondarynHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.ActivePass.SecondarynHolesGapMax)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.ActivePass.SecondaryXi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.ActivePass.SecondaryXi2maxNoAdd)
    kwargs.setdefault("SearchInCaloROI", False)
    if kwargs["SearchInCaloROI"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROI_SelectorCfg
        acc.merge(CaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("InputClusterContainerName", "InDetCaloClusterROIs") # InDetKeys.CaloClusterROIContainer()
    else:
        kwargs.setdefault("InputClusterContainerName", "")
    kwargs.setdefault("ConsistentSeeds", True)
    kwargs.setdefault("BremCorrection", False)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("nWClustersMin", 0)

    InDetTRT_SeededTrackTool = CompFactory.InDet.TRT_SeededTrackFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededTrackTool)
    return acc
