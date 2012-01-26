
#job options for running the LVL2 cosmic chain (Muon & ID) based on
# RecExCommon but with AllAlgs set to False
#===========================================================================
doM4L1Menu = True
doM3Data = True
doM4Data = True
doLVL2=True
CosmicL1Signature='1MU3'
CosmicL1Threshold='MU1'
CosmicL1TGCSignature='1MU1'
CosmicL1TGCThreshold='MU1'

CosmicL1StandaloneThreshold=''
CosmicL1StandaloneSignature=''
CosmicStandaloneL2Prescale=1
CosmicStandaloneEFPrescale=1
TrigEGammaL2PrescaleFactor=1
TrigEGammaL2PassThrough=0

L2HitsHypoNTRTDC=False
EFHitsHypoNTRTHTDC=False
EFHitsHypoNTRTDC=False
L2HitsHypoNTRTHTDC=False

FakeLVL1=True


if not 'FakeLVL1' in dir():
    FakeLVL1 = True

if not 'doSiTrack' in dir():
    doSiTrack = False

if not 'noField' in dir():
    noField = True


TrigT1ConfigVersion = "CSC-02-900GeV"

######################################################
# configure the algorithms...


########################################################################
# now setup stuff trhough RecExCommon & Trigger Release
########################################################################
# Switch off all the RecExCommon algorithms
AllAlgs = False
# Now switch on the ones we want
doTrigger = True
doNewSteering=True

# By default write ESD, AOD and TAG simultenaously
doWriteESD=False # comment to write ESD
doWriteAOD=False # comment to write AOD
doWriteTAG=False # comment to write TAG
doAOD=False      # comment to create AOD
doESD=False      # comment to create ESD
doTAG=False      # comment to create TAG

doJiveXML = False

OutputLevel=INFO

include ("RecExCommon/RecExCommon_flags.py")
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

if doM4L1Menu:
    TriggerFlags.readLVL1configFromXML=True
    TriggerFlags.inputLVL1configFile =  'TrigL2CosmicMuonHypo/L1MenuM4.xml'

#TriggerFlags.inputHLTconfigFile  = 'hltMenu_cosmic_NewSteer.xml'
#TriggerFlags.inputLVL1configFile =  'Lvl1Config_cosmic_NewSteer.xml'

TriggerFlags.doLVL1 = False
TriggerFlags.doLVL2 = True
TriggerFlags.doEF=False
TriggerFlags.doID=True
TriggerFlags.doMuon=True
    
TriggerFlags.EgammaSlice.unsetAll()
TriggerFlags.MuonSlice.unsetAll()
TriggerFlags.BphysicsSlice.unsetAll()
TriggerFlags.EgammaSlice.setL2Calo()
TriggerFlags.CosmicSlice.setAll()
triggerMenu.disableAll()
triggerMenu.enableSignature("e10")


#theApp.Dlls += ['TGCgeometry']
#theApp.ExtSvc += ['TGCgeometrySvc']
theApp.Dlls += ['TrigL2CosmicCombined']
theApp.Dlls += ['TrigL2CosmicMuon']
theApp.Dlls   += [ "TrigmuFast" ] # needed for MUON::ROBmapper::ROBmapper(std::basic_string<char, std::char_traits<char>, std::allocator<char> >)
theApp.Dlls += ['TrigL2CosmicMuonHypo']
theApp.Dlls += [ "TrigOnlineSpacePointTool","TrigOfflineSpacePointTool" ]

theApp.Dlls += [ "TrkExTools" ]

# triggerMenu.enableSignature('cosmic_IDSCAN')
triggerMenu.enableSignature('cosmic_trtxk')
triggerMenu.enableSignature('cosmic_muonOnly')


#
# include the main jobOption
#

doTruth=True
TriggerFlags.enableMonitoring = [ "Cosmic", "Time" ]
# include("TriggerRelease/Trigger_topOptions_standalone.py")
include ("TriggerRelease/jobOfragment_forRecExCommon.py")

PoolRDOInput = ATLASCosmicFlags.InputFile
doHist = True

# set message format so there is enough field-width for the algo. name
#MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
#get rid of messages and increase speed
#Service ("StoreGateSvc" ).ActivateHistory=False

###############################################################
## set up the fake ROI

if FakeLVL1:

    from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
    fake = FakeLvl1RoIatFixedEtaPhi()
    fake.FakeMuonRoiPhi=3.1
    fake.FakeMuonRoiEta=0.1
    fake.FakeMuonRoiLabel=CosmicL1Threshold
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    job.TrigSteer_L2.LvlConverterTool = fake
    
if noField:
    Algorithm("ToolSvc.TrigInDetTrackFitter").UseAthenaFieldService=False
    Algorithm("ToolSvc.TrigInDetTrackExtrapolator").UseAthenaFieldService=False
    Algorithm("ToolSvc.TrigTRT_TrackExtensionTool.TrigMagneticFieldTool").UseAthenaFieldService=False
    Algorithm("ToolSvc.TrigMagneticFieldTool").UseAthenaFieldService=False

if ATLASCosmicFlags.doMuons:
    svcMgr.MDTcablingSvc.CABfile = 'Cabling_barrel_sector3456.v2_6.data'
    svcMgr.MDTcablingSvc.ShiftLinks=False
    svcMgr.RPCcablingSvc.CosmicConfiguration=True

