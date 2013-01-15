#-------------------------------------------------------------------------------
# Author:   Gorm Galster <gorm.galster@cern.ch>
# Date:     ~ December, 2012
# About:    A job options file to convert a RDO fresh from digitization to a
#           byte stream file. Apparently this seem to require the running of
#           L1 trigger. The L1 result is stripped (EventHeader info is NOT
#           stripped at this point)
#-------------------------------------------------------------------------------
#from PyJobTransformsCore.runargs import RunArguments
#runArgs = RunArguments()
#runArgs.maxEvents = -1
#-------------------------------------------------------------------------------
try:
    include("PATJobTransforms/CommonSkeletonJobOptions.py")
except NameError as e:
    if not str(e) == "name 'runArgs' is not defined": raise e
#-------------------------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = "atlas"
globalflags.DataSource = "geant4"
#globalflags.DetDescrVersion = _geo # Ecpected to be set elsewhere
#globalflags.ConditionsTag = _con # Ecpected to be set elsewhere
#-------------------------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc

# Disable AOD and ESD
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)

# As a 0th order, don't pretend to write *anything*
rec.doWriteBS   =   False
rec.doWriteRDO  =   False
rec.doWriteESD  =   False
rec.doWriteAOD  =   False
rec.doWriteTAG  =   False
rec.doWriteTAGCOM   =   False

# As a 0th order, don't pretend to read *anything*
rec.readRDO =   False
rec.readESD =   False
rec.readAOD =   False
rec.readTAG =   False

# Do trigger, damn it.
rec.doTrigger.set_Value_and_Lock(True)
recAlgs.doTrigger.set_Value_and_Lock(True)

# Config trigger
TriggerFlags.doLVL1.set_Value_and_Lock(True)
TriggerFlags.doLVL2.set_Value_and_Lock(False)
TriggerFlags.doEF.set_Value_and_Lock(False)
#-------------------------------------------------------------------------------
#TriggerFlags.triggerConfig.set_Value_and_Lock(env["TRIGCONF"]) # Expected to be set elsewhere.
TriggerFlags.configurationSourceList.set_Value_and_Lock(["xml"])

# Forget truth
rec.doTruth.set_Value_and_Lock(False)


# Define input data
rec.readRDO.set_Value_and_Lock(True)
globalflags.InputFormat.set_Value_and_Lock('pool')
#athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([_i]) # Expected to be set elsewhere

# Define output data
rec.doWriteBS.set_Value_and_Lock(True)
#athenaCommonFlags.BSRDOOutput.set_Value_and_Lock(_o) # Expected to be set elsewhere


# Follow the "normal" reco
include("RecExCommon/RecExCommon_topOptions.py")

theApp.EvtMax = -1


# Blacklist the following modules, they are not wanted at this point
_blacklist = ["CTP_RDO#*", "MuCTPI_RDO#*", "HLT::HLTResult#*", "ROIB::RoIBResult#*", ]
_blacklist += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
_blacklist += [ "LVL1::JEPBSCollection#JEPBSCollection" ] #
_blacklist += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
_blacklist += [ "LVL1::CPBSCollection#CPBSCollection" ] #
_blacklist += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]

"""
_blacklist  = [ "CTP_RDO#*" ]
_blacklist += [ "DataVector<LVL1::CMMCPHits>#*" ]
_blacklist += [ "DataVector<LVL1::CMMEtSums>#*" ]
_blacklist += [ "DataVector<LVL1::CMMJetHits>#*" ]
_blacklist += [ "DataVector<LVL1::CPMHits>#*" ]
_blacklist += [ "DataVector<LVL1::CPMRoI>#*" ]
_blacklist += [ "DataVector<LVL1::CPMTower>#*" ]
_blacklist += [ "DataVector<LVL1::JEMEtSums>#*" ]
_blacklist += [ "DataVector<LVL1::JEMHits>#*" ]
_blacklist += [ "DataVector<LVL1::JEMRoI>#*" ]
_blacklist += [ "DataVector<LVL1::JetElement>#*" ]
_blacklist += [ "HLT::HLTResult#*" ]
_blacklist += [ "LVL1::CMMRoI#*" ]
_blacklist += [ "LVL1::CPBSCollection#*" ]
_blacklist += [ "LVL1::JEPBSCollection#*" ]
_blacklist += [ "LVL1::JEPRoIBSCollection#*" ]
_blacklist += [ "MuCTPI_RDO#*" ]
_blacklist += [ "ROIB::RoIBResult#*" ]
"""
StreamBSFileOutput.ItemList = [f for f in StreamBSFileOutput.ItemList if f not in _blacklist]

del _blacklist

# Gimme some good ol debug
svcMgr.StoreGateSvc.Dump = True

