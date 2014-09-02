

from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsTool

MuonTrackStatisticsTool = MuonTrackStatisticsTool("MuonTrackStatisticsTool")
MuonTrackStatisticsTool.OutputLevel= WARNING

ToolSvc += MuonTrackStatisticsTool


from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg

MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
MuonTrackStatistics.doTruth = False
MuonTrackStatistics.writeToFile = False
MuonTrackStatistics.FileName = "trkSummary.txt"

#uncomment below to add a muon container to the summary

MuonTrackStatistics.TrackLocationList += ["MooreTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMuIdCBTracks"]
MuonTrackStatistics.TrackLocationList += ["MuidExtrapolatedTracks"]
MuonTrackStatistics.TrackLocationList += ["MuGirlRefittedTracks"]
MuonTrackStatistics.TrackLocationList += ["MuTagIMOTracks"]

MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyTracks"]
#MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyMuonSpectroOnlyTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedStacoTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMuTagTracks"]

MuonTrackStatistics.TrackLocationList += ["MuonSpectrometerTracks"]


MuonTrackStatistics.OutputLevel = INFO

topSequence += MuonTrackStatistics
print MuonTrackStatistics


