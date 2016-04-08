# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getLucidPhysicsTool(name="LucidPhysicsTool", **kwargs):
    #Add any future configuration here
    return CfgMgr.LucidPhysicsTool(name, **kwargs)
