from RecExConfig.RecFlags import rec as recFlags
from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsTool

MuonTrackStatisticsTool = MuonTrackStatisticsTool("MuonTrackStatisticsTool")
MuonTrackStatisticsTool.OutputLevel= WARNING
MuonTrackStatisticsTool.doTruth = rec.doTruth()

ToolSvc += MuonTrackStatisticsTool


from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg

MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
MuonTrackStatistics.doTruth = True
MuonTrackStatistics.writeToFile = True #False
MuonTrackStatistics.FileName = "trkSummary.txt"

#uncomment below to add a muon container to the summary
from MuonRecExample.MuonRecFlags import muonRecFlags

if muonRecFlags.doMoore():
   MuonTrackStatistics.TrackLocationList += ["MooreTracks"]
if muonRecFlags.doMuonboy(): 
   MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyTracks"]
if muonRecFlags.doStandalone(): 
   MuonTrackStatistics.TrackLocationList += ["MuonSpectrometerTracks"]


#MuonTrackStatistics.TrackLocationList += ["ConvertedMuIdCBTracks"]
#MuonTrackStatistics.TrackLocationList += ["ConvertedStacoTracks"]
#MuonTrackStatistics.TrackLocationList += ["MuGirlRefittedTracks"]

if rec.doTruth():
   if muonRecFlags.doMoore():
      MuonTrackStatistics.TruthTrackLocationList += ["MooreTracksTruth"]
   if muonRecFlags.doMuonboy():
      MuonTrackStatistics.TruthTrackLocationList += ["ConvertedMBoyTracksTruth"]
   if muonRecFlags.doStandalone(): 
      MuonTrackStatistics.TruthTrackLocationList += ["MuonSpectrometerTracksTruth"]

#MuonTrackStatistics.TrackLocationList += ["ConvertedMuIdCBTracksTruth"]
#MuonTrackStatistics.TrackLocationList += ["ConvertedStacoTracksTruth"]
#MuonTrackStatistics.TrackLocationList += ["MuGirlRefittedTracksTruth"]

MuonTrackStatistics.OutputLevel = INFO

topSequence += MuonTrackStatistics
print MuonTrackStatistics


