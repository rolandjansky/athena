# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# The earliest bunch crossing time for which interactions will be sent
# to the HGTD Smeared Digitization code.
def HGTD_FirstXing():
    FirstXing = -50
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = -25
    return FirstXing

# The latest bunch crossing time for which interactions will be sent
# to the HGTD_ Digitization code.
def HGTD_LastXing():
    LastXing = 100
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 75
        else :
            LastXing = 25
    return LastXing


def HGTD_SmearedDigitizationTool(name="HGTD_SmearedDigitizationTool", **kwargs):
    kwargs.setdefault("SiHitCollectionName", "HGTD_Hits")
    from AthenaCommon import CfgMgr
    return CfgMgr.HGTD_SmearedDigitizationTool(name, **kwargs)

def getSmearedHGTD_Range(name="SmearedHGTDRange" , **kwargs):
    kwargs.setdefault('FirstXing', HGTD_FirstXing() )
    kwargs.setdefault('LastXing',  HGTD_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#HGTD_Hits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)
