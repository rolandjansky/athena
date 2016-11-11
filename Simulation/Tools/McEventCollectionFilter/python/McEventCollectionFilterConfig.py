# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getMcEventCollectionFilter(name="McEventCollectionFilter", **kwargs):
    from AthenaCommon.DetFlags import DetFlags
    kwargs.setdefault("UseTRTHits", DetFlags.detdescr.TRT_on())
    return CfgMgr.McEventCollectionFilter(name, **kwargs)
