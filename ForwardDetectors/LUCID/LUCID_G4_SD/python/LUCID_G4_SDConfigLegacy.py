# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getLUCID_SensitiveDetector(name="LUCID_SensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LUCID::lvPmt"])
    kwargs.setdefault("OutputCollectionNames", ["LucidSimHitsVector"])
    return CfgMgr.LUCID_SensitiveDetectorTool(name, **kwargs)
