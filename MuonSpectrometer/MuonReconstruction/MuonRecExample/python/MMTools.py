# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for MicroMegas reconstruction"""

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon import CfgMgr

def SimpleMMClusterBuilderTool(name="SimpleMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__SimpleMMClusterBuilderTool(name,**kwargs)

