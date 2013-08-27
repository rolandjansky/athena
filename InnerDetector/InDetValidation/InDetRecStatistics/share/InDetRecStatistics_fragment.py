#
# --- load truth to track tool
#
from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
InDetTruthToTrack  = Trk__TruthToTrack(name         = "InDetTruthToTrack",
                                         Extrapolator = MyExtrapolator)#replace with whatever extrapolator you are using
ToolSvc += InDetTruthToTrack
print      InDetTruthToTrack
#
# --- load statistics alg
#
from InDetRecStatistics.InDetRecStatisticsConf import InDet__InDetRecStatisticsAlg
InDetRecStatistics = InDet__InDetRecStatisticsAlg (name                     = "InDetRecStatistics",
                                                     TrackCollectionKeys      = MyTrackCollections, #list of track collections
                                                     TrackTruthCollectionKeys = MyTruthTrackCollections, #list of corresponding truth collection,must be of same length as MyTrackCollection
                                                     MakeNtuple               = True,
                                                     TruthToTrackTool         = InDetTruthToTrack,
                                                     UseTrackSummary          = True,
                                                     SummaryTool              = MyInDetTrackSummaryTool,#your configured SummaryTool
                                                     minPt                    = 1000.0)#min pt for considered tracks
                                      
topSequence += InDetRecStatistics
print          InDetRecStatistics
# comment out following if make Ntuple = False
theApp.HistogramPersistency = "ROOT"
NTupleSvc         =  Service( "NTupleSvc" )
NTupleSvc.Output += [ "INDETRECSTAT DATAFILE='" + "InDetRecStatistics.root" + "'  TYP='ROOT'  OPT='NEW'" ]
