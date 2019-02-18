# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for MicroMegas and STgc reconstruction"""

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon import CfgMgr

##
## Micromegas Tools
##
def SimpleMMClusterBuilderTool(name="SimpleMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__SimpleMMClusterBuilderTool(name,**kwargs)
##
## STgc Tools
##
def SimpleSTgcClusterBuilderTool(name="SimpleSTgcClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__SimpleSTgcClusterBuilderTool(name,**kwargs)
    
