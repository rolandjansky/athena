from glob import glob

filelist = ['/var/clus/usera/will/testareas/ValidateDPS/DigiJobOptions/out.allConstantsSV_mV_LSB_RDO.pool.root']

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = filelist
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections =  athenaCommonFlags.FilesInput()


svcMgr += CfgMgr.DBReplicaSvc(UseCOOLSQLite=False) #speeds up things a bit
from IOVDbSvc.CondDB import conddb
conddb.addFolder('','/Digitization/Parameters')
conddb.addFolder('LAR_OFL',"/LAR/Identifier/LArTTCellMapAtlas")
conddb.addFolder('CALO',"/CALO/Identifier/CaloTTOnOffIdMapAtlas")
conddb.addFolder('CALO',"/CALO/Identifier/CaloTTOnAttrIdMapAtlas")
conddb.addFolder('CALO',"/CALO/Identifier/CaloTTPpmRxIdMapAtlas")
conddb.addFolder('LAR_OFL',"/LAR/IdentifierOfl/OnOffIdMap_SC")

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

#use filepeeker to set some variables
from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if "IS_DATA" in inputFileSummary['evt_type'] else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']
##before loading det description, turn off everything except Calorimeter - quicker
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()

include("RecExCond/AllDet_detDescr.py")


job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.SuperCellVsCaloCellTestAlg("MyAlg")

#svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=500)

if not hasattr(svcMgr,"THistSvc"): svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output +=  ["SUPERCELLTEST DATAFILE='hist.root' OPT='RECREATE' "]