include.block("DetectorStatus/DetStatusSvc_ToFileMetaData.py")
# write Detector Status information to FileMetaData
# add the folder to the list of objects which IOVDbSvc will transfer to the meta-data
# We no longer want to write any of this stuff
#from IOVDbSvc.CondDB import conddb
#conddb.toMetaData('/GLOBAL/DETSTATUS/LBSUMM')
import logging
local_logger = logging.getLogger('DetectorStatus/DetStatusSvc_ToFileMetaData.py')
local_logger.warning('Use of DetStatusSvc DEPRECATED for Run 2; this file will go away soon')
del local_logger
