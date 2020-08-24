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

def UTPCMMClusterBuilderTool(name="UTPCMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__UTPCMMClusterBuilderTool(name,**kwargs)

def ProjectionMMClusterBuilderTool(name="ProjectionMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__ProjectionMMClusterBuilderTool(name,**kwargs)

def ConstraintAngleMMClusterBuilderTool(name="ConstraintAngleMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__ConstraintAngleMMClusterBuilderTool(name,**kwargs)

def ClusterTimeProjectionMMClusterBuilderTool(name="ClusterTimeProjectionMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__ClusterTimeProjectionMMClusterBuilderTool(name,**kwargs)
##
## STgc Tools
##
def SimpleSTgcClusterBuilderTool(name="SimpleSTgcClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__SimpleSTgcClusterBuilderTool(name,**kwargs)
    
