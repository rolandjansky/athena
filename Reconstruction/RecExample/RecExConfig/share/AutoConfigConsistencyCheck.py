# check consistency between AthenaCommonFlags.FilesInput and
# ServiceMgr.EventSelector.FilesInput
#TODO to be included at the end of RecExCommon_flags.py
#     treat BS case
#     protect for online

import RecExConfig.PyComps as recpc
#svcMgr += MySvc("DRMySvc")
svcMgr += recpc.AutoConfigConsistencyCheckSvc("AutoConfigConsistencyCheckSvc")

