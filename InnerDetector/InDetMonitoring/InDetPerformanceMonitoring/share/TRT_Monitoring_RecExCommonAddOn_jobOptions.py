from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

TRTElectronMonManager = AthenaMonManager(name="TRTElectronMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                   DataType            = DQMonFlags.monManDataType(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                   Run                 = DQMonFlags.monManRun(),
                                   LumiBlock           = DQMonFlags.monManLumiBlock())
topSequence += TRTElectronMonManager

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import TRT_Electron_Monitoring_Tool
TRT_Electron_Monitoring_Tool = TRT_Electron_Monitoring_Tool (
							     name                         = "TRT_Electron_Monitoring_Tool",
							     #trackName          	  = InDetKeys.TRTTracks() if InDetFlags.doTrackSegmentsTRT() else InDetKeys.UnslimmedTracks(),
#							     trackName          	  = "CombinedInDetTracks",
#							     trackName          	  = "CombinedTrackParticle",
							     trackName          	  = "InDetTrackParticles",
							     electronName		  = "Electrons",
							     muonName			  = "Muons",
							     conversionName		  = "ConversionCandidate",
							     doShift                      = True,
							     doTracksMon                  = True,
							     doElectronMon                = True,
							     doMuonMon                    = True,
							     doRecElectrons		  = True,
							     doConversions		  = True,
							     NPhiBins                     = 100,
							     NEtaBins                     = 100,
							     NZRBins                      = 100,
							     ZRMax                        = 1.0,
							     isEMFlag                     = "Tight",  #Must be "Loose","Medium", or "Tight" (no TRT selection in any)
							     NMinTRTHits		  = 250, # min number of hits per gamma bin
							     useTRTOnly			  = False,
							     pionTRTHitCut		  = 20,
							     #isOnline			  = athenaCommonFlags.isOnline(),
							     #doExpert                     = athenaCommonFlags.isOnline())
							     isOnline			  = True,
							     doExpert                     = True)

ToolSvc += TRT_Electron_Monitoring_Tool

TRTElectronMonManager.AthenaMonTools += [ TRT_Electron_Monitoring_Tool]

print TRTElectronMonManager
