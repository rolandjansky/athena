#######################################################################
#
# Job Options to run TrigTileMuId algorithms using the flag useRODTag:
#
# TileLookForMuAlg:
#     Tile Calorimeter scan to search low pT muons at LVL2 PU
#
# TileRODMuAlg:
#     Algorithm results from BS processed at TileCal ROD DSP
#
#######################################################################

useRODTag=True
commissioningFile=True

doAOD=False
doESD=False
doTAG=False
doTruth=False
doTrigger=True

from AthenaCommon.DetFlags import DetFlags
DetFlags.Tile_setOn()
DetFlags.LAr_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.Print()

if useRODTag:
    BSRDOInput=["rfio:/castor/cern.ch/atlas/P1commisioning/phaseIII/daq_SFI-3_m3_combined_0010825_file01.data"]
else:
    if commissioningFile:
        BSRDOInput=["rfio:/castor/cern.ch/atlas/P1commisioning/phaseIII/daq_SFI-3_m3_combined_0010825_file01.data"]
    else:
        BSRDOInput=["/afs/cern.ch/user/b/baines/releases/data/daq_Athena_DC3_0005200_CaloMuon_file01.data"]

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat.set_bytestream()

from TrigConfigSvc.TrigConfigSvcConfig import DefaultHLTConfigSvc
HLTConfigSvc = DefaultHLTConfigSvc()
HLTConfigSvc.ConfigSource = "XML"
HLTConfigSvc.OutputLevel = VERBOSE

include ("RecExCond/AllDet_detDescr.py")

EventSelector = Service('EventSelector')
EventSelector.ByteStreamInputSvc = 'ByteStreamEventStorageInputSvc'

ByteStreamEventStorageInputSvc = Service('ByteStreamEventStorageInputSvc')
ByteStreamEventStorageInputSvc.FullFileName = BSRDOInput

EventPersistency = Service('EventPersistencySvc')
EventPersistency.CnvServices += ['ByteStreamCnvSvc']
EventPersistency.CnvServices += ['ByteStreamEventStorageInputSvc']

ProxyProviderSvc = Service('ProxyProviderSvc')
ProxyProviderSvc.ProviderNames += ['ByteStreamAddressProviderSvc']

ByteStreamAddressProviderSvc = Service('ByteStreamAddressProviderSvc')

DetDescrVersion="ATLAS-CSC-01-02-00"

GlobalFlags.Print()

doCBNT=False
doCaloCluster=False
doCaloTopoCluster=False
doMuonIDStandAlone=False
doMissingET=False
AllAlgs=False
doMuonboyEDM=False
doJetRec=False
doWriteAOD=False
doWriteESD=False
readESD=False
readAOD=False
doWriteRDO=False

TriggerFlags.doEF=False
TriggerFlags.doLVL1=False
TriggerFlags.doID=False
TriggerFlags.readHLTconfigFromXML=True
TriggerFlags.TauSlice.unsetAll()
TriggerFlags.JetSlice.unsetAll()
TriggerFlags.BphysicsSlice.unsetAll()
TriggerFlags.BjetSlice.unsetAll()
TriggerFlags.METSlice.unsetAll()
TriggerFlags.EgammaSlice.unsetAll()
TriggerFlags.MuonSlice.unsetAll()
TriggerFlags.printSliceFlags()

if useRODTag:
    try:
        from TrigTileMuId.TrigTileRODMuAlgConfig import *
        theTrigTileRODMuAlg = TrigTileRODMuAlg_L2()
        print theTrigTileRODMuAlg
    except:
        print "could not load TrigTileRODMuAlgConfig"
        import traceback
        print traceback.format_exc()
    theTrigTileRODMuAlg.doTiming = True
    TriggerFlags.inputHLTconfigFile="TrigTileRODMuAlg_menu.xml"
    Algorithm("TrigTileRODMuAlg_L2").OutputLevel=VERBOSE
else:
    try:
        from TrigTileMuId.TrigTileLookForMuAlgConfig import *
        theTrigTileLookForMuAlg = TrigTileLookForMuAlg_L2()
        print theTrigTileLookForMuAlg
    except:
        print "could not load TrigTileLookForMuAlgConfig"
        import traceback
        print traceback.format_exc()
    if commissioningFile:
        theTrigTileLookForMuAlg.UseRS = True
        theTrigTileLookForMuAlg.DRAWERIDLIST=range(0,192) # all LBA, LBC, EBA
        print theTrigTileLookForMuAlg.DRAWERIDLIST
    else:
        theTrigTileLookForMuAlg.UseRS = False
    theTrigTileLookForMuAlg.doTiming = True
    TriggerFlags.inputHLTconfigFile="TrigTileLookForMuAlg_menu.xml"
    Algorithm("TrigTileLookForMuAlg_L2").OutputLevel=VERBOSE

include ("RecExCommon/RecExCommon_topOptions.py")


########################
# Trigger libraries
########################

from TrigNavigation.TrigNavigationConfig import *
Navigation = HLTNavigationOffline()

ScalerSvc = Service( "ScalerSvc" )
ScalerSvc.DefaultType = "HLT::PeriodicScaler"


########################
# L2 Steering:
########################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from TrigSteering.TrigSteeringConfig import TrigSteer_L2

hlt_menu_file_name = TriggerFlags.inputHLTconfigFile
TriggerFlags.inputLVL1configFile="LVL1Config.xml"

lv1_menu_file_name = TriggerFlags.inputLVL1configFile

TrigSteer_L2 = TrigSteer_L2('TrigSteer_L2', hltFile=hlt_menu_file_name, lvl1File=lv1_menu_file_name)
job += TrigSteer_L2
TrigSteer_L2.OutputLevel=VERBOSE
TrigSteer_L2.HLTLevel="L2"
TrigSteer_L2.doTiming=TRUE
TrigSteer_L2.LvlConverterTool="HLT::Lvl1FromFile"
TrigSteer_L2 += Algorithm( "ResultBuilder_L2" )
TrigSteer_L2.ResultBuilder_L2.doStreamTags=True
TrigSteer_L2.ResultBuilder_L2.OutputLevel=DEBUG
TrigSteer_L2 += Navigation


############################
# Generate a LVL1 Fake RoI
############################

FakeLVL1 = True
if FakeLVL1:
    from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
    fake = FakeLvl1RoIatFixedEtaPhi()
    fake.FakeMuonRoiPhi = -1.6
    fake.FakeMuonRoiEta = 0.0
    fake.FakeMuonRoiLabel = "MU0"
    fake.OutputLevel = VERBOSE
    TrigSteer_L2.LvlConverterTool = fake


############################
# TrigTimeAlgs
############################

include( "TrigTimeAlgs/jobOfragment_TrigTimerSvc.py" )
TrigTimerSvc.OutputLevel = VERBOSE


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 2000000

EvtMax = 10
theApp.EvtMax = EvtMax

print topSequence
