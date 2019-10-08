# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
#
# Import NSWCalibTools job properties
#
from AthenaCommon import CfgMgr

def NSWCalibSmearingTool(name="",**kwargs):
    return CfgMgr.NSWCalibSmearingTool(name,**kwargs)
