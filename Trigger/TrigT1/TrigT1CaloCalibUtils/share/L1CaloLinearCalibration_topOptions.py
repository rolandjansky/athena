###############################################################################################
# specify run number for this calibration before using this script
# e.g. athena -c 'RunNumber=123232' L1CaloLinearCalibration_topOptions.py
#
# in addition a database string can be specified for the output.
# default is:
# OutputDbConnection = "sqlite://;schema=energyscanresults.sqlite;dbname=L1CALO"
#
if not 'RunNumber' in dir():
    raise RuntimeError("Specifiy run number used for this calibration, e.g."
                       "athena -c 'RunNumber=123232' L1CaloLinearCalibration_topOptions.py")

if not 'OutputDbConnection' in dir():
    OutputDbConnection = "sqlite://;schema=energyscanresults.sqlite;dbname=L1CALO"
#
###############################################################################################

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

import AthenaPoolCnvSvc.AthenaPool
import IOVDbSvc.IOVDb

theApp.EvtMax                   = 1

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'bytestream'
globalflags.ConditionsTag = 'COMCOND-ES1C-000-00'
globalflags.DetDescrVersion = 'ATLAS-GEONF-04-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())
svcMgr.IOVDbSvc.overrideTags +=  ["<prefix>/CALO/Identifier/CaloTTOnOffIdMapAtlas</prefix> <tag>CALOIdentifierCaloTTOnOffIdMapAtlas-0002</tag>"]

include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
include("LArIdCnv/LArIdCnv_joboptions.py")

from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

svcMgr += getConfigurable( "CondProxyProvider" )()
svcMgr.CondProxyProvider.InputCollections = ["L1CaloRampData.pool.root"]

IOVSvc=Service("IOVSvc")
IOVSvc.preLoadData=True

include( "LArDetDescr/LArDetDescr_joboptions.py" )

from TrigT1CaloCalibUtils.TrigT1CaloCalibUtilsConf import L1CaloLinearCalibration
topSequence += L1CaloLinearCalibration()

# configure writing of pool files
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
include('RegistrationServices/IOVRegistrationSvc_jobOptions.py')
outputConditionsAlg = OutputConditionsAlg("outputConditionsAlg", "dummy.root")
outputConditionsAlg.ObjectList = ["CondAttrListCollection#/TRIGGER/L1Calo/Results/EnergyScanResults"]
outputConditionsAlg.WriteIOV = True
outputConditionsAlg.Run1 = RunNumber

svcMgr.IOVDbSvc.dbConnection= OutputDbConnection
