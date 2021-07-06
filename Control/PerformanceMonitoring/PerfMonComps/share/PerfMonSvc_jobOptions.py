# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonComps/PerfMonSvc_jobOptions.py
# @author: Sebastien Binet
# $Id: PerfMonSvc_jobOptions.py,v 1.3 2007-08-01 20:58:52 binet Exp $

###############################
# Load PerfMonSvc
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'PerfMonSvc'):
    from PerfMonComps.JobOptCfg import PerfMonSvc
    svcMgr += PerfMonSvc( "PerfMonSvc" )
    pass
            
# uncomment the following to activate PerfMonSvc:
## from PerfMonComps.PerfMonFlags import jobproperties
## jobproperties.PerfMonFlags.doMonitoring = True

