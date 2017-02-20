# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getDarkSectorPhysicsTool(name="DarkSectorPhysicsTool", **kwargs):
     return CfgMgr.DarkSectorPhysicsToolTool(name, **kwargs)
