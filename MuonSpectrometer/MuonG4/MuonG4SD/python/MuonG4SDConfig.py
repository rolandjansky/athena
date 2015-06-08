# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getCSCSensitiveDetectorCosmics(name="CSCSensitiveDetectorCosmics", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", ["CSC_Hits"])
    return CfgMgr.CSCSensitiveDetectorCosmicsTool(name, **kwargs)
def getCSCSensitiveDetector(name="CSCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::CscArCO2"])
    kwargs.setdefault("OutputCollectionNames", ["CSC_Hits"])
    return CfgMgr.CSCSensitiveDetectorTool(name, **kwargs)
def getGenericMuonSensitiveDetector(name="GenericMuonSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["GenericSenitiveVolume"])
    kwargs.setdefault("OutputCollectionNames", ["GenericMuonSensitiveDetector"])
    return CfgMgr.GenericMuonSensitiveDetectorTool(name, **kwargs)
def getMDTSensitiveDetectorCosmics(name="MDTSensitiveDetectorCosmics", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", ["MDT_Hits"])
    return CfgMgr.MDTSensitiveDetectorCosmicsTool(name, **kwargs)
def getMDTSensitiveDetector(name="MDTSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::SensitiveGas"])
    kwargs.setdefault("OutputCollectionNames", ["MDT_Hits"])
    return CfgMgr.MDTSensitiveDetectorTool(name, **kwargs)
def getMicromegasSensitiveDetector(name="MicromegasSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::MM_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["MicromegasSensitiveDetector"])
    return CfgMgr.MicromegasSensitiveDetectorTool(name, **kwargs)
def getRPCSensitiveDetectorCosmics(name="RPCSensitiveDetectorCosmics", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", ["RPC_Hits"])
    return CfgMgr.RPCSensitiveDetectorCosmicsTool(name, **kwargs)
def getRPCSensitiveDetector(name="RPCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::gazGap"])
    kwargs.setdefault("OutputCollectionNames", ["RPC_Hits"])
    return CfgMgr.RPCSensitiveDetectorTool(name, **kwargs)
def getTGCSensitiveDetectorCosmics(name="TGCSensitiveDetectorCosmics", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", ["TGC_Hits"])
    return CfgMgr.TGCSensitiveDetectorCosmicsTool(name, **kwargs)
def getTGCSensitiveDetector(name="TGCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::muo::TGCGas"])
    kwargs.setdefault("OutputCollectionNames", ["TGC_Hits"])
    return CfgMgr.TGCSensitiveDetectorTool(name, **kwargs)
def getsTGCSensitiveDetector(name="sTGCSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Muon::sTGC_Sensitive"])
    kwargs.setdefault("OutputCollectionNames", ["sTGCSensitiveDetector"])
    return CfgMgr.sTGCSensitiveDetectorTool(name, **kwargs)
