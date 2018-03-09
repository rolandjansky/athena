###############################################################
#
# Job options file for CaloCellCalcEnergyCorr
#
#==============================================================


sqlite="EMB_EMEC_PS_800HV_CellEnergyCorrection.db"

# write here the run number and lumiblock of a time after temperature drop

RunNumber = 274936
LumiBlock = 1

#LastRunNumber = 999999

GlobalTag =  'CONDBR2-ES1PA-2017-09'
Geometry = 'ATLAS-R2-2015-04-00-00'

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(RunNumber)

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

trigDB=indirectOpen('COOLONL_TRIGGER/CONDBR2',oracle=True)
trigfolder=trigDB.getFolder('/TRIGGER/LUMI/LBLB')
runiov=(RunNumber << 32)+ LumiBlock
print " runiov ", runiov
obj=trigfolder.findObject(runiov,0)
payload=obj.payload()
TimeStamp=payload['StartTime']/1000000000L
trigDB.closeDatabase()

# this setting is just to get directly pileup noise as b and write back the same in the database...
from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.)

#TimeStamp = 1274368420

print " TimeStamp : ",TimeStamp


#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring = True
#from AthenaCommon.Resilience import treatException,protectedInclude
#protectedInclude( "PerfMonComps/PerfMonSvc_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')

import AthenaCommon.AtlasUnixGeneratorJob

# Get a handle to the default top-level algorithm sequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

#from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
#theCaloNoiseTool = CaloNoiseToolDefault()
#ToolSvc += theCaloNoiseTool

from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
theLArHVCorrTool=LArHVCorrToolDefault()
ToolSvc += theLArHVCorrTool

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloCellCalcEnergyCorr
theRescaler=CaloCellCalcEnergyCorr("EMBRescaler")
theRescaler.CaloSamples = [ 0, 4 ]
# the EMBPS has a 1200V OFC bias (1.005)
# and EMECPS has a 2000V OFC on 1600V bias (0.9943)
theRescaler.SampleValues = [ 1.005, 0.9943 ]
theRescaler.HVLines = [ 281014, 283014, 293013, 295013, 288000, 290000, 289000, 291000, 289004, 291004, 289008, 291008, 292000, 294000, 280000, 282000, 281004, 283004, 284000, 286000, 284008, 286008, 268008, 270008, 264012, 266012, 284014, 286014]
theRescaler.HVvalues = [ 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992 ]

topSequence += theRescaler

from CaloCondPhysAlgs.CaloCondPhysAlgsConf import LArHVMapTool
theLArHV=LArHVMapTool("LArHVMapTool")
ToolSvc += theLArHV

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = LumiBlock
svcMgr.EventSelector.InitialTimeStamp  = TimeStamp
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1


#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
## if not hasattr(ServiceMgr, 'THistSvc'):
##    from GaudiSvc.GaudiSvcConf import THistSvc
##    ServiceMgr += THistSvc()

## ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise_data.root' OPT='RECREATE'"];

OutputList=[ "AthenaAttributeList#/LAR/CellCorrOfl/EnergyCorr", ]
OutputTagList=["LARCellCorrOflEnergyCorr-UPD1-00",]


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                           OutputList,OutputTagList,True)


#Periode C1-C8 (28 Jun - 23 Jul)
theOutputConditionsAlg.Run1 = RunNumber
theOutputConditionsAlg.LB1 = 1
	
#theOutputConditionsAlg.Run2 = LastRunNumber + 1
#theOutputConditionsAlg.LB2 = 0
	
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+sqlite+";dbname=CONDBR2"
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = True #Allow add in a second tag
svcMgr.IOVRegistrationSvc.OverrideNames += ["CaloCondBlob16M"]
svcMgr.IOVRegistrationSvc.OverrideTypes += [ "Blob16M" ]


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
#svcMgr.IOVDbSvc.OutputLevel        = 




