
#job options for running the LVL2 cosmic chain (Muon & ID) based on
# RecExCommon but with AllAlgs set to False
#===========================================================================

if not 'FakeLVL1' in dir():
    FakeLVL1 = True

if not 'doSiTrack' in dir():
    doSiTrack = False

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


DetDescrVersion='ATLAS-CSC-00-00-00'
PoolRDOInput = ['/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/cosmic.10evts.rdo.root']

EvtMax=-1
include ("RecExCommon/RecExCommon_flags.py")
DetFlags.Calo_setOff()

FakeLVL1=True
if FakeLVL1: 
    TriggerFlags.fakeLVL1 = True

DetFlags.Muon_setOff()
doMuon=False
doMuons=False
TriggerFlags.doMuon=False
TriggerFlags.doCalo=False
TriggerFlags.doEF=False

if doSiTrack:
    TriggerFlags.inputHLTsequenceFile = 'HLTsequenceL2CosmicSITRACK.xml'
else:
    TriggerFlags.inputHLTsequenceFile = 'HLTsequenceL2CosmicIDSCAN.xml'

TriggerFlags.inputHLTsignatureFile = 'HLTsignatureL2Cosmic.xml'

#theApp.Dlls += ['TGCgeometry']
#theApp.ExtSvc += ['TGCgeometrySvc']
theApp.Dlls += ['TrigL2CosmicCombined']
theApp.Dlls += ['TrigL2CosmicMuon']
theApp.DLLs   += [ "TrigmuFast" ] # needed for MUON::ROBmapper::ROBmapper(std::basic_string<char, std::char_traits<char>, std::allocator<char> >)
theApp.Dlls += ['TrigL2CosmicMuonHypo']
theApp.Dlls += [ "TrigOnlineSpacePointTool","TrigOfflineSpacePointTool" ]



#
# include the main jobOption
#

doCBNT=False
doTruth=False
include ("RecExCommon/RecExCommon_topOptions.py")

# set message format so there is enough field-width for the algo. name
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False 

###############################################################
## set up the trigger stuff...

# fake LVL1
if FakeLVL1: 
    include( "TrigFake/jobOfragment_TrigFake.py" )
    TrigSteer_L2 = Algorithm( "HLT::TrigSteer/TrigSteer_L2" )

    TrigSteer_L2.LvlConverter="FakeLvl1RoIatFixedEtaPhi"
    TrigSteer_L2 += Algorithm( "LvlConverterAlg_L2" )
    TrigSteer_L2.LvlConverterAlg_L2.FakeMuonRoiPhi=3.1
    TrigSteer_L2.LvlConverterAlg_L2.FakeMuonRoiEta=0.1
    TrigSteer_L2.LvlConverterAlg_L2.FakeMuonRoiLabel="MU06"
    TrigSteer_L2.OutputLevel=VERBOSE

 
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


print "TOPLEVEL ALGORITHM : ", theApp.TopAlg

######################################################
# configure the algorithms...

if doSiTrack:

    from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Cosmics
    sitrackCosmics = TrigSiTrack_Cosmics() # here we created an instance
    TrigSteer_L2 += sitrackCosmics
    Algorithm("TrigSiTrack_Cosmics").OutputLevel=DEBUG
    Algorithm("TrigSiTrack_Cosmics_L2").Seeding_SCTOnly = True
    # the above lines should be moved to jobOfragment_TrigSiTrack_Cosmics.py
else:
    from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
    idscanCosmics = TrigIDSCAN_Cosmics() # here we created an instance
    TrigSteer_L2 += idscanCosmics
    Algorithm("TrigIDSCAN_Cosmics").OutputLevel=DEBUG

from TrigL2CosmicCombined.TrigL2CosmicCombined_Config import CombinedIDHalfTracks_Cosmic
htcombinedCosmics = CombinedIDHalfTracks_Cosmic() # here we created an instance
TrigSteer_L2 += htcombinedCosmics


from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CombinedIDHalfTracksHypo_Cosmic
htcombinedCosmicsHypo = CombinedIDHalfTracksHypo_Cosmic() # here we created an instance
TrigSteer_L2 += htcombinedCosmicsHypo

# cosmic muon hasnt been migrated to configurables yet...
TrigL2CosmicMuon = Algorithm('TrigL2CosmicMuon_1_L2')
TrigL2CosmicMuon.OutputLevel = DEBUG
TrigL2CosmicMuon.DoFullScan = True

#Algorithm('CombinedIDHalfTracks_1_L2').OutputLevel=VERBOSE
#Algorithm('CombinedIDMuon_1_L2').OutputLevel=VERBOSE
#Algorithm('CombinedIDHalfTracksHypo_1_L2').OutputLevel=VERBOSE


########################################################
# generate configurations for L1 and EF
# make it here, it will be factored out to somehing like
# jobOfragment_nakeXMLConfig

from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('hltMenu_cosmic_NewSteer.xml',
                           'Lvl1Config_cosmic_NewSteer.xml' )
# LVL1 menu
item = LVL1MenuItem('iMU06', prescale='1', ctpid='1')
item.addAndedCondition(name='MU06', multi='1')
menu.addLVL1Item(item)

menu.addEMThreshold('MU06', value='9', range_begin='0', range_end='2')

# HLT menu
# L2 Sequences

if doSiTrack:
    menu.addSequence('MU06',[sitrackCosmics], 'sitrack')
else:
    menu.addSequence('MU06',[idscanCosmics], 'sitrack')

#menu.addSequence('sitrack',['TrigL2CosmicMuon/TrigL2CosmicMuon/1_L2'],'sitrack2')
menu.addSequence('sitrack',[htcombinedCosmics,htcombinedCosmicsHypo],'L2out')
#menu.addSequence('sitrack',[htcombinedCosmics,'CombinedIDHalfTracksHypo/CombinedIDHalfTracksHypo/1_L2'],'L2out')
#menu.addSequence('L2out2',['CombinedIDHalfTracksHypo/CombinedIDHalfTracksHypo/1_L2'],'L2out')

chain = HLTChain( chain_name="cosmic_L2", chain_counter="1", lower_chain_name="iMU06", level="L2", prescale="1", pass_through="1")
#chain.addHLTSignature( ['sitrack'] ).addHLTSignature( ['sitrack2']).addHLTSignature( 'L2out' )
#chain.addHLTSignature( ['sitrack'] ).addHLTSignature( 'L2out2' ).addHLTSignature( 'L2out' )
chain.addHLTSignature( ['sitrack'] ).addHLTSignature( 'L2out' )
chain.addTriggerTypeBit('4')
chain.addStreamTag('cosmics', prescale='1')
menu.addHLTChain(chain)  

menu.writeConfigFiles()
menu.dot(algs=True)

# make use of generated menu
#LVL1ConfigSvc.XMLFile = "Lvl1Config_cosmic_NewSteer.xml"
HLTConfigSvc.XMLMenuFile = "hltMenu_cosmic_NewSteer.xml"

print "List of all top algorithms"
print theApp.TopAlg
