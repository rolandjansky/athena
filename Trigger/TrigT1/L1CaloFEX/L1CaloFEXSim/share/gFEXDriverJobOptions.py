#jps.AthenaCommonFlags.AccessMode = "POOLAccess" # use POOL read mode because reading calocells
#svcMgr.EventSelector.InputCollections = jps.AthenaCommonFlags.FilesInput()
from AthenaCommon.GlobalFlags  import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

if type(theApp).__name__ == "fakeAppMgr": theApp.initialize() #this line cuts off pathena when joboption parsing ... since all outputs now declared

include( "LArDetDescr/LArDetDescr_joboptions.py" )


include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )

IOVBeginRun   = IOVRunNumberMin
IOVEndRun     = IOVRunNumberMax
IOVBeginLB = IOVLBNumberMin
IOVEndLB   = IOVLBNumberMax

import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc


if "GlobalTag" not in dir():
    GlobalTag     = 'OFLCOND-CSC-00-01-00'

svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

# configure detector description from metadata in input file
from RecExConfig import AutoConfiguration
AutoConfiguration.ConfigureSimulationOrRealData()
AutoConfiguration.ConfigureGeo()
AutoConfiguration.ConfigureConditionsTag()
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile.root' OPT='RECREATE'"]
#######################################################
log.info("==========================================================")
log.info("Scheduling gFEXDriver")
athAlgSeq += CfgMgr.LVL1__gFEXDriver('MygFEXDriver')
#athAlgSeq += CfgMgr.LVL1__gFEXNtupleWriter('MygFEXNtupleWriter')
log.info("==========================================================")
#######################################################
