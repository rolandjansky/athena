# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getMonopolePhysicsTool(name="MonopolePhysicsTool", **kwargs):
    return CfgMgr.MonopolePhysicsToolTool(name, **kwargs)
