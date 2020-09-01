# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg
from AthenaConfiguration.ComponentFactory import CompFactory
MinBiasScintillatorSDTool=CompFactory.MinBiasScintillatorSDTool


def MinBiasScintillatorSDCfg(ConfigFlags, name="MinBiasScintillatorSD", **kwargs):
    bare_collection_name = "MBTSHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MBTSHits"
    region = "CALO"

    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::MBTS1", "LArMgr::MBTS2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.ReadTR:
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)

    result.setPrivateTools( MinBiasScintillatorSDTool(name, **kwargs) )

    return result
