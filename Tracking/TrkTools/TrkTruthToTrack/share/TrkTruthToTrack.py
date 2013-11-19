include.block("TrkTruthToTrack/TrkTruthToTrack.py")

from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
TrkTruthToTrack  = Trk__TruthToTrack( name = "TrkTruthToTrack" )

ToolSvc = Service("ToolSvc")
ToolSvc += TrkTruthToTrack
print      TrkTruthToTrack


