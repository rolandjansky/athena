#**************************************************************
#
# MonROBDataProviderSvc job options fragment
#
#==============================================================
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr

# remove old ROBDataProviderSvc definition
if hasattr(svcMgr, 'ROBDataProviderSvc'):
    if theApp.__dict__[ 'CreateSvc' ].count( svcMgr.ROBDataProviderSvc.getFullName() ):
        theApp.__dict__[ 'CreateSvc' ].remove( svcMgr.ROBDataProviderSvc.getFullName() )
    del svcMgr.allConfigurables['ROBDataProviderSvc']
    del svcMgr.ROBDataProviderSvc
    if dir().count('ROBDataProviderSvc'):
        del ROBDataProviderSvc

# configure the offline monitoring ROBDataProviderSvc         
svcMgr += CfgMgr.MonROBDataProviderSvc("ROBDataProviderSvc")
theApp.CreateSvc += [ svcMgr.ROBDataProviderSvc.getFullName() ]
ROBDataProviderSvc = svcMgr.ROBDataProviderSvc

# switch on algorithm auditing in case it is not yet done
theApp.AuditAlgorithms = True
#
#==============================================================
#
# End of MonROBDataProviderSvc job options fragment
#
#**************************************************************
