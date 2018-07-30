# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# As of the summer of 2018, there is no way to turn off the NSW for RUN3 configuration
# By default for RUN3, readAGDD is true so the NSW information is pulled directly from the database
# If the user sets readAGDD to false in the config files, an XML file must be defined also.
# Otherwise, with readAGDD overwritten to false, the job will crash 

from AthenaCommon import CfgMgr
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags

def MuonAGDDTool(name="MuonSpectrometer", **kwargs):
    kwargs.setdefault("BuildNSW", False)
    return CfgMgr.MuonAGDDTool(name, **kwargs)

def NSWAGDDTool(name="NewSmallWheel", **kwargs):
    kwargs.setdefault("Locked", False)
    if not CommonGeometryFlags.Run() == "RUN3" :
        kwargs.setdefault("XMLFiles", ["NSW_StationDescription.xml"])
    kwargs.setdefault("Volumes", ["NewSmallWheel"])
    kwargs.setdefault("DefaultDetector", "Muon")
    return CfgMgr.NSWAGDDTool(name, **kwargs)

