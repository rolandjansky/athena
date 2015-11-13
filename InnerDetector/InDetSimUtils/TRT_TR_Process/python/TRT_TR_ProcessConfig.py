# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTRTPhysicsTool(name="TRTPhysicsTool", **kwargs):
    #Add any future configuration here
    return CfgMgr.TRTPhysicsTool(name, **kwargs)
