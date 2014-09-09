# Setup DetStatusSvc to read from conditions database
#
# service setup is implicit - only needed explicitly if you set parameters
# as given in following commented out 3 lines
# from AthenaCommon.AppMgr import ServiceMgr
# from DetectorStatus.DetectorStatusConf import DetStatusSvc
# ServiceMgr+=DetStatusSvc(StringReqs='TRTB 2')

# set up conditions DB access
# conditions DB tag is not specified - will be taken from hierarchical tagging
#from IOVDbSvc.CondDB import conddb
#conddb.addFolder('GLOBAL_OFL','/GLOBAL/DETSTATUS/LBSUMM')
import logging
local_logger = logging.getLogger('DetStatusSvc_CondDB.py')
local_logger.warning('Use of DetStatusSvc DEPRECATED for Run 2; this file will go away soon')
del local_logger
