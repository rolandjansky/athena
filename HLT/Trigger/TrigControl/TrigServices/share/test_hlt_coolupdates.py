# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Job options to test/configure TrigCOOLUpdateHelper
#

# Set minimal flags for online/data running
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = 'CONDBR2-HLTP-2018-01'
globalflags.DatabaseInstance='CONDBR2'
globalflags.DataSource = 'data'

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

# Enable COOL updates
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.HltEventLoopMgr.CoolUpdateTool.enable()
