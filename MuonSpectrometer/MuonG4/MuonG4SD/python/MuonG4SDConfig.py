# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getCSCSensitiveDetectorCosmics(name="CSCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.CSCSensitiveDetectorCosmicsTool(name, **kwargs)


def getCSCSensitiveDetector(name="CSCSensitiveDetector", **kwargs):
    bare_collection_name = "CSC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "CSCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.CSCSensitiveDetectorTool(name, **kwargs)


def getGenericMuonSensitiveDetector(name="GenericMuonSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["GenericSenitiveVolume"])
    kwargs.setdefault("OutputCollectionNames", ["GenericMuonSensitiveDetector"])
    return CfgMgr.GenericMuonSensitiveDetectorTool(name, **kwargs)


def getMDTSensitiveDetectorCosmics(name="MDTSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.MDTSensitiveDetectorCosmicsTool(name, **kwargs)


def getMDTSensitiveDetector(name="MDTSensitiveDetector", **kwargs):
    bare_collection_name = "MDT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "MDTHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.MDTSensitiveDetectorTool(name, **kwargs)


def getMicromegasSensitiveDetector(name="MicromegasSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::MM_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["MicromegasSensitiveDetector"])
    return CfgMgr.MicromegasSensitiveDetectorTool(name, **kwargs)


def getRPCSensitiveDetectorCosmics(name="RPCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name])
    return CfgMgr.RPCSensitiveDetectorCosmicsTool(name, **kwargs)


def getRPCSensitiveDetector(name="RPCSensitiveDetector", **kwargs):
    bare_collection_name = "RPC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "RPCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.RPCSensitiveDetectorTool(name, **kwargs)


def getTGCSensitiveDetectorCosmics(name="TGCSensitiveDetectorCosmics", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.TGCSensitiveDetectorCosmicsTool(name, **kwargs)


def getTGCSensitiveDetector(name="TGCSensitiveDetector", **kwargs):
    bare_collection_name = "TGC_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TGCHits"
    region = "MUON"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.TGCSensitiveDetectorTool(name, **kwargs)


def getsTGCSensitiveDetector(name="sTGCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::sTGC_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["sTGCSensitiveDetector"])
    return CfgMgr.sTGCSensitiveDetectorTool(name, **kwargs)
