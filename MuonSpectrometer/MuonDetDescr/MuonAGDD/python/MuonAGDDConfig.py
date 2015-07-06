# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def MuonAGDDTool(name="MuonSpectrometer", **kwargs):
    kwargs.setdefault("BuildNSW", False)
    return CfgMgr.MuonAGDDTool(name, **kwargs)

def NSWAGDDTool(name="NewSmallWheel", **kwargs):
    kwargs.setdefault("Locked", False)
    kwargs.setdefault("XMLFiles", ["NSW_StationDescription.xml"])
    kwargs.setdefault("Volumes", ["NewSmallWheel"])
    kwargs.setdefault("DefaultDetector", "Muon")
    return CfgMgr.NSWAGDDTool(name, **kwargs)

