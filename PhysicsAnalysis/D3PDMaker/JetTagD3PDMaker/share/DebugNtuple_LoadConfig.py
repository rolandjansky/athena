from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

from RecExConfig.RecFlags import rec

### include the full ntuple
include("JetTagD3PDMaker/FullNtuple_LoadConfig.py")

### change the cuts
JetTagD3PDFlags.MCPtMinCut = -1.
JetTagD3PDFlags.MCEtaCut =-1.
JetTagD3PDFlags.JetFilterPtMinCut = -1.
JetTagD3PDFlags.JetFilterEtaCut = 5.
JetTagD3PDFlags.LevelOffset=-100

### change some flags
JetTagD3PDFlags.SkimD3PD = False
JetTagD3PDFlags.AddContainedTrackP4 = True
JetTagD3PDFlags.MCEmptyEvents = True
TruthD3PDFlags.GenVertexOutPartAssoc = True
TruthD3PDFlags.GenVertexInPartAssoc = True
