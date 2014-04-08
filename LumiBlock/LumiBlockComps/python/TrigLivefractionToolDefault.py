# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *

# Configuration for LivefractionTool
# Also needs LuminosityTool to be configured (should be in RecExCommon) 
def TrigLivefractionToolDefault(name='TrigLivefractionToolDefault'):
    mlog = logging.getLogger(name)
    # mlog.warning("TrigLivefractionToolDefault called")

    # Set up DB configuration
    from IOVDbSvc.CondDB import conddb
    
    # Mistakenly created as multi-version folder, must specify HEAD
    conddb.addFolderWithTag('TRIGGER', '/TRIGGER/LUMI/PerBcidDeadtime', 'HEAD')

    # Add the luminosity tool as a public tool
    from LumiBlockComps.LumiBlockCompsConf import TrigLivefractionTool
    liveTool = TrigLivefractionTool("TrigLivefractionTool")

    return liveTool
