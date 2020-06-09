

## AthAlgSeq INFO Member list:
##    LVL1::TriggerTowerMaker/TriggerTowerMaker,
##    LVL1::CPMTowerMaker/CPMTowerMaker,
##    LVL1::JetElementMaker/JetElementMaker,
##    LVL1::EmTauTrigger/EmTauTrigger,
##    LVL1::JetTrigger/JetTrigger,
##    LVL1::EnergyTrigger/EnergyTrigger,
##    LVL1::ROD/ROD,
##    LVL1::CPCMMMaker/CPCMMMaker,
##    LVL1::JEPCMMMaker/JEPCMMMaker,
##    LVL1MUCTPI::L1Muctpi/L1Muctpi_on_RDO,
##    LVL1CTP::CTPSimulation/CTPSimulation,
##    ROIB::RoIBuilder/RoIBuilder,
##    JetGlobalEventSetup,
##    HLT::TrigSteer/TrigSteer_L2,
##    HLT::TrigSteer/TrigSteer_EF,
##    TrigDec::TrigDecisionMaker/TrigDecMaker,
##    TrigInDetTrackTruthMaker,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamAOD_FeaturesDrop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamAOD_L2Drop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamESD_FeaturesDrop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamESD_L2Drop


import logging
log = logging.getLogger("skeleton.BStoTRIGBS")
log.info("="*20 + " Start of BStoTRIGBS " + "="*20)

# import -------------------------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags

# runargs -------------------------------------------------------------------------------


# setting input and output from runArgs
athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBS_OLDFile )
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBS_TRIGFile )

# setting trigger configuration from runArgs
TriggerFlags.triggerConfig.set_Value_and_Lock( runArgs.triggerConfig )

globalflags.DetDescrVersion = runArgs.geometryVersion
globalflags.ConditionsTag   = runArgs.conditionsTag






topSequence = AlgSequence()

#####################################################
#
# Setting trigger flags
#
#####################################################


# Trying manual setup of L1 instead of automatic (throgh L1ConfigGetter).
TriggerFlags.doLVL1 = False
TriggerFlags.doLVL2 = True
TriggerFlags.doEF = True

TriggerFlags.doTruth.set_Value_and_Lock(False)

TriggerFlags.doHLTpersistency = True
TriggerFlags.writeBS = True
TriggerFlags.abortOnConfigurationError = True

TriggerFlags.enableMonitoring = []

TriggerFlags.doID = True
TriggerFlags.doMuon = True
TriggerFlags.doCalo = True

TriggerFlags.CosmicSlice.testCosmic = False

#-------------------------------------------------------------------------------

#####################################################
#
# Athena flags
#
#####################################################


athenaCommonFlags.isOnline.set_Value_and_Lock(False)

globalflags.DetGeo = "atlas"
globalflags.DataSource = "geant4"
globalflags.Luminosity = "zero"


from AthenaCommon.BeamFlags import jobproperties as beamJobProperties
beamJobProperties.Beam.beamType = 'collisions'
globalflags.InputFormat = "bytestream"

#-------------------------------------------------------------------------------

#####################################################
#
# LVL1 simulation
#
#####################################################

from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
LVL1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
LVL1ConfigSvc.XMLFile = TriggerFlags.inputLVL1configFile()
#LVL1ConfigSvc.CreateLegacyObjects=True




include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
include ("TrigT1CaloSim/TrigT1CaloSimJobOptions.py" )

print "JOERG after including T1Calo simulation"
print topSequence.getChildren()

toRemove = [c for c in topSequence.getChildren() if c.name()=='TriggerTowerMaker']
for x in toRemove:
    topSequence.remove(x)

print "JOERG after removing TriggerTowerMaker"
print topSequence.getChildren()

from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
topSequence += LVL1__TrigT1MBTS()

from TrigT1BCM.TrigT1BCMConf import LVL1__TrigT1BCM
topSequence += LVL1__TrigT1BCM()

from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid
topSequence += LVL1__TrigT1Lucid()

from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
topSequence += L1Muctpi_on_RDO()
topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "/Run/L1MuCTPItoRoIBLocation"
topSequence.L1Muctpi_on_RDO.OverlapStrategyName = "NULL"

from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
topSequence += CTPSimulationInReco("CTPSimulation")
topSequence.CTPSimulation.IsData = False



from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
topSequence += RoIBuilder("RoIBuilder")

#-------------------------------------------------------------------------------

##################################
### DO THE MAIN CONFIGURATION  ###
##################################
# This used to be an include of a modified TriggerJobOpts/Trigger_topOptions_standalone.py

#HLTOutputLevel = 3
#include("TriggerJobOpts/Trigger_topOptions_standalone.py")


##############################
### SETTING DETECTOR FLAGS ###
##############################

DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.Calo_setOn()

DetFlags.makeRIO.all_setOn()




from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LoadElecCalib.set_Value_and_Lock(False)

# Setup IOVDbSvc
from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()


# Original comment: Setting detector geometry
include ("RecExCond/AllDet_detDescr.py")

if not hasattr(svcMgr,"ByteStreamCnvSvc"):
   from ByteStreamCnvSvc import ReadByteStream
   # Define the input
   svcMgr.EventSelector.Input = athenaCommonFlags.BSRDOInput()
   theApp.ExtSvc += [ "ByteStreamCnvSvc"]

# Online specific setup of BS converters
include( "TriggerJobOpts/jobOfragment_ReadBS_standalone.py" )



from InDetTrigRecExample.InDetTrigConfigConditions import \
    PixelConditionsSetup, SCT_ConditionsSetup, TRT_ConditionsSetup


PixelConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#PixelConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
PixelConditionsSetup.lock()
PixelConditionsSetup.createSvc()

SCT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#SCT_ConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
SCT_ConditionsSetup.lock()
SCT_ConditionsSetup.createTool()

TRT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#TRT_ConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
TRT_ConditionsSetup.lock()
TRT_ConditionsSetup.createSvc()

include( "InDetRecExample/InDetRecCabling.py" )






from CaloTools.CaloToolsConf import CaloCompactCellTool
svcMgr.ToolSvc += CaloCompactCellTool()

#if TriggerFlags.doMuon():

from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
muonByteStreamFlags.RpcDataType = 'atlas'
muonByteStreamFlags.MdtDataType = 'atlas'
muonByteStreamFlags.TgcDataType = 'atlas'

import MuonCnvExample.MuonCablingConfig
import MuonRecExample.MuonReadCalib

include ("MuonRecExample/MuonRecLoadTools.py")


#######################
### MENUS AND STUFF ###
#######################


from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu().generate()



from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc

_svc = SetupTrigConfigSvc()

_svc.l1XmlFile = TriggerFlags.inputLVL1configFile() if TriggerFlags.readLVL1configFromXML() \
        else TriggerFlags.outputLVL1configFile()

_svc.hltXmlFile = TriggerFlags.inputHLTconfigFile() if TriggerFlags.readHLTconfigFromXML() \
        else TriggerFlags.outputHLTconfigFile()





try:
    _svc.SetStates(["xml"])
except:
    log.error('Failed to set state of TrigConfigSvc to ["xml"]')
else:
    log.info('State of TrigConfigSvc set to ["xml"]')




try:
    _svc.InitialiseSvc()
except Exception, ex:
    log.error( 'Failed to activate TrigConfigSvc: %r' % ex )



_sl = dict([(svc, getattr(svcMgr, svc)) for svc in ['LVL1ConfigSvc', 'HLTConfigSvc'] if svc in dir(svcMgr)])

if TriggerFlags.triggerDbConnection():
    from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection
    _conn_param = interpretConnection(TriggerFlags.triggerDbConnection(), resolveAlias=False)

    if "alias" in _conn_param:
        for svc in _sl.values():
            svc.ConfigSource = 'DBLookUp'
            svc.DBServer  = _conn_param["alias"]
            svc.DBUser = ""
    else:
        technology = _conn_param["techno"]
        svcMgr.LVL1ConfigSvc.ConfigSource = technology
        svcMgr.HLTConfigSvc.ConfigSource  = technology

        if technology == 'sqlite':
            for svc in _sl.values():
                svc.DBServer = _conn_param["filename"]
                svc.DBUser = "dummy"
        elif technology == 'oracle':
            for svc in _sl.values():
                svc.DBServer  = _conn_param["server"]
                svc.DBAccount = _conn_param["schema"]
                svc.DBUser    = _conn_param["user"]
                svc.DBPass    = _conn_param["passwd"]

    svcMgr.LVL1ConfigSvc.DBSMKey     = TriggerFlags.triggerDbKeys()[0]
    svcMgr.LVL1ConfigSvc.DBLVL1PSKey = TriggerFlags.triggerDbKeys()[1]
    svcMgr.LVL1ConfigSvc.DBBGSKey    = TriggerFlags.triggerDbKeys()[3]

    svcMgr.HLTConfigSvc.DBSMKey      = TriggerFlags.triggerDbKeys()[0]
    svcMgr.HLTConfigSvc.DBHLTPSKey   = TriggerFlags.triggerDbKeys()[2]


for n, svc in _sl.items():
    log.info("Service     %s %r" % (n, dir(svc)) )



from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
lvl1 = Lvl1SimulationGetter()

from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
hlt = HLTSimulationGetter()



rng = getattr(ToolSvc,'HLT::RandomScaler')
rng.useEventSeed = True
#EF gets an extra seed "hash-offset" of 1 to make it independent of L2
rng.config(seed=int(TriggerFlags.doEF()), luxury=2)

### End of the mess.....
###############################





# Must make sure that no OutStream's have been declared
theApp.OutStream = []

# Enable Aditors (i.e. AlgContextAuditor to get algorithm stack in CoreDumpSvc)
theApp.AuditAlgorithms = True



#-------------------------------------------------------------------------------

########################################
#
#   POST CONFIGURE RERUNNING OF LVL1
#
########################################

# Not tested if necessary
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "MuCTPI_RIO/MUCTPI_RIO", "CTP_RIO/CTP_RIO" ]


#-------------------------------------------------------------------------------

########################################
#
#  Define output
#
########################################

#athenaCommonFlags.BSRDOOutput = _o # Expected to be set elsewhere
from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
StreamBSFileOutput.OutputFile = athenaCommonFlags.BSRDOOutput()
ServiceMgr.ByteStreamCnvSvc.IsSimulation = True



### # Taken from storegate dump
### StreamBSFileOutput.ItemList += [ "CTP_RDO#*" ]
### #StreamBSFileOutput.ItemList += [ "CTP_RIO#*" ] # No RIO
### #StreamBSFileOutput.ItemList += [ "CscDigitContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "CscRawDataContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "DataHeader#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "DataVector< LVL1::RecEnergyRoI >#*" ] # No converter
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMCPHits>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMEtSums>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMJetHits>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMHits>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ] # Chosen by data address
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMTower>#CPMTowers" ] # Chosen by data address
### #StreamBSFileOutput.ItemList += [ "DataVector<LVL1::EmTauROI>#*" ] # No converter
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMEtSums>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMHits>#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMRoI>#JEMRoIs" ] # Chosen by data address
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetElement>#*" ]
### #StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetROI>#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ] # Chosen by data address
### #StreamBSFileOutput.ItemList += [ "DataVector<LVL1CTP::SlinkWord>#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "DataVector<TrigMissingET>#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "EventInfo#*" ] # Not relevant
### StreamBSFileOutput.ItemList += [ "HLT::HLTResult#*" ]
### #StreamBSFileOutput.ItemList += [ "HLT::TrigEDMSizes#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "InDet::PixelClusterContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "InDet::SCT_ClusterContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "InDet::SiClusterContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "InDet::TRT_DriftCircleContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "InDetBSErrContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "L1MUINT::MuCTPIToRoIBSLink#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LArRawChannelContainer#*" ] # Not relevant
### StreamBSFileOutput.ItemList += [ "LVL1::CMMRoI#*" ]
### StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#*" ]
### #StreamBSFileOutput.ItemList += [ "LVL1::EmTauCTP#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1::EnergyCTP#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1::EnergyRoI#*" ] # No converter
### StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#*" ]
### StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#*" ]
### #StreamBSFileOutput.ItemList += [ "LVL1::JetCTP#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1::JetEtRoI#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1::MuCTPICTP#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1CTP::CTPSLink#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "LVL1CTP::Lvl1Result#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "MdtCsmContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "MdtDigitContainer#*" ] # Not relevant
### StreamBSFileOutput.ItemList += [ "MuCTPI_RDO#MUCTPI_RDO+" ]
### #StreamBSFileOutput.ItemList += [ "MuCTPI_RIO#*" ] # No RIO
### #StreamBSFileOutput.ItemList += [ "ROIB::RecRoIBResult#*" ] # No RIO
### StreamBSFileOutput.ItemList += [ "ROIB::RoIBResult#*" ]
### #StreamBSFileOutput.ItemList += [ "RpcDigitContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "RpcPadContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "RpcSectorLogicContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "SpacePointContainer#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "TRT_BSErrContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TRT_BSIdErrContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TgcDigitContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TgcRdoContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TileCellIDC#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TileRawChannelContainer#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TrigDec::TrigDecision#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "TrigInDetTrackTruthMap#*" ] # Not relevant
### #StreamBSFileOutput.ItemList += [ "TrigMissingETContainer#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "TrigOperationalInfoCollection#*" ] # No converter
### #StreamBSFileOutput.ItemList += [ "std::vector<unsigned int>#*" ] # Not relevant
### 
### # LVL1
### StreamBSFileOutput.ItemList = [ "ROIB::RoIBResult#*" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
### StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#JEPBSCollection" ]
### StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
### StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#CPBSCollection" ]
### StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]
### 
### # LVL2
### StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
### StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]
### 
### 
### 
### StreamBSFileOutput.ItemList   += [ "CTP_RDO#CTP_RDO" ]
### StreamBSFileOutput.ItemList   += [ "MuCTPI_RDO#MUCTPI_RDO+" ]


#-------------------------------------------------------------------------------

###################
### POST CONFIG ###
###################

theApp.EvtMax = runArgs.maxEvents

svcMgr.StoreGateSvc.Dump = True

print svcMgr

svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG

print "Joerg All Services:"
for svc in svcMgr.getChildren():
    print "       ",svc
    if "CnvSvc" in svc.name():
        svc.OutputLevel = VERBOSE
    if "InputSvc" in svc.name():
        svc.OutputLevel = VERBOSE



svcMgr.ByteStreamEventStorageOutputSvc.OutputLevel = DEBUG


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

print "Joerg Algorithms"
import AthenaCommon.Configurable as Configurable
Configurable.log.setLevel( INFO )
print topSequence.getChildren()
print topSequence



from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
chainMoni = TrigChainMoni()
chainMoni.OutputLevel=VERBOSE
topSequence.TrigSteer_L2.MonTools += [chainMoni]
topSequence.TrigSteer_EF.MonTools += [chainMoni]


#-------------------------------------------------------------------------------

log.info("="*20 + " End of BStoTRIGBS " + "="*20)
