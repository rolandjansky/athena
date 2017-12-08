from RecExConfig.RecFlags import rec as rec
from RecExConfig.RecAlgsFlags import recAlgs as recAlgs
from MuonRecExample.MuonRecFlags import muonRecFlags,muonStandaloneFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError = False

# configure all flags to run all Subset + Muon Combined
import MuonCombinedRecExample.MuonCombinedRecOnlySetup
#import MuonRecExample.MuonRecStandaloneOnlySetup

#--------------------------------------------------------------------------------
# Input setup
#--------------------------------------------------------------------------------
### choose one of the pre-defined data files from DataFiles.py
fileListZ = [
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0001.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0002.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0003.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0004.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0005.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0006.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0007.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0008.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0009.rdo.pool.root",
"root://eosatlas.cern.ch//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0010.rdo.pool.root"]

fileListT1 = [ "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000001.pool.root.1",
                                 "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000002.pool.root.1",
                                 "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000003.pool.root.1",
                                 "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000004.pool.root.1" ]

fileListMu10 = [ "root://eosatlas.cern.ch//eos/atlas/user/n/nrbern/mc12_8TeV.107211.ParticleGenerator_mu_Pt10.recon.RDO.e1717/Pt10.rdo.pool.root"]

athenaCommonFlags.FilesInput = fileListZ
#athenaCommonFlags.FilesInput = fileListT1
#athenaCommonFlags.FilesInput = fileListMu10
useIBLRDO = False
athenaCommonFlags.FilesInput = fileListT1
if useIBLRDO:
    athenaCommonFlags.FilesInput = ['/afs/cern.ch/user/n/neldik/work/RDO.19.1.1.pool.root']
    from IOVDbSvc.CondDB import conddb
    conddb.addOverride("/GLOBAL/TrackingGeo/LayerMaterialV2","AtlasLayerMat_x16_ATLAS-IBL-03")




#--------------------------------------------------------------------------------
# Reco setup
#--------------------------------------------------------------------------------
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doConversions           = False
#InDetFlags.doVertexFinding         = False
#InDetFlags.doSecVertexFinder       = False
#InDetFlags.doTRTPhaseCalculation   = False
#InDetFlags.doCaloSeededBrem = False

rec.doTrigger = False
#rec.doFloatingPointException.set_Value_and_Lock(True)
#rec.doAOD.set_Value_and_Lock(True)
rec.doWritexAOD.set_Value_and_Lock(True)
rec.doTruth.set_Value_and_Lock(True)
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.JetTag = False
AODFlags.ParticleJet = False
AODFlags.FastSimulation = False
AODFlags.Streaming = False
AODFlags.MuonTrackSlimmer = False

#muonRecFlags.doNSWNewThirdChain = True
#muonCombinedRecFlags.doCombinedFit = True
muonCombinedRecFlags.doStatisticalCombination = True
muonCombinedRecFlags.doMuGirl = True
muonCombinedRecFlags.doMuonSegmentTagger = True
#muonCombinedRecFlags.doxAOD = True
muonCombinedRecFlags.doAOD = False
muonCombinedRecFlags.doCaloTrkMuId = True

muonRecFlags.doCreateClusters = False 
muonRecFlags.prdToxAOD = False

muonRecFlags.doTGCClusterSegmentFinding = False
muonRecFlags.doRPCClusterSegmentFinding = False

muonStandaloneFlags.printSummary = True
muonRecFlags.doTrackPerformance = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel = 5


muonCombinedRecFlags.printSummary = True
muonCombinedRecFlags.doTrackPerformance = True
muonCombinedRecFlags.TrackPerfSummaryLevel = 2
muonCombinedRecFlags.TrackPerfDebugLevel = 5

#--------------------------------------------------------------------------------
# General setup
#--------------------------------------------------------------------------------

#recFlags.doNameAuditor = True
#muonRecFlags.doVP1 = True    # Decide whether to run Virtual Point1 graphical event display
athenaCommonFlags.EvtMax = 100
athenaCommonFlags.SkipEvents = 0
#EventList = "missed.txt"

#--------------------------------------------------------------------------------
# Output setup
#--------------------------------------------------------------------------------
#recFlags.doTruth=False
rec.doWriteESD = False
rec.doWriteAOD = False


##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####
#    topSequence.MergeMuonCollections.OutputLevel = DEBUG
#    topSequence.MakeAODMuons.OutputLevel = DEBUG
    if muonCombinedRecFlags.doxAOD():
        #ToolSvc.MuonCreatorTool.OutputLevel = VERBOSE
        #ToolSvc.MuonCombinedFitTagTool.OutputLevel = VERBOSE
        #ToolSvc.MuonSegmentTagTool.OutputLevel = DEBUG
        #ToolSvc.MuonCandidateTool.OutputLevel = VERBOSE
        #ToolSvc.InDetCandidateTool.OutputLevel = VERBOSE
        #ToolSvc.MuonCombinedParticleCreator.OutputLevel = VERBOSE
        #topSequence.ExtrapolatedMuonTracksTruthAlg.OutputLevel = VERBOSE
        #topSequence.CombinedMuonTracksTruthAlg.OutputLevel = VERBOSE
        #ToolSvc += CfgMgr.Trk__TrackParticleCaloExtensionTool("TrackParticleCaloExtensionTool",OutputLevel = VERBOSE)
        #ToolSvc.MuonCombinedTrackFitter.OutputLevel = VERBOSE
        #ToolSvc.OutwardsTrackCleaner.OutputLevel = VERBOSE
        #ToolSvc.MuidTrackCleaner.OutputLevel = VERBOSE
        #ToolSvc += CfgMgr.Rec__MuidCaloMaterialParam("MuidCaloMaterialParam",OutputLevel = VERBOSE)
        #ToolSvc += CfgMgr.Rec__MuidCaloTrackStateOnSurface("MuidCaloTrackStateOnSurface",OutputLevel = VERBOSE)
        #ToolSvc.CombinedMuonTrackBuilder.OutputLevel=VERBOSE
        #ToolSvc.iPatFitter.OutputLevel=DEBUG
        #ToolSvc.TrackDepositInCaloTool.OutputLevel = VERBOSE
        #ToolSvc +=  CfgMgr.xAOD__TrackIsolationTool("TrackIsolationTool",OutputLevel=DEBUG);
        #ToolSvc +=  CfgMgr.xAOD__CaloIsolationTool("CaloIsolationTool",OutputLevel=DEBUG);
        #ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.OutputLevel = VERBOSE
        pass
    #ToolSvc.MuonTrackSteering.OutputLevel = VERBOSE
    #ToolSvc.MuonTrackSteering.OnlyMdtSeeding = True
    #ToolSvc.MuonPatternSegmentMaker.OutputLevel = VERBOSE
    #ToolSvc.DCMathSegmentMaker.OutputLevel = DEBUG
    #ToolSvc.DCMathSegmentMaker.CurvedErrorScaling = False
    #ToolSvc += CfgMgr.Muon__MuonSegmentInOverlapResolvingTool("MuonSegmentInOverlapResolvingTool",OutputLevel = VERBOSE)
    #ToolSvc.MuonTrackCleaner.OutputLevel = VERBOSE

    #ToolSvc.MuonExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MCTBExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MuonNavigator.OutputLevel = VERBOSE
    #ToolSvc.AtlasNavigator.OutputLevel = VERBOSE
    #ToolSvc.AtlasExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MCTBFitter.OutputLevel = VERBOSE
    #ToolSvc.TrackingVolumeArrayCreator.OutputLevel = VERBOSE
    #ToolSvc.TrackingVolumeHelper.OutputLevel = VERBOSE
    #ToolSvc.MuonStationBuilder.OutputLevel = VERBOSE
    #ToolSvc.MuonInertMaterialBuilder.OutputLevel = VERBOSE
    #ToolSvc.MCTBSLFitter.OutputLevel = VERBOSE
    #ToolSvc.MooMuonTrackBuilder.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MCTBSLFitter.OutputLevel = VERBOSE
    #ToolSvc.AtlasSTEP_Propagator.OutputLevel=VERBOSE
    #ToolSvc.MdtMathSegmentFinder.FinderDebugLevel = 10
    #ToolSvc.MuSt_MooTrackFitter.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MooSLTrackFitter.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MooCandidateMatchingTool.OutputLevel = VERBOSE
    #ToolSvc.MooMuonTrackBuilder.OutputLevel = VERBOSE    #topSequence.MuonSegmentCnvAlg.OutputLevel = VERBOSE
    #topSequence.TrackParticleTruthAlg.OutputLevel = VERBOSE
    #from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonTruthDecorationAlg
    #from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
    #ToolSvc += Muon__MuonIdHelperTool("MuonIdHelperTool",OutputLevel=VERBOSE)
    #ToolSvc.MdtDriftCircleOnTrackCreator.OutputLevel=DEBUG
    #ToolSvc.MuonLayerHoughTool.OutputLevel = VERBOSE
    #ToolSvc.MuonLayerHoughTool.DebugHough = True
    #ToolSvc.MuonLayerHoughTool.DoNtuple = True
    #ToolSvc.MuonLayerHoughTool.UseSeeds = False
    colname = "Muons"
    #from MuonTrackPerformance.MuonTrackPerformanceConf import MuonSegmentPerformanceAlg
    #topSequence += MuonSegmentPerformanceAlg(name="MuonSegmentPerformanceAlg",OutputLevel=VERBOSE)
    #topSequence.MuonSegmentPerformanceAlg.OutputLevel=VERBOSE
    #topSequence.MuonTruthDecorationAlg.OutputLevel=VERBOSE
    if rec.doTruth() and muonRecFlags.doTrackPerformance():
        topSequence.MuonStandalonePerformanceAlg.ProduceEventListMissedTracks = 2 # "0: off, 1: two station, 2: + one station" );
        if( muonCombinedRecFlags.doxAOD() ):
            topSequence.ExtrapolatedMuonPerformanceAlg.ProduceEventListMissedTracks = 2
            topSequence.CombinedMuonPerformanceAlg.ProduceEventListMissedTracks = 2
    
    #include("MuonTestEDM/MuonTestEDM_jobOptions.py")

    doMon = False
    if( doMon ):
        from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
        tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool()
        tool1.EnableLumi = False
        tool1.DetailLevel = 1
        tool1.OutputLevel = VERBOSE
        tool1.MuonContainerName = colname
        
        ToolSvc += tool1

        from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
        topSequence += AthenaMonManager( "PhysValMonManager" )

        from AthenaCommon.AppMgr import ServiceMgr
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
        svcMgr.THistSvc.Output += ["MuonMonExample DATAFILE='MuonMonExample.root' OPT='RECREATE'"]
        
        monMan = topSequence.PhysValMonManager
        monMan.ManualDataTypeSetup = True
        monMan.DataType            = "monteCarlo"
        monMan.Environment         = "altprod"
        monMan.ManualRunLBSetup    = True
        monMan.Run                 = 1
        monMan.LumiBlock           = 1
        
        monMan.FileKey = "MuonMonExample"
        monMan.AthenaMonTools += [ tool1 ]



    ###### put any user finetuning before this line #####
except:
    print 'got an exception' 
    pass

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
from MuonRecExample.MuonRecUtils import dumpDetFlags
dumpDetFlags("config.txt")
