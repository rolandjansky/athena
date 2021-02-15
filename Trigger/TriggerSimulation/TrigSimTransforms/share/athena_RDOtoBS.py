# this runs


## Algorithm AthSequencer/TopAlg:
##     'EventCounter/EventCounter',
##     'LVL1::TriggerTowerMaker/TriggerTowerMaker',
##     'LVL1::CPMTowerMaker/CPMTowerMaker',
##     'LVL1::JetElementMaker/JetElementMaker',
##     'LVL1::EmTauTrigger/EmTauTrigger',
##     'LVL1::JetTrigger/JetTrigger',
##     'LVL1::EnergyTrigger/EnergyTrigger',
##     'LVL1::ROD/ROD',
##     'LVL1::CPCMMMaker/CPCMMMaker',
##     'LVL1::JEPCMMMaker/JEPCMMMaker',
##     'LVL1::TrigT1MBTS/LVL1::TrigT1MBTS',
##     'MuonRdoToMuonDigit/MuonRdoToMuonDigit',
##     'TrigT1RPC/TrigT1RPC',
##     'LVL1TGCTrigger::LVL1TGCTrigger/LVL1TGCTrigger',
##     'LVL1MUCTPI::L1Muctpi/L1Muctpi',
##     'LVL1::TrigT1BCM/LVL1::TrigT1BCM',
##     'LVL1::TrigT1TRT/LVL1::TrigT1TRT',
##     'LVL1::TrigT1ZDC/LVL1::TrigT1ZDC',
##     'LVL1CTP::CTPSimulation/CTPSimulation',
##     'ROIB::RoIBuilder/RoIBuilder',
##     'JetGlobalEventSetup/JetGlobalEventSetup',
##     'JobInfo/JobInfo',
##     'CreateLumiBlockCollectionFromFile/CreateLumiBlockCollectionFromFile']


#-------------------------------------------------------------------------------
# Authors:  Gorm Galster <gorm.galster@cern.ch>
#           Joerg Stelzer <stelzer@cern.ch>
# Date:     ~ December, 2012
# About:    A job options file to convert a RDO from digitization to a
#           byte stream file. Apparently this seem to require the running of
#           L1 trigger. The L1 result is stripped (EventHeader info is NOT
#           stripped at this point)
#-------------------------------------------------------------------------------


try:
    include("PATJobTransforms/CommonSkeletonJobOptions.py")
except NameError as e:
    if not str(e) == "name 'runArgs' is not defined": raise e

#-------------------------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = "atlas"
globalflags.DataSource = "geant4"
#-------------------------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc

# Disable AOD and ESD
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)

# Enable BS writing
rec.doWriteBS.set_Value_and_Lock(True)
rec.doWriteBS   =   False
rec.doWriteRDO  =   False
rec.doWriteESD  =   False
rec.doWriteAOD  =   False
rec.doWriteTAG  =   False
rec.doWriteTAGCOM = False

# Define input
rec.readRDO.set_Value_and_Lock(True)
rec.readESD = False
rec.readAOD = False
rec.readTAG = False
globalflags.InputFormat.set_Value_and_Lock('pool')

# Do trigger
rec.doTrigger.set_Value_and_Lock(True)
recAlgs.doTrigger.set_Value_and_Lock(True)

# Config trigger
TriggerFlags.doLVL1.set_Value_and_Lock(True)   # this should be disabled in the future
TriggerFlags.doHLT.set_Value_and_Lock(False)
#-------------------------------------------------------------------------------
TriggerFlags.configurationSourceList.set_Value_and_Lock(["xml"])

# Forget truth
rec.doTruth.set_Value_and_Lock(False)


# Follow the "normal" reco
include("RecExCommon/RecExCommon_topOptions.py")

theApp.EvtMax = runArgs.maxEvents
#theApp.SkipEvents = 7

from TrigSimTransforms.rdo2bs_rodversion_setup import rod_version_setup

rod_version_setup("17.1.3.4")


# Blacklist the following modules, they are not wanted at this point
_blacklist =  []
#_blacklist += [ "CTP_RDO#*", "MuCTPI_RDO#*", "HLT::HLTResult#*", "ROIB::RoIBResult#*", ]
#_blacklist += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
#_blacklist += [ "LVL1::JEPBSCollection#JEPBSCollection" ] #
#_blacklist += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
#_blacklist += [ "LVL1::CPBSCollection#CPBSCollection" ] #
#_blacklist += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]

## _blacklist  = [ "CTP_RDO#*" ]
## _blacklist += [ "DataVector<LVL1::CMMCPHits>#*" ]
## _blacklist += [ "DataVector<LVL1::CMMEtSums>#*" ]
## _blacklist += [ "DataVector<LVL1::CMMJetHits>#*" ]
## _blacklist += [ "DataVector<LVL1::CPMHits>#*" ]
## _blacklist += [ "DataVector<LVL1::CPMRoI>#*" ]
## _blacklist += [ "DataVector<LVL1::CPMTower>#*" ]
## _blacklist += [ "DataVector<LVL1::JEMEtSums>#*" ]
## _blacklist += [ "DataVector<LVL1::JEMHits>#*" ]
## _blacklist += [ "DataVector<LVL1::JEMRoI>#*" ]
## _blacklist += [ "DataVector<LVL1::JetElement>#*" ]
## _blacklist += [ "HLT::HLTResult#*" ]
## _blacklist += [ "LVL1::CMMRoI#*" ]
## _blacklist += [ "LVL1::CPBSCollection#*" ]
## _blacklist += [ "LVL1::JEPBSCollection#*" ]
## _blacklist += [ "LVL1::JEPRoIBSCollection#*" ]
## _blacklist += [ "MuCTPI_RDO#*" ]
## _blacklist += [ "ROIB::RoIBResult#*" ]

StreamBSFileOutput.ItemList = [f for f in StreamBSFileOutput.ItemList if f not in _blacklist]
# + [ "TileTTL1Container#TileTTL1Cnt", "LArTTL1Container#LArTTL1EM", "LArTTL1Container#LArTTL1HAD" ]

StreamBSFileOutput.ItemList += [ "TileRawChannelContainer#*" ]
# StreamBSFileOutput.ItemList += [ "TileTTL1Container#*" ]
# StreamBSFileOutput.ItemList += [ "LVL1::MbtsCTP#*" ]
del _blacklist

print "JOERG These collections are in StreamBSFileOutput"
for coll in StreamBSFileOutput.ItemList:
    print "     ",coll



from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc

svcMgr.ByteStreamCnvSvc

# Gimme some good ol debug
svcMgr.StoreGateSvc.Dump = True


print "All Services:"
for svc in svcMgr.getChildren():
    print "       ",svc

from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 1
#theApp.SkipEvents = 2

svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG


svcMgr.ByteStreamEventStorageOutputSvc.OutputLevel = DEBUG

ToolSvc.LArRawDataContByteStreamTool.OutputLevel = VERBOSE
ToolSvc.LArRawDataContByteStreamTool.DSPRunMode = 4
ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion = 10 

MessageSvc.debugLimit = 100000
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

print "Algorithms"
import AthenaCommon.Configurable as Configurable
Configurable.log.setLevel( INFO )
print svcMgr
print topSequence

