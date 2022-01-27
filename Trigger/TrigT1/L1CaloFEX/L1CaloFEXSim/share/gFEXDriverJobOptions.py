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

# menu with default configuration for testing
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
flags.lock()
CAtoGlobalWrapper(L1ConfigSvcCfg,flags)

from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
generateL1Menu(flags)


svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile.root' OPT='RECREATE'"]


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamAOD_Augmented = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.gFEX.output.root" )
StreamAOD = StreamAOD_Augmented.GetEventStream()

# Generic event info
StreamAOD.ItemList+=["xAOD::EventInfo#*"]
StreamAOD.ItemList+=["xAOD::EventAuxInfo#*"]

# # the gFex containers
StreamAOD.ItemList+=["xAOD::gFexJetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::gFexJetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::gFexGlobalRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::gFexGlobalRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::TriggerTowerContainer#*"]

#Physics Objects
StreamAOD.ItemList+=["xAOD::JetContainer#*"]
StreamAOD.ItemList+=["xAOD::JetAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo"]
StreamAOD.ItemList+=["xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.-ConstitObjectLinks.-ConstitObjectWeights"]




#######################################################
log.info("==========================================================")
log.info("Scheduling gFEXDriver")
athAlgSeq += CfgMgr.LVL1__gFEXDriver('MygFEXDriver')
athAlgSeq += CfgMgr.LVL1__gFEXNtupleWriter('MygFEXNtupleWriter')
log.info("==========================================================")
#######################################################
