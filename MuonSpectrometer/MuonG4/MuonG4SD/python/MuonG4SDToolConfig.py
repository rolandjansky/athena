# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from MuonG4SD.MuonG4SDConf import CSCSensitiveDetectorCosmicsTool,CSCSensitiveDetectorTool, GenericMuonSensitiveDetectorTool,MDTSensitiveDetectorCosmicsTool, MDTSensitiveDetectorTool, MicromegasSensitiveDetectorTool, RPCSensitiveDetectorCosmicsTool, RPCSensitiveDetectorTool, TGCSensitiveDetectorCosmicsTool, TGCSensitiveDetectorTool, sTGCSensitiveDetectorTool

def CSCSensitiveDetectorCosmicsCfg(ConfigFlags, name="CSCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CSCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result
def CSCSensitiveDetectorCfg(ConfigFlags, name="CSCSensitiveDetector", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( CSCSensitiveDetectorTool(name, **kwargs) )
    return result


def GenericMuonSensitiveDetectorCfg(ConfigFlags, name="GenericMuonSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["GenericSenitiveVolume"])
    kwargs.setdefault("OutputCollectionNames", ["GenericMuonSensitiveDetector"])

    result.setPrivateTools( GenericMuonSensitiveDetectorTool(name, **kwargs) )
    return result


def MDTSensitiveDetectorCosmicsCfg(ConfigFlags, name="MDTSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( MDTSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def MDTSensitiveDetectorCfg(ConfigFlags, name="MDTSensitiveDetector", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( MDTSensitiveDetectorTool(name, **kwargs) )
    return result


def MicromegasSensitiveDetectorCfg(ConfigFlags, name="MicromegasSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::MM_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["MicromegasSensitiveDetector"]) # need ISF_Collection merger in acc?

    result.setPrivateTools( MicromegasSensitiveDetectorTool(name, **kwargs) )
    return result


def RPCSensitiveDetectorCosmicsCfg(ConfigFlags, name="RPCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name]) #is this correct?

    result.setPrivateTools( RPCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def RPCSensitiveDetectorCfg(ConfigFlags, name="RPCSensitiveDetector", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( RPCSensitiveDetectorTool(name, **kwargs) )
    return result


def TGCSensitiveDetectorCosmicsCfg(ConfigFlags, name="TGCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( TGCSensitiveDetectorCosmicsTool(name, **kwargs) )
    return result


def TGCSensitiveDetectorCfg(ConfigFlags, name="TGCSensitiveDetector", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( TGCSensitiveDetectorTool(name, **kwargs) )
    return result


def sTGCSensitiveDetectorCfg(ConfigFlags, name="sTGCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::sTGC_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["sTGCSensitiveDetector"])

    result.setPrivateTools( sTGCSensitiveDetectorTool(name, **kwargs) )
    return result
