#job options for running the LVL2 cosmic chain (Muon & ID) based on
# RecExCommon but with AllAlgs set to False
#===========================================================================

if not 'FakeLVL1' in dir():
    FakeLVL1 = True

if not 'doSiTrack' in dir():
        doSiTrack = False
        
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

# set OutputLevel
OutputLevel=INFO
#VERBOSE
#INFO


DetDescrVersion='ATLAS-CSC-00-00-00'
PoolRDOInput = ['/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/cosmic.10evts.rdo.root']

#PoolRDOInput = ['/space2/jboyd/cosmic_sim/data/prod/trt_vol/dig_10.root']
#DetDescrVersion = "ATLAS-CommNF-00-00-00"

#PoolRDOInput = ['/space2/jboyd/cosmic_sim/data/prod/trt_vol_withField/dig.trtbarrel_01110000.root']
#DetDescrVersion = "ATLAS-Comm-00-00-00"


#PoolRDOInput = ['config1000b/dig.root']
#DetDescrVersion='ATLAS-DC3-06'


EvtMax=-1


include ("RecExCommon/RecExCommon_flags.py")
#DetFlags.Calo_setOff()

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

#DetFlags.ID_setOff()
#DetFlags.Muon_setOff()

TriggerFlags.doEF=False
TriggerFlags.doID=True
TriggerFlags.doMuon=False

TriggerFlags.Slices_all_setOff()

triggerMenu.disableAll()

if doSiTrack:
    triggerMenu.enableSignature("cosmic_SiTrack")
else:
    triggerMenu.enableSignature("cosmic_IDSCAN")
    
    
TriggerFlags.CosmicSlice.setL2()


#theApp.Dlls += ['TGCgeometry']
#theApp.ExtSvc += ['TGCgeometrySvc']
#theApp.Dlls += ['TrigL2CosmicCombined']
#theApp.Dlls += ['TrigL2CosmicMuon']
theApp.DLLs   += [ "TrigmuFast" ] # needed for MUON::ROBmapper::ROBmapper(std::basic_string<char, std::char_traits<char>, std::allocator<char> >)
#theApp.Dlls += ['TrigL2CosmicMuonHypo']
theApp.Dlls += [ "TrigOnlineSpacePointTool","TrigOfflineSpacePointTool" ]

theApp.Dlls += [ "TrkExTools" ]


#
# include the main jobOption
#

doCBNT=False
doTruth=False
#True
#e
TriggerFlags.enableMonitoring = [ "Cosmic", "Time" ]
include ("RecExCommon/RecExCommon_topOptions.py")

# set message format so there is enough field-width for the algo. name
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

###############################################################
## set up the fake ROI

# fake LVL1
if FakeLVL1:

    from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
#    from TrigFake.TrigFakeConf import FakeLvl1MultipleRoIsatFixedEtaPhi

#    fake = FakeLvl1MultipleRoIsatFixedEtaPhi()
#    fake.FakeMuonRoiPhi=[1.5708]#,-1.5708]
#    fake.FakeMuonRoiEta=[0.1]#,0.2]
#    fake.FakeMuonRoiLabel=["MU06"]#,"MU06"]
    fake = FakeLvl1RoIatFixedEtaPhi()
    fake.FakeMuonRoiPhi=3.1
    fake.FakeMuonRoiEta=0.1
    fake.FakeMuonRoiLabel="MU06"

    # fake.OutputLevel=VERBOSE

    job.TrigSteer_L2.LvlConverterTool =  fake

##
# set up monitoring...

numberOfThreads = 0
theApp.ExtSvc += ['THistSvc/THistSvc']
THistSvc = Service('THistSvc')
THistSvc.Output += ["EXMON DATAFILE='rectest.root' OPT='RECREATE'"]


CBNTAthenaAware=False
#True
if CBNTAthenaAware:
    THistSvc = Service ( "THistSvc" )
    if doSiTrack:
        THistSvc.Output = ["AANT DATAFILE='ntuple_sitrack.aan.root' OPT='RECREATE'"]
    else:
        THistSvc.Output = ["AANT DATAFILE='ntuple_idscan.aan.root' OPT='RECREATE'"]
    theApp.Dlls   += [ "AnalysisTools" ]
    theApp.TopAlg += [ "AANTupleStream" ]
    AANTupleStream = Algorithm( "AANTupleStream" )
    AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
    AANTupleStream.OutputName = 'AnalysisSkeleton.aan.root'
    theApp.Dlls += [ "RootHistCnv" ]
    theApp.HistogramPersistency = "ROOT"

    include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
    include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
    CBNT_Athena = Algorithm( "CBNT_AthenaAware" )
    CBNT_Athena.Members += [ "CBNTAA_CosmicIDCombined" ]
#    CBNT_AthenaAware.Members += [ "TestBeam::FillTrack/FillTrack" ]
#    FillTrack = Algorithm( "FillTrack" )
#    FillTrack.OutputLevel = VERBOSE
#    FillTrack.getMC = True
#    FillTrack.ExtrapolationTool="Trk::Extrapolator/InDetExtrapolator"

#    CBNT_Athena.Members += [ "CBNTAA_CosmicIDHalfTrackMuon"]
#    CBNT_Athena.Members += [ "CBNTAA_MdtHit" ]
#    CBNT_Athena.Members += [ "CBNTAA_TgcHit" ]
#    CBNT_Athena.Members += [ "CBNTAA_TrigCosmic" ]
#    CBNT_Athena.Members += [ "CBNTAA_CosmicIDMuon" ]

#    CBNT_Athena.Members += [ "CBNTAA_EventInfo" ]
#    CBNT_Athena.Members += [ "CBNTAA_ReadMuCTPI_RDO" ]
#    CBNT_Athena.Members += [ "CBNTAA_ReadRoIBResult" ]
#    CBNT_Athena.NtupleLocID = '/FILE1/FILE1/tree'
    include("TrigNtInDet/jobOfragment_TrigNtInDet.py")
    AANTupleStream.WriteInputDataHeader = True

#Algorithm("ToolSvc.TrigInDetTrackFitter").UseAthenaFieldService=False
#Algorithm("ToolSvc.TrigInDetTrackExtrapolator").UseAthenaFieldService=False
#Algorithm("ToolSvc.TrigTRT_TrackExtensionTool.TrigMagneticFieldTool").UseAthenaFieldService=False




print "TOPLEVEL ALGORITHM : ", theApp.TopAlg


print "List of all top algorithms"
print theApp.TopAlg
