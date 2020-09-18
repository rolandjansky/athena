#jps.AthenaCommonFlags.AccessMode = "POOLAccess" # use POOL read mode because reading calocells
#svcMgr.EventSelector.InputCollections = jps.AthenaCommonFlags.FilesInput()
from AthenaCommon.GlobalFlags  import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

if type(theApp).__name__ == "fakeAppMgr": theApp.initialize() #this line cuts off pathena when joboption parsing ... since all outputs now declared

#from RecExConfig import AutoConfiguration
#AutoConfiguration.ConfigureSimulationOrRealData()
#AutoConfiguration.ConfigureGeo()
#AutoConfiguration.ConfigureConditionsTag()
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.Calo_setOn()
#include("RecExCond/AllDet_detDescr.py")

#include( "CaloConditions/CaloConditions_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )


include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )

IOVBeginRun   = IOVRunNumberMin
IOVEndRun     = IOVRunNumberMax
IOVBeginLB = IOVLBNumberMin
IOVEndLB   = IOVLBNumberMax

import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc


#svcMgr.IOVDbSvc.DBInstance=""

if "GlobalTag" not in dir():
    GlobalTag     = 'OFLCOND-CSC-00-01-00' #Sadly event his doesn't work for the 14 TeV jetjet sample 'OFLCOND-MC15c-SDR-14-02' #This works for 13 TeV 'OFLCOND-CSC-00-01-00'    #No idea what this is: COMCOND-BLKPST-004-05'

#svcMgr.IOVDbSvc.GlobalTag = GlobalTag

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
#svcMgr.THistSvc.Output += ["ISO DATAFILE='tobIso.root' OPT='RECREATE'"]
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile.root' OPT='RECREATE'"]
#######################################################
log.info("==========================================================")
log.info("Scheduling eFEXDriver")
athAlgSeq += CfgMgr.LVL1__eFEXDriver('MyeFEXDriver')
athAlgSeq += CfgMgr.LVL1__eFEXNtupleWriter('MyeFEXNtupleWriter')
log.info("==========================================================")
#######################################################
