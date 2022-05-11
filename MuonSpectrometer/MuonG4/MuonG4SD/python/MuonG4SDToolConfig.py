# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg


def CSCSensitiveDetectorCosmicsCfg(ConfigFlags, name="CSCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.CSCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result
def CSCSensitiveDetectorCfg(ConfigFlags, name="CSCSensitiveDetector", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.CSCSensitiveDetectorTool(name, **kwargs) )
    return result


def GenericMuonSensitiveDetectorCfg(ConfigFlags, name="GenericMuonSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["GenericSenitiveVolume"])
    kwargs.setdefault("OutputCollectionNames", ["GenericMuonSensitiveDetector"])
    result=ComponentAccumulator()
    result.setPrivateTools( CompFactory.GenericMuonSensitiveDetectorTool(name, **kwargs) )
    return result


def MDTSensitiveDetectorCosmicsCfg(ConfigFlags, name="MDTSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.MDTSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def MDTSensitiveDetectorCfg(ConfigFlags, name="MDTSensitiveDetector", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.MDTSensitiveDetectorTool(name, **kwargs) )
    return result


def MicromegasSensitiveDetectorCfg(ConfigFlags, name="MicromegasSensitiveDetector", **kwargs):
    bare_collection_name = "MM_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MMHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::MM_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.MicromegasSensitiveDetectorTool(name, **kwargs) )
    return result


def RPCSensitiveDetectorCosmicsCfg(ConfigFlags, name="RPCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name]) #is this correct?

    result.setPrivateTools( CompFactory.RPCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def RPCSensitiveDetectorCfg(ConfigFlags, name="RPCSensitiveDetector", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.RPCSensitiveDetectorTool(name, **kwargs) )
    return result


def TGCSensitiveDetectorCosmicsCfg(ConfigFlags, name="TGCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.TGCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def TGCSensitiveDetectorCfg(ConfigFlags, name="TGCSensitiveDetector", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.TGCSensitiveDetectorTool(name, **kwargs) )
    return result


def sTGCSensitiveDetectorCfg(ConfigFlags, name="sTGCSensitiveDetector", **kwargs):
    bare_collection_name = "sTGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "sTGCHits"
    region = "MUON"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                       bare_collection_name,
                                                       mergeable_collection_suffix,
                                                       merger_input_property,
                                                       region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::sTGC_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CompFactory.sTGCSensitiveDetectorTool(name, **kwargs) )
    return result
