# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getActsFatrasSimTool(name="ISF_ActsFatrasSimTool", **kwargs):
    return CfgMgr.ISF__ActsFatrasSimTool(name, **kwargs)
