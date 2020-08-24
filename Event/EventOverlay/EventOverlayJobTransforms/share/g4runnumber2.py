
#set the max number of events
from OverlayCommonAlgs.OverlayFlags import overlayFlags
fname = overlayFlags.EventIDTextFile()
num_lines = 0
with open(fname, 'r') as f:
    for line in f:
        num_lines += 1
printfunc ("Number of lines in events.txt is "+str(num_lines))
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax.unlock()
athenaCommonFlags.EvtMax.set_Value_and_Lock(num_lines)

#use this for the run/event numbers to look up in the vertexoverridefile, if we are overriding vertices
#job.VertexPositionGenerator.VertexOverrideEventFile = "events.txt" #old MC11 way

#new MC12 way, should be a preInclude
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.VertexOverrideEventFile.set_Value_and_Lock(fname)

