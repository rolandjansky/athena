import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = inFileName

## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-R2-2016-01-00-01"

from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
#oks.addStreamESD('CaloCellContainer', ['AllCalo'] )
oks.addStreamESD('CaloCalibrationHitContainer',['LArCalibrationHitActive','LArCalibrationHitInactive','LArCalibrationHitDeadMaterial_DEAD','TileCalibHitActiveCell','TileCalibHitInactiveCell','TileCalibHitDeadMaterial'])

## re-do topo clusters on EM scale
#from CaloRec.CaloRecFlags import jobproperties
#from CaloRec.CaloTopoClusterFlags import jobproperties
#jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
#jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
#from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter, addSnapshot
#CaloClusterTopoGetter()

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

svcMgr.IOVDbSvc.GlobalTag = "OFLCOND-MC16-SDR-16"
svcMgr.IOVDbSvc.forceRunNumber=284500

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='LArEM_SF.root' OPT='RECREATE'"]

#from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCWeights
from AthenaCommon.SystemOfUnits import deg, GeV, MeV, TeV
from AthenaCommon.AlgSequence import AlgSequence
#from math import ( pi as m_pi, log10 as m_log10 )

LarEMSamplingFraction = CfgMgr.LarEMSamplingFraction()
LarEMSamplingFraction.DoCells = 0
LarEMSamplingFraction.CalibrationHitContainerNames = ["LArCalibrationHitInactive"
                                       ,"LArCalibrationHitActive"
                                       ,"TileCalibHitInactiveCell"
                                       ,"TileCalibHitActiveCell"]
                                       #,"LArCalibrationHitDeadMaterial_DEAD"
                                       #,"TileCalibHitDeadMaterial"]
topSequence += LarEMSamplingFraction

theApp.EvtMax = -1

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO #2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL
MessageSvc.infoLimit = 100000

svcMgr.EventSelector.InputCollections = inFileName
