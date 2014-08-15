# this is a preOption file.  cf: COMCOND-BLKPST-002-00
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr 
from IOVDbSvc.CondDB import conddb
#force oracle
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
svcMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="/DBRelease/")
##PIX####################################################################################
# replace this fragment by JO using digi metadata.
conddb.addFolder("TDAQ","/TDAQ/EnabledResources/ATLAS/PIXEL/Modules","",forceData=True)
from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
pcss = PixelConditionsSummarySvc()
pcss.UseTDAQ = True
##SCT####################################################################################
pass
##TRT####################################################################################
#DONE IN GLOBAL TAG.
#pre-option.
#conddb.addFolderWithTag('TRT_OFL','/TRT/Cond/StatusPermanent','TrtStrawStatusPermanentAllBoardsBarrelIndividual-BLK-UPD4-00-00',forceData=True)
#conddb.addFolderWithTag('TRT_OFL','/TRT/Cond/Status','TrtStrawStatusTemporaryEmpty-BLK-UPD4-00-00',forceData=True)
##LAr####################################################################################
# in digitization
pass
##Tile####################################################################################
#DONE IN GLOBAL TAG.
#conddb.addFolderWithTag('TILE_OFL','/TILE/OFL02/STATUS/ADC','TileOfl02StatusAdc-REP-Sept2010-03',forceData=True)
pass
##Muons###################################################################################
pass
