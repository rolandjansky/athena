#
# steering file for testing muon trigger slice with offline supertools
#
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.CfgGetter import getAlgorithm
from AthenaCommon.Logging import logging

from TriggerJobOpts.TriggerFlags import TriggerFlags

from PerfMonComps import PerfMonFlags

from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

include("TriggerTest/TrigScanFiles.py")


if 'doOldChains' not in dir():
    doOldChains=True

if 'doNewChains' not in dir():
    doNewChains=True

if 'doIsolation' not in dir():
    doIsolation=True

if 'doOffline' not in dir():
    doOffline=True

if 'makeESD' not in dir():
    makeESD=True

if 'makeAOD' not in dir():
    makeAOD=False

if 'doMuonCombined' not in dir():
    doMuonCombined=True

if 'doTrackPerformance' not in dir():
    doTrackPerformance=False

# if 'doMuonSA' not in dir():
#     doMuonSA=False

# if 'doMuonSA_only' not in dir():
#     doMuonSA_only=False

if 'v4Menus' not in dir():
    v4Menus=True

if 'v4MenuValidation' not in dir():
    v4MenuValidation = False

if 'EvtMax' not in dir():
    athenaCommonFlags.EvtMax = 1
else:
    athenaCommonFlags.EvtMax = EvtMax
    del EvtMax # no longer needed and gives WARNING or ERROR from RecExCommon

if 'Input' not in dir():
    Input='T1'


# a few specific OUTPUT levels
if athenaCommonFlags.EvtMax() <= 100:
    ConfigOutputLevel = VERBOSE
    L2OutputLvl = INFO
    EFOutputLvl = VERBOSE
    TMEFOutputLvl = VERBOSE
    MGOutputLvl = VERBOSE
elif athenaCommonFlags.EvtMax() <= 1000:
    ConfigOutputLevel = VERBOSE
    L2OutputLvl = INFO
    EFOutputLvl = VERBOSE
    TMEFOutputLvl = DEBUG
    MGOutputLvl = DEBUG
else:
    ConfigOutputLevel = INFO
    L2OutputLvl = INFO
    EFOutputLvl = INFO
    TMEFOutputLvl = INFO
    MGOutputLvl = INFO


#athenaCommonFlags.FilesInput = getEOSDirectoryList( "/eos/atlas/atlasgroupdisk/trig-daq/validation/test_data/EnhancedBias10TeV15315m31.digit.RDO.e357_s462_d148_t93a97/", [1,20] )

#athenaCommonFlags.FilesInput = [ "root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data11_7TeV.00191628.physics_eb_zee_zmumu._0001.data" ]

if Input=='Zmumu':
    athenaCommonFlags.FilesInput = getEOSDirectoryList( "/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.106047.PythiaZmumu_no_filter.digit.RDO.e574_s1021_d423_tid211907_00", [1,20] )
elif Input == "T1":
    athenaCommonFlags.FilesInput = getEOSDirectoryList( "/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366_tid197091_00", [1,20] )
elif Input != "user":
    raise RuntimeError("Unknown input data: %r" % Input)
else:
    # assumes user has set athenaCommonFlags.FilesInput
    pass

# do trigger
rec.doTrigger.set_Value_and_Lock(True)
TriggerFlags.doLVL1.set_Value_and_Lock(True)
TriggerFlags.doLVL2.set_Value_and_Lock(True)
TriggerFlags.doEF.set_Value_and_Lock(True)
rec.doESD      = True

# detector flags
rec.doMuon          = True
rec.doInDet         = True
rec.doCalo          = True
rec.doLArg          = True
rec.doTile          = True
rec.doLucid         = True

rec.doNameAuditor = True

muonRecFlags.doMuonboy = True
muonRecFlags.doStandalone = False
muonRecFlags.doMoore = True

muonRecFlags.Enabled = doOffline

muonCombinedRecFlags.doMuidMuonCollection = True
muonCombinedRecFlags.doStacoMuonCollection = False
muonCombinedRecFlags.doMuonCollection = False
muonCombinedRecFlags.doCaloMuonCollection = False

muonCombinedRecFlags.Enabled = doOffline

# muon flags
# from MuonRecExample.MuonRecFlags import muonRecFlags
# muonRecFlags.doStandalone = True

#from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
#muonStandaloneFlags.trackBuilder = 'Moore'

#from TrigMuonSuperEF.TrigMuonFlags import trigMuonFlags
#trigMuonFlags.doStandalone = True
#trigMuonFlags.trackBuilder = 'Moore'


# input/output
rec.readESD         = False
rec.doWriteESD      = makeESD 
rec.doCBNT          = False
rec.doWriteAOD      = makeAOD
rec.doWriteTAG      = False
rec.doWriteBS       = False

if not doOffline: doMuonCombined = False

# don't do unnecessary stuff
rec.doMuonCombined  = doMuonCombined
rec.doAOD           = makeAOD
rec.doAODall        = False
rec.doJiveXML       = False

# physics flags
rec.doHeavyIon        = False
rec.doEgamma          = False
rec.doTau             = False
rec.doJetMissingETTag = False

# monitoring/auditing
rec.doPerfMon             = False
rec.doDetailedPerfMon     = False
rec.doSemiDetailedPerfMon = False
rec.doMonitoring          = False
rec.doFastPhysMonitoring  = False
rec.doDetailedAuditor     = False
rec.doSGAuditor           = False

# algorithms
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger           = True
recAlgs.doTrackRecordFilter = True # filter truth muons getting into the spectrometer
recAlgs.doMissingET         = False
recAlgs.doMissingETSig      = False
recAlgs.doObjMissingET      = False
recAlgs.doEFlow             = False
recAlgs.doEFlowJet          = False
recAlgs.doAtlfast           = False
recAlgs.doMuonSpShower      = False
recAlgs.doTileMuID          = False
recAlgs.doMuTag             = False
recAlgs.doCaloTrkMuId       = False
recAlgs.doEgammaBremReco    = False
recAlgs.doAtlfast           = False

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled=doMuonCombined

from InDetRecExample.InDetJobProperties import InDetFlags
jobproperties.InDetJobProperties.disableInDetReco=not doMuonCombined

from MuonRecExample.MuonRecFlags import muonRecFlags
jobproperties.MuonRec.Enabled=doMuonCombined

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags 
jobproperties.MuonCombinedRec.Enabled=doMuonCombined

from MuonCombinedRecExample.MuidFlags import muidFlags 
jobproperties.Muid.Enabled=doMuonCombined

from egammaRec.egammaRecFlags import jobproperties
jobproperties.egammaRecFlags.Enabled=False

from JetRec.JetRecFlags import jobproperties
jobproperties.JetRecFlags.Enabled=False
   
from tauRec.tauRecFlags import jobproperties
jobproperties.tauRecFlags.Enabled=False
 
# disable JetRec (no RecFlags flag for that)
try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.Enabled = False
except ImportError:
    pass

# bug in RecExCond does not pass rec.doTau()
try:
    from tauRec.tauRecFlags import jobproperties
    jobproperties.tauRecFlags.Enabled = rec.doTau()
except ImportError:
    pass


if ConfigOutputLevel <= DEBUG:
    from AthenaCommon import CfgGetter
    CfgGetter.setLogLevel(ConfigOutputLevel)


def MuonOnly():
    global TriggerFlags,doOldChains,doNewChains,doIsolation,v4Menus
    from TriggerMenuPython.StreamingSliceFlags  import StreamingSliceFlags # not included in TriggerFlags
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MuonSlice.setAll()

    from AthenaCommon.JobProperties import JobPropertyContainer
    for prop in TriggerFlags.__dict__.values():
        if isinstance( prop, JobPropertyContainer ) and prop.__name__ != 'MuonSlice':
            try:
                prop.signatures = []
            except AttributeError:
                pass


    Muons = []

    if v4Menus:
        if v4MenuValidation:
            # only use a matching sub-set, so the monitoring histograms can be used for validation
            #     if not doMuonSA_only:
            #                 Muons += [ 'mu24_tight', 'mu24_tight_MuonEF', 'mu24_tight_MG' ]
            #                 if doIsolation:
            #                     Muons += [ 'mu24i_tight', 'mu24i_tight_MuonEF', 'mu24i_tight_MG']
            #             if doMuonSA and doIsolation:
            #                 Muons += [ 'mu24i_tight_newMuonSA' ]
            
            #Muons += [ 'mu24_tight', 'mu24_tight_MuonEF', 'mu24_tight_MG' ]
            if doIsolation:
                Muons += [ 'mu24i_tight' ]#, 'mu24i_tight_MuonEF', 'mu24i_tight_MG']
            
            TriggerFlags.MuonSlice.signatures = Muons
        else: # full v4 Menu for Muon
            if not doOldChains:
                Muons = [ m for m in TriggerFlags.MuonSlice.signatures() if not m.endswith('_MuonEF') and not m.endswith('_MG') ]
                TriggerFlags.MuonSlice.signatures = Muons
            pass

        return


    # v3 menus
    if doNewChains:
        # TrigMuSuperEF chains
        Muons += [

        # 'mu18_OR_medium', 

        # special versions for performance comparison
        'mu18_OR_medium_MGfirst',
        'mu18_OR_medium_TMEFfirst',
        'mu18_OR_medium_both',

        # special versions for validation
        'mu18_OR_medium_MGonly',
        'mu18_OR_medium_TMEFonly',
        ]

        if doIsolation:
            Muons += [

                # isolation chains
                # 'mu18i1_OR_medium',
                # 'mu18i2_OR_medium',
                # 'mu18i3_OR_medium',
                # 'mu18i4_OR_medium',
                # 'mu24i1_OR_tight',
                # 'mu24i2_OR_tight',
                # 'mu24i3_OR_tight',
                # 'mu24i4_OR_tight',
                # 'mu24i5_OR_tight',

                # special versions for performance comparison
                'mu24i5_OR_tight_MGfirst',
                'mu24i5_OR_tight_TMEFfirst',
                'mu24i5_OR_tight_both',

                # special versions for validation
                # 'mu24i1_OR_tight_TMEFonly',
                # 'mu24i2_OR_tight_TMEFonly',
                # 'mu24i3_OR_tight_TMEFonly',
                # 'mu24i4_OR_tight_TMEFonly',
                'mu24i5_OR_tight_TMEFonly',
                'mu24i5_OR_tight_MGonly',

                ]

    if doOldChains:
        # MuGirl chains
        Muons += [
            'mu18_MG_medium', 
            ]

        # TrigMuonEF chains
        Muons += [
            # non-isolation chains
            'mu18_medium', 
            ]

        if doIsolation:
            Muons += [
                # isolation chains
                'mu24i5_tight',
                ]


    TriggerFlags.MuonSlice.signatures = Muons




# to get fast error feedback
MuonOnly()


try:
    #-----------------------------------------------------------
    include("RecExCond/RecExCommon_flags.py")
    #-----------------------------------------------------------


    # from testMuonAthenaTrigRDO.py
    # set up trigger monitoring
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

    # set trigger menu
    TriggerFlags.readHLTconfigFromXML=False
    TriggerFlags.readLVL1configFromXML=False
    if v4Menus:
        TriggerFlags.triggerMenuSetup = 'MC_pp_v4'
    else:
        TriggerFlags.triggerMenuSetup = 'MC_pp_v3'
    TriggerFlags.L1PrescaleSet = 'None' 
    TriggerFlags.HLTPrescaleSet = 'None' 
    TriggerFlags.useL1CaloCalibration = True

    logging.getLogger("TriggerMenuPython.MuonDef").setLevel(ConfigOutputLevel)

    # enable only Muon slice
    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(MuonOnly)

    # main jobOption
    include ("RecExCommon/RecExCommon_topOptions.py")

    include("TriggerTest/TriggerTestCommon.py")

    print "printing topSequence: "
    print topSequence

    ################################################################################
    # L2 tweeking
    ################################################################################
    for L2_alg in topSequence.TrigSteer_L2.getChildren():
        algType = L2_alg.getType()

        # special setting for some algs
        if algType == "muFast":
            L2_alg.MUlvl1INFO  = False
            L2_alg.MUtrackINFO = False
            L2_alg.MUroadsINFO = False
            L2_alg.MUdecoINFO  = False
            L2_alg.MUcontINFO  = False
            L2_alg.MUfitINFO   = False
            L2_alg.MUsagINFO   = False
            L2_alg.MUptINFO    = True
            L2_alg.TestString = L2_alg.getName()+"  REGTEST "


    ################################################################################
    # EF tweeking
    ################################################################################
    TrigSteer_EF = topSequence.TrigSteer_EF

    #
    # here we add the algorithm used for debugging TrigMuSuperEF
    #
    for EF_alg in TrigSteer_EF.getChildren():
        algType = EF_alg.getType()

        TMEF_SegmentFinder = None
        TMEF_TrackBuilder = None
        TMEF_Extrapolator = None
        TMEF_Combiner = None
        TMEF_CombinerHypo = None
        TMEF_Isolation = None
        TMEF_IsolationHypo = None
        TrigMuGirl = None
        TrigMuGirlHypo = None

        if algType == 'TrigMuSuperEF':
            TMEF_SegmentFinder = EF_alg
            TMEF_TrackBuilder  = EF_alg
        elif algType == 'TrigMuonEFSegmentFinder':
            TMEF_SegmentFinder = EF_alg
        elif algType == 'TrigMuonEFTrackBuilder':
            TMEF_TrackBuilder = EF_alg
        elif algType == 'TrigMuonEFExtrapolator':
            TMEF_Extrapolator = EF_alg
        elif algType == 'TrigMuonEFCombiner':
            TMEF_Combiner = EF_alg
        elif algType == 'TrigMuGirl':
            TrigMuGirl = EF_alg
        else:
            continue            

        # set some properties
        if TMEF_SegmentFinder is not None:
            TMEF_SegmentFinder.recordSegmentCombinations = False
            TMEF_SegmentFinder.recordPatternCombinations = False

 
            #           if TMEF_TrackBuilder is not None:
            # for debug purposes write TrackCollection in StoreGate and use MuonTrackPerformanceAlg
            #            if doTrackPerformance:
            #                TMEF_TrackBuilder.recordTrackCollection = True
    
    if doTrackPerformance:
        from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
        topSequence += MuonTrackPerformanceAlg("MuonTrackPerformanceAlg",
                                               TrackInputLocation = "TrigMuonEFTBTracks",
                                               SegmentCombitLocation = "TrigMuonEFSegments" )
        topSequence.MuonTrackPerformanceAlg.DoSummary = 1
        topSequence.MuonTrackPerformanceAlg.DoTrackDebug = 5
        

    include("TrigMuSuperEF/TrigMuSuperEF_debug_postOptions.py")


    if makeESD:
        # Remove the following outputs defined in
        # MuonSpectrometer/MuonReconstruction/MuonRecExample/share/MuonRecOutputItemList_jobOptions.py.
        # To fix bug #79056: ERROR INCORRECT Object/count: Muon PRDs
        
        while "Muon::CscPrepDataContainer#CSC_Clusters" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::CscPrepDataContainer#CSC_Clusters")
        while "Muon::CscPrepDataContainer#CSC_Clusters" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::CscPrepDataContainer#CSC_Clusters")
        while "Muon::CscPrepDataContainer#CSC_Clusters" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::CscPrepDataContainer#CSC_Clusters")
        while "Muon::CscStripPrepDataContainer#CSC_Measurements" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::CscStripPrepDataContainer#CSC_Measurements")
        while "Muon::RpcPrepDataContainer#RPC_Measurements" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::RpcPrepDataContainer#RPC_Measurements")
        while "Muon::TgcPrepDataContainer#TGC_Measurements" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcPrepDataContainer#TGC_Measurements")
        while "Muon::MdtPrepDataContainer#MDT_DriftCircles" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::MdtPrepDataContainer#MDT_DriftCircles")
        
        while "Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC")
        while "Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC")
        
        while "Muon::TgcCoinDataContainer#TrigT1CoinDataCollection" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcCoinDataContainer#TrigT1CoinDataCollection")
        while "Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC")
        while "Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC")
        
        while "Muon::RpcCoinDataContainer#RPC_triggerHits" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("Muon::RpcCoinDataContainer#RPC_triggerHits")
        while "RpcSectorLogicContainer#RPC_SECTORLOGIC" in topSequence.StreamESD.ItemList:
            topSequence.StreamESD.ItemList.remove("RpcSectorLogicContainer#RPC_SECTORLOGIC")
        
except:
    # still do a few things in case of error
    if ConfigOutputLevel <= DEBUG:
        from AthenaCommon import CfgGetter
        CfgGetter.printStats()

    from AthenaCommon.ConfigurationShelve import saveToAscii
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    saveToAscii("config.txt")
    dumpDetFlags("config.txt")

    print 80*"="
    print "TriggerFlags"
    print 80*"="
    print TriggerFlags

    raise

else:
    if ConfigOutputLevel <= DEBUG:
        from AthenaCommon import CfgGetter
        CfgGetter.printStats()

    from AthenaCommon.ConfigurationShelve import saveToAscii
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    saveToAscii("config.txt")
    dumpDetFlags("config.txt")

    print 80*"="
    print "TriggerFlags"
    print 80*"="
    print TriggerFlags
