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

# menu with default configuration for testing
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
flags.Input.Files = athenaCommonFlags.FilesInput()
flags.Trigger.triggerConfig = "FILE"
flags.lock()
CAtoGlobalWrapper(L1ConfigSvcCfg,flags)

from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
generateL1Menu(flags)

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile_jfex.root' OPT='RECREATE'"]


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamAOD_Augmented = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.jFEX.output.root" )
StreamAOD = StreamAOD_Augmented.GetEventStream()

# the jFex containers
StreamAOD.ItemList+=["xAOD::jFexSRJetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSRJetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexLRJetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexLRJetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::TriggerTowerContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexTauRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexTauRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSumETRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSumETRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexMETRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexMETRoIAuxContainer#*"]

#Physics Objects
StreamAOD.ItemList+=["xAOD::JetContainer#Anti*"]
StreamAOD.ItemList+=["xAOD::JetAuxContainer#Anti*"]
StreamAOD.ItemList+=["xAOD::JetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::JetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::ElectronContainer#Electrons"]
StreamAOD.ItemList+=["xAOD::ElectronAuxContainer#ElectronsAux."]
StreamAOD.ItemList+=["xAOD::TauJetContainer#TauJets"]
StreamAOD.ItemList+=["xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters."]








#######################################################
log.info("==========================================================")
log.info("Scheduling jFEXDriver")
athAlgSeq += CfgMgr.LVL1__jFEXDriver('MyjFEXDriver')
athAlgSeq += CfgMgr.LVL1__jFEXNtupleWriter('MyjFEXNtupleWriter')
log.info("==========================================================")
#######################################################
