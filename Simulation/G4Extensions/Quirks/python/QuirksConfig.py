# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getQuirkPhysicsTool(name="QuirkPhysicsTool", **kwargs):
    return CfgMgr.QuirkPhysicsToolTool(name, **kwargs)
