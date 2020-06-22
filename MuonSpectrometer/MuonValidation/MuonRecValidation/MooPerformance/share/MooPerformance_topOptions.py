# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

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
DetFlags.dcs.Calo_setOn()
DetFlags.detdescr.Calo_setOn()
DetFlags.geometry.Calo_setOn()
DetFlags.makeRIO.Calo_setOn()
DetFlags.readRDOPool.Calo_setOn()
# LVL1
DetFlags.LVL1_setOff()
DetFlags.dcs.LVL1_setOn()
DetFlags.detdescr.LVL1_setOn()
DetFlags.geometry.LVL1_setOn()
DetFlags.makeRIO.LVL1_setOn()
DetFlags.readRDOPool.LVL1_setOn()
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
recFlags.doAOD                                     = False
recFlags.doAODCaloCells                            = False
recFlags.doDPD                                     = False
recFlags.doForwardDet                              = False
recFlags.doJetMissingETTag                         = False
recFlags.doEgamma                                  = False
recFlags.doTau                                     = False
recFlags.doTrigger                                 = False
recFlags.oldFlagTopSteering                        = False
recFlags.oldRecExCommissionConfig                  = False
recFlags.doWriteCalibHits                          = False
recFlags.doWriteESD                                = False
recFlags.doWriteAOD                                = False
recFlags.doWriteTAG                                = False
recFlags.doFileMetaData                            = False

#--------------------------------------------------------------------------------
# muon reco flags (non-default values)
#--------------------------------------------------------------------------------

import MagFieldServices.SetupField
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doCalibNtuple                         = False
muonRecFlags.doMSVertex                            = False
muonRecFlags.doTrackPerformance                    = False

print ('INFO: MooPerformance_topOptions.py - muonRecFlags:')
print (muonRecFlags)

#--------------------------------------------------------------------------------
# combined muon reco flags (non-default values)
#--------------------------------------------------------------------------------

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doMuGirlLowBetaMuonCollection = False
muonCombinedRecFlags.printConfigurables            = True
muonCombinedRecFlags.doTrackPerformance            = False
print ('INFO: MooPerformance_topOptions.py - muonCombinedRecFlags:')
print (muonCombinedRecFlags)

# eliminate annoying messages from StoreGateSvc (clogging up log file)
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.setError +=  [ "StoreGateSvc"]

##### no more flags after this line #####
try:
    include ("RecExCommon/RecExCommon_topOptions.py")
    
    ###### put any user finetuning after this line #####

    #setup TTree registration Service
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output = ["HIST DATAFILE='" + "ntuple.root" + "' OPT='RECREATE'"]

    ## configure MuidStatistics for new unified chain (but no longer in release)
    from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys

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
    MuonTrackStatistics.TrackLocationList =  ["CombinedMuonTracks",
                                              "ExtrapolatedMuonTracks"]
    topSequence += MuonTrackStatistics

    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
    topSequence += MuonTrackPerformanceAlg("MuidCombinedPerformanceAlg",
                                           TrackInputLocation = "CombinedMuonTracks",
                                           IsCombined   = True )
    
    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
    topSequence += MuonTrackPerformanceAlg("MuidExtrapolatedPerformanceAlg",
                                           TrackInputLocation = "ExtrapolatedMuonTracks",
                                           IsCombined   = True )

    print ('INFO: MooPerformance_topOptions.py - MuonTrackStatistics: %s'%MuonTrackStatistics)
    print ('INFO: MooPerformance_topOptions.py - MuonTrackStatisticsTool: %s'%ToolSvc.MuonTrackStatisticsTool)
    print ('INFO: MooPerformance_topOptions.py - finished')

    ###### put any user finetuning before this line #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print ('WARNING: MooPerformance_topOptions.py - stack trace:')
    print (traceback.format_exc().rstrip())

    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    import sys
    sys.exit(10)

ServiceMgr.GeoModelSvc.SupportedGeometry=20
