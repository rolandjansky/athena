#--------------------------------------------------------------
# FPE check
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
# theApp.CreateSvc += ["FPEControlSvc"]


#--------------------------------------------------------------------------------
# configure required detector flags for combined reco of simulated muons 
#--------------------------------------------------------------------------------

from AthenaCommon.DetFlags  import DetFlags
#  inner detector
DetFlags.ID_setOff()
DetFlags.dcs.ID_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.geometry.ID_setOn()
DetFlags.makeRIO.ID_setOn()
DetFlags.readRDOPool.ID_setOn()
#  calorimetry
DetFlags.Calo_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.geometry.Calo_setOn()
#  muons
DetFlags.Muon_setOff()
DetFlags.dcs.Muon_setOn()
DetFlags.detdescr.Muon_setOn()
DetFlags.geometry.Muon_setOn()
DetFlags.makeRIO.Muon_setOn()
DetFlags.readRDOPool.Muon_setOn()
#  truth
DetFlags.Truth_setOn()
DetFlags.readRDOBS.Truth_setOff()
DetFlags.readRIOBS.Truth_setOff()
DetFlags.readRIOPool.Truth_setOff()
DetFlags.simulate.Truth_setOff()
DetFlags.simulateLVL1.Truth_setOff()
DetFlags.writeBS.Truth_setOff()
DetFlags.writeRDOPool.Truth_setOff()
DetFlags.writeRIOPool.Truth_setOff()
DetFlags.Truth_setOff()
DetFlags.dcs.Truth_setOn()
DetFlags.detdescr.Truth_setOn()
DetFlags.geometry.Truth_setOn()
DetFlags.makeRIO.Truth_setOn()
DetFlags.readRDOPool.Truth_setOn()

#--------------------------------------------------------------------------------
# configure rec flags to only run algs relevant for combined muons
#--------------------------------------------------------------------------------

from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
recFlags.doDPD                                     = False
recFlags.doForwardDet                              = False
recFlags.doJetMissingETTag                         = False
recFlags.doEgamma                                  = False
recFlags.doTau                                     = False
recFlags.doTrigger                                 = False
recFlags.oldRecExCommissionConfig                  = False
recFlags.doWriteCalibHits                          = False
recFlags.doWriteESD                                = False
recFlags.doWriteAOD                                = False
recFlags.doWriteTAG                                = False
recFlags.doFileMetaData                            = False

#--------------------------------------------------------------------------------
# indet reco flags (running with PileUp - need to speed up execution)
# select HighPileUp but a PtMin setting would be better
#--------------------------------------------------------------------------------

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doHighPileup.set_Value_and_Lock   (True)
InDetFlags.doForwardTracks.set_Value_and_Lock(True)

#--------------------------------------------------------------------------------
# muon reco flags
#--------------------------------------------------------------------------------

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doMuonboy                             = False  # switch off Muonboy
muonRecFlags.doStandalone                          = True   # 3rd chain 
muonRecFlags.enableErrorTuning                     = False
muonRecFlags.doTrackPerformance                    = False
muonRecFlags.doCalibNtuple                         = False

#--------------------------------------------------------------------------------
# combined muon reco flags
#--------------------------------------------------------------------------------

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doStacoMuonCollection         = False
muonCombinedRecFlags.doCaloMuonCollection          = False
muonCombinedRecFlags.doMuidMuonCollection          = False # 2nd chain
muonCombinedRecFlags.doMuonCollection              = True  # 3rd chain
muonCombinedRecFlags.doMuGirlLowBetaMuonCollection = False
muonCombinedRecFlags.doMuGirl                      = True
muonCombinedRecFlags.doMuTagIMO                    = True
muonCombinedRecFlags.doMergeMuons                  = True
muonCombinedRecFlags.doAODMuons                    = True
muonCombinedRecFlags.doMuonPhysicsDressing         = False
muonCombinedRecFlags.printConfigurables            = True
muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(False)

##### no more flags after this line #####
try:
    include ("RecExCommon/RecExCommon_topOptions.py")
    
    ###### put any user finetuning after this line #####
    ## suppress annoying WARNING
    topSequence.BeamBackgroundFiller.Enable        = False
    topSequence.BackgroundWordFiller.Enable        = False

    ## stuff for Thijs
    #MuonReFitTrack.DummyMode = False

    #setup TTree registration Service
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output = ["HIST DATAFILE='" + "ntuple.root" + "' OPT='RECREATE'"]

    ## configure MuidStatistics for new unified chain
    from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
    from MuidStatistics.MuidStatisticsConf import MuidStatistics
    MuidStatistics = MuidStatistics()
    MuidStatistics.HistSvc              = ServiceMgr.THistSvc
    MuidStatistics.CombinedMuonLocation = MuonCbKeys.FinalMuons() ## "Muons"
    MuidStatistics.OutputLevel          = DEBUG
    topSequence += MuidStatistics

    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsTool
    MuonTrackStatisticsTool = MuonTrackStatisticsTool("MuonTrackStatisticsTool")
    MuonTrackStatisticsTool.OutputLevel   = WARNING
    MuonTrackStatisticsTool.doTruth       = False
    ToolSvc += MuonTrackStatisticsTool
	
    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg
    MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
    MuonTrackStatistics.doTruth           = False
    MuonTrackStatistics.writeToFile       = True
    MuonTrackStatistics.FileName          = "trkSummary.txt"
    MuonTrackStatistics.TrackLocationList =  ["MuidCBTracksFromMuidStatistics",
                                              "MuGirlTracksFromMuidStatistics"]
    topSequence += MuonTrackStatistics

    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
    topSequence += MuonTrackPerformanceAlg("MuidCombinedPerformanceAlg",
                                           TrackInputLocation = "MuidCBTracksFromMuidStatistics",
                                           DoTruth      = True,
                                           IsCombined   = True )
    topSequence += MuonTrackPerformanceAlg("MuGirlPerformanceAlg",
                                           TrackInputLocation = "MuGirlTracksFromMuidStatistics",
                                           DoTruth      = True,
                                           IsCombined   = True )
    #topSequence += MuonTrackPerformanceAlg("MuonCombinedRefitPerformanceAlg",
    #                                       TrackInputLocation = "Combined_Tracks",
    #                                       DoTruth      = True,
    #                                       IsCombined   = True )

    print MuonTrackStatistics,ToolSvc.MuonTrackStatisticsTool
    print 'MooPerformance_topOptions: finished '
    
    ###### put any user finetuning before this line #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print traceback.format_exc().rstrip()

    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    import sys
    sys.exit(10)
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
