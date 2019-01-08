# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for MicroMegas reconstruction"""

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

import copy

from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

from AthenaCommon.ConfiguredFactory import getProperty

from MuonRecUtils import logMuon,ConfiguredBase,ExtraFlags

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

def SimpleMMClusterBuilderTool(name="SimpleMMClusterBuilderTool",extraFlags=None,**kwargs):
    return CfgMgr.SimpleMMClusterBuilderTool(name,**kwargs)

