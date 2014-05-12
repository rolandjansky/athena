#**************************************************************
#
# TrigROBDataProviderSvc_RTT job options fragment
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
svcMgr += CfgMgr.TrigROBDataProviderSvc_RTT("ROBDataProviderSvc")
theApp.CreateSvc += [ svcMgr.ROBDataProviderSvc.getFullName() ]
ROBDataProviderSvc = svcMgr.ROBDataProviderSvc

# switch on algorithm auditing in case it is not yet done
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
theApp.AuditTools      = True
#
# TrigROBDataProviderSvc_RTT specific options
#
# provide histograms
svcMgr.ROBDataProviderSvc.doMonitoring = True
#False
# provide cost monitoring information from ROBs
svcMgr.ROBDataProviderSvc.doDetailedROBMonitoring = True
# simulate online like data access,
# if = True:  ROBs need to be registered before they can be retrieved as in online running
# if = False: Full event fragment is available as in offline running
svcMgr.ROBDataProviderSvc.SimulateOnlineDataAccess = True
# use a ROB-ROS mapping file to simulate ROS access
# (for the simulation of online running this should be True
#  and a ROB-ROS mapping should be provided. A simple ROB-ROS mapping file
#  can be generated with the tool generate-rob-ros-map-from-data.py in the
#  directory  python/scripts  of this package)
svcMgr.ROBDataProviderSvc.UseROSmappingForROBRetrieval = True
#
# svcMgr.ROBDataProviderSvc.RobRosMapping = {}
#
# for finding modules which make a ROB request in getROBData to a
# not registered ROB it is possible to obtain a complete backtrace
# if this option is switched on. The output may be however big.
svcMgr.ROBDataProviderSvc.PrintStackTraceGetROBData = False
#True
#
# for enabling the pre-fetching at algorithm level, debugging prefetching
svcMgr.ROBDataProviderSvc.enablePrefetchingAtAlgoLevel = True
#
#svcMgr.ROBDataProviderSvc.OutputLevel=DEBUG
print svcMgr.ROBDataProviderSvc
#==============================================================
#
# End of TrigROBDataProviderSvc_RTT job options fragment
#
#**************************************************************
