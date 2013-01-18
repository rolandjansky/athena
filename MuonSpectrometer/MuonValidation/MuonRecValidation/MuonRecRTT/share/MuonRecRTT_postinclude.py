#from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")

import AthenaServices.AthDsoLogger as ADL
dso = ADL.DsoLogger() # monitor library sizes

include ("MuonTestEDM/MuonTestEDM_jobOptions.py")
MyEDMTester.DoDumpSummaryToFile = True
MyEDMTester.DoDumpToFile = False 

# Aesthetic track summary for all collections
include("MuonTrackPerformance/MuonTrackStatistics_jobOptions.py")

#from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg
#MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
MuonTrackStatistics.writeToFile = True
MuonTrackStatistics.TrackLocationList  = ["MooreTracks"]
if muonCombinedRecFlags.doMuidCombined():
    MuonTrackStatistics.TrackLocationList += ["ConvertedMuIdCBTracks"]
if muonCombinedRecFlags.doMuGirl():
    MuonTrackStatistics.TrackLocationList += ["MuGirlRefittedTracks"]
if muonCombinedRecFlags.doMuTagIMO():
    MuonTrackStatistics.TrackLocationList += ["MuTagIMOTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyTracks"]
if muonCombinedRecFlags.doStaco():
    MuonTrackStatistics.TrackLocationList += ["ConvertedStacoTracks"]
if muonCombinedRecFlags.doMuTag():
    MuonTrackStatistics.TrackLocationList += ["ConvertedMuTagTracks"]
MuonTrackStatistics.TrackLocationList += ["MuonSpectrometerTracks"]

# Suppress too many WARNINGs
from TrkResidualPullCalculator.TrkResidualPullCalculatorConf import Trk__ResidualPullCalculator
Trk__ResidualPullCalculator.OutputLevel = ERROR

# kluge to avoid crash in creating D3PD
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
AANTupleStream('D3PD').ExistDataHeader = False
