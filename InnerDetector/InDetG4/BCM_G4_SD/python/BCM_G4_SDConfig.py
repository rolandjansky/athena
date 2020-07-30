# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import simFlags

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getBCMSensorSD(name="BCMSensorSD", **kwargs):
    bare_collection_name = "BCMHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "BCMHits"
    region = "ID"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::bcmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.BCMSensorSDTool(name, **kwargs)
