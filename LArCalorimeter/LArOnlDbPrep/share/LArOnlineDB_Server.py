###############################################################
#
# Job options file
#
#==============================================================

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

ServiceMgr.EventSelector.RunNumber=0xFFFFFF ## need large number; OxFFFFFFFF is too large
print ServiceMgr.EventSelector.RunNumber
ServiceMgr.EventSelector.EventsPerRun=100000
ServiceMgr.EventSelector.FirstEvent=1

LArOnline=False

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-20-00-01')
globalflags.DetGeo.set_Value_and_Lock('commis')

#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1PA-006-04"

from IOVDbSvc.CondDB import conddb

# LAr ID Converters; LAr Cabling Service
include( "LArIdCnv/LArIdCnv_joboptions.py" )

# barrel A1 (new scheme)
LArCoolChannelSelection="35:66,149:180,271:302,385:416,507:538,621:652,1001,1005,1013,1017,1025,1029" 

from LArConditionsCommon.LArCondFlags import larCondFlags 
larCondFlags.SingleVersion = True
larCondFlags.useShape = True 
larCondFlags.OFCShapeFolder = ""

svcMgr.PoolSvc.ReadCatalog += ["prfile:poolcond/PoolCat_diskbuffer.xml"] ## only outside P1
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py")

#conddb.addFolder("","<db>sqlite://;schema=LArDSPThresholds.db;dbname=COMP200</db><tag>testTag</tag>/LAR/Online/DSPThresholds")
##conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels") ## done in LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py
##conddb.addFolder(LArDB,"/LAR/BadChannels/MissingFEBs") ## done in LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py

conddb.addFolder(LArDB,"/LAR/Configuration/DSPThreshold/Thresholds")
#conddb.addFolder("","<db>sqlite://;schema=fixedtQThr250sampleThr1000.db;dbname=COMP200</db><tag>testTag</tag>/LAR/Online/DSPThresholds")

if LArOnline:
  try:
      print 'Trying to remove PoolCat_comcond_castor.xml'
      PoolSvc.ReadCatalog.remove("prfile:poolcond/PoolCat_comcond_castor.xml")
  except ValueError:
      print 'PoolCat_comcond_castor.xml cannot be removed'

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel      = INFO
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.warningLimit     = 50

theApp.EvtMax = 2 ## may need larger number -- test this

#--------------------------------------------------------------
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
# Check the dictionary in memory for completeness
AthenaSealSvc = Service( "AthenaSealSvc" )
# AthenaSealSvc.CheckDictionary = True
AthenaSealSvc.OutputLevel     = ERROR

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

if LArOnline:
    # Enable Oracle Replica in case of local sqlite replica is corrupted
    PoolSvc=Service("PoolSvc")
    PoolSvc.SortReplicas=False
    DBReplicaSvc=Service("DBReplicaSvc")
    DBReplicaSvc.UseCOOLSQLite=False
    
#######################################################
# fix of LArOnlDbPrepTool on 03/12/2008
#######################################################
from LArOnlDbPrep.LArOnlDbPrepConf import LArOnlDbPrepTool
dbPrep = LArOnlDbPrepTool()
svcMgr.ToolSvc += dbPrep

import PyCintex

# this must be the last one, since proxy classes are patched by this
include ('PyKernel/InitPyKernel.py')


svcMgr.ToolSvc.LArOnlDbPrepTool.keyOFC = "LArOFC" 
svcMgr.ToolSvc.LArOnlDbPrepTool.keyPedestal = "Pedestal" 
svcMgr.ToolSvc.LArOnlDbPrepTool.keyRamp = "LArRamp" 
svcMgr.ToolSvc.LArOnlDbPrepTool.OutputLevel = DEBUG
svcMgr.ToolSvc.LArOnlDbPrepTool.keyDSPThresholds = "LArDSPThresholds"
svcMgr.ToolSvc.LArOnlDbPrepTool.applyRampIntercept = True
#Set these to False to be backwards-compatible
svcMgr.ToolSvc.LArOnlDbPrepTool.FCalInMediumGain = False
svcMgr.ToolSvc.LArOnlDbPrepTool.EMECIWInMediumGain = False

theApp.initialize() 
theApp.nextEvent(1)

import AthenaPython.PyAthena as PyAthena
prep = PyAthena.py_tool("LArOnlDbPrepTool")
prep.prepare()  

include ("LArOnlDbPrep/LArOnlineDB_Client.py")

theApp.finalize()

theApp.exit()
