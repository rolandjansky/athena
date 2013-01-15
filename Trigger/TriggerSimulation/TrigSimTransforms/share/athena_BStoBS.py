#-------------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
#-------------------------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
#-------------------------------------------------------------------------------
log = logging.getLogger("BStoBS.py")
topSequence = AlgSequence()
#-------------------------------------------------------------------------------

#############################
### SETTING TRIGGER FLAGS ###
#############################

# Trying manual setup of L1 instead of automatic (throgh L1ConfigGetter).
TriggerFlags.doLVL1 = False
TriggerFlags.doLVL2 = True
TriggerFlags.doEF = True

TriggerFlags.doTruth.set_Value_and_Lock(False)

TriggerFlags.doHLTpersistency = True
TriggerFlags.writeBS = True
TriggerFlags.abortOnConfigurationError = True



#TriggerFlags.triggerConfig = _trig # Expected to be set elsewhere


TriggerFlags.enableMonitoring = []

TriggerFlags.doID = True
TriggerFlags.doMuon = True
TriggerFlags.doCalo = True

TriggerFlags.CosmicSlice.testCosmic = False

#-------------------------------------------------------------------------------

############################
### SETTING ATHENA FLAGS ###
############################

athenaCommonFlags.isOnline.set_Value_and_Lock(False)

globalflags.DetGeo = "atlas"
globalflags.DataSource = "geant4"


#globalflags.DetDescrVersion = _geo # Expected to be set elsewhere
#globalflags.ConditionsTag = _con # Expected to be set elsewhere

globalflags.Luminosity = "zero"


from AthenaCommon.BeamFlags import jobproperties as beamJobProperties
beamJobProperties.Beam.beamType = 'collisions'


globalflags.InputFormat = "bytestream"
#athenaCommonFlags.BSRDOInput = [ _i ] # Expected to be set elsewhere

#-------------------------------------------------------------------------------

############################################
### CONFIGUREING LVL1 SIMULATION BY HAND ###
############################################

from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
LVL1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
LVL1ConfigSvc.XMLFile = TriggerFlags.inputLVL1configFile()
LVL1ConfigSvc.CreateLegacyObjects=True


include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
include ("TrigT1CaloSim/TrigT1CaloSimJobOptions.py" )


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
# This used to be an include of a modified TriggerRelease/Trigger_topOptions_standalone.py

#HLTOutputLevel = 3
#include("TriggerRelease/Trigger_topOptions_standalone.py")


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



print "Hest:: not hasattr(svcMgr,'ByteStreamCnvSvc')", not hasattr(svcMgr,"ByteStreamCnvSvc")
if not hasattr(svcMgr,"ByteStreamCnvSvc"):
   from ByteStreamCnvSvc import ReadByteStream
   # Define the input
   svcMgr.ByteStreamInputSvc.FullFileName = athenaCommonFlags.BSRDOInput()
   theApp.ExtSvc += [ "ByteStreamCnvSvc"]

# Online specific setup of BS converters
print "Hest:: ", "\"Online specific setup of BS converters\""
include( "TriggerRelease/jobOfragment_ReadBS_standalone.py" )






from InDetTrigRecExample.InDetTrigConfigConditions import \
    PixelConditionsSetup, SCT_ConditionsSetup, TRT_ConditionsSetup


print "Hest:: <id>ConditionsSetup(onlineMode=False) breaks something."
PixelConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#PixelConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
PixelConditionsSetup.lock()
PixelConditionsSetup.createSvc()

SCT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#SCT_ConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
SCT_ConditionsSetup.lock()
SCT_ConditionsSetup.createSvc()

TRT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
#TRT_ConditionsSetup.config(useDCS=False, onlineMode=False, prefix='')
TRT_ConditionsSetup.lock()
TRT_ConditionsSetup.createSvc()

print "Unexplored/unknown include InDetRecCabling ?????"
include( "InDetRecExample/InDetRecCabling.py" )






from CaloTools.CaloToolsConf import CaloCompactCellTool
svcMgr.ToolSvc += CaloCompactCellTool()

#if TriggerFlags.doMuon():
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
    print "Hest:::", n, dir(svc)
    print



#l2algs, efalgs = ServiceMgr.HLTConfigSvc.getAlgorithmsByLevel()
#print l2algs
#print efalgs




from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
lvl1 = Lvl1SimulationGetter()

from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
hlt = HLTSimulationGetter()



#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#TriggerConfigGetter()


#from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
#from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
#lvl1 = Lvl1SimulationGetter()


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
### POST CONFIGURE RERUNNING OF LVL1 ###
########################################

#Not tested if necessary
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "MuCTPI_RIO/MUCTPI_RIO", "CTP_RIO/CTP_RIO" ]


#-------------------------------------------------------------------------------

#####################
### Define output ###
#####################

#athenaCommonFlags.BSRDOOutput = _o # Expected to be set elsewhere
from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
print "Hest: ", StreamBSFileOutput
print "Hest: ", StreamBSFileOutput.__class__
StreamBSFileOutput.OutputFile = athenaCommonFlags.BSRDOOutput()
ServiceMgr.ByteStreamCnvSvc.IsSimulation = True


"""
# Taken from storegate dump
StreamBSFileOutput.ItemList += [ "CTP_RDO#*" ]
#StreamBSFileOutput.ItemList += [ "CTP_RIO#*" ] # No RIO
#StreamBSFileOutput.ItemList += [ "CscDigitContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "CscRawDataContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "DataHeader#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "DataVector< LVL1::RecEnergyRoI >#*" ] # No converter
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMCPHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMEtSums>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMJetHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ] # Chosen by data address
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMTower>#CPMTowers" ] # Chosen by data address
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::EmTauROI>#*" ] # No converter
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMEtSums>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMRoI>#JEMRoIs" ] # Chosen by data address
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetElement>#*" ]
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetROI>#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ] # Chosen by data address
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1CTP::SlinkWord>#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "DataVector<TrigMissingET>#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "EventInfo#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "HLT::HLTResult#*" ]
#StreamBSFileOutput.ItemList += [ "HLT::TrigEDMSizes#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "InDet::PixelClusterContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "InDet::SCT_ClusterContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "InDet::SiClusterContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "InDet::TRT_DriftCircleContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "InDetBSErrContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "L1MUINT::MuCTPIToRoIBSLink#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LArRawChannelContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "LVL1::CMMRoI#*" ]
StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#*" ]
#StreamBSFileOutput.ItemList += [ "LVL1::EmTauCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::EnergyCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::EnergyRoI#*" ] # No converter
StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#*" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#*" ]
#StreamBSFileOutput.ItemList += [ "LVL1::JetCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::JetEtRoI#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::MuCTPICTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1CTP::CTPSLink#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1CTP::Lvl1Result#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "MdtCsmContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "MdtDigitContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "MuCTPI_RDO#MUCTPI_RDO+" ]
#StreamBSFileOutput.ItemList += [ "MuCTPI_RIO#*" ] # No RIO
#StreamBSFileOutput.ItemList += [ "ROIB::RecRoIBResult#*" ] # No RIO
StreamBSFileOutput.ItemList += [ "ROIB::RoIBResult#*" ]
#StreamBSFileOutput.ItemList += [ "RpcDigitContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "RpcPadContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "RpcSectorLogicContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "SpacePointContainer#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "TRT_BSErrContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TRT_BSIdErrContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TgcDigitContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TgcRdoContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TileCellIDC#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TileRawChannelContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TrigDec::TrigDecision#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "TrigInDetTrackTruthMap#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TrigMissingETContainer#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "TrigOperationalInfoCollection#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "std::vector<unsigned int>#*" ] # Not relevant

# LVL1
StreamBSFileOutput.ItemList = [ "ROIB::RoIBResult#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#JEPBSCollection" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#CPBSCollection" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]

# LVL2
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]



StreamBSFileOutput.ItemList   += [ "CTP_RDO#CTP_RDO" ]
StreamBSFileOutput.ItemList   += [ "MuCTPI_RDO#MUCTPI_RDO+" ]

"""

#-------------------------------------------------------------------------------

###################
### POST CONFIG ###
###################

theApp.EvtMax = -1

svcMgr.StoreGateSvc.Dump = True

print svcMgr

#-------------------------------------------------------------------------------
