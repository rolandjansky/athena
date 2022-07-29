# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_SeededSpacePointFinderTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def SimpleTRT_SeededSpacePointFinder_ATLCfg(flags, name='InDetTRT_SeededSpFinder', InputCollections=[], **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags)

    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = (len(InputCollections) > 0)

    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints')
    kwargs.setdefault("PRDtoTrackMap", 'InDetSegmentPRDtoTrackMap' if usePrdAssociationTool else "")
    kwargs.setdefault("PerigeeCut", 1000.)
    kwargs.setdefault("DirectionPhiCut", .3)
    kwargs.setdefault("DirectionEtaCut", 1.)
    kwargs.setdefault("MaxHoles", 2)
    kwargs.setdefault("RestrictROI", True)

    InDetTRT_SeededSpacePointFinder = CompFactory.InDet.SimpleTRT_SeededSpacePointFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSpacePointFinder)
    return acc

def TRT_SeededSpacePointFinder_ATLCfg(flags, name='InDetTRT_SeededSpFinder', InputCollections=[], **kwargs):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = (len(InputCollections) > 0)

    #
    # --- defaul space point finder
    #
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints')
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints')
    kwargs.setdefault("PRDtoTrackMap", 'InDetSegmentPRDtoTrackMap' if usePrdAssociationTool else "")
    kwargs.setdefault("NeighborSearch", True)
    kwargs.setdefault("LoadFull", False)
    kwargs.setdefault("DoCosmics", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minSecondaryPt)

    InDetTRT_SeededSpacePointFinder = CompFactory.InDet.TRT_SeededSpacePointFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSpacePointFinder)
    return acc
