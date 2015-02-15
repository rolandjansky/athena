########################################################################
# Lines below add the "nHoles" info in the TrkValidation ntuple
########################################################################
from TrkValTools.TrkValToolsConf import Trk__TrackSummaryNtupleTool
InDetTrackSummaryNtupleTool = Trk__TrackSummaryNtupleTool(name="InDetTrackSummaryNtupleTool")
ToolSvc += InDetTrackSummaryNtupleTool
if InDetFlags.doPrintConfigurables():
   print InDetTrackSummaryNtupleTool

from TrkValAlgs.TrkValAlgsConf import Trk__TrackValidationNtupleWriter
TrkValNtupleWriter = Trk__TrackValidationNtupleWriter(name = 'InDetValNtupleWriter',
ValidationNtupleTools = [ InDetTrackInfoNtupleTool, InDetPerigeeNtupleTool, InDetMeastsNtupleTool, InDetTrackSummaryNtupleTool ],
NtupleFileName = 'TRKVAL',
NtupleDirectoryName = 'Validation',
DoTruth = InDetFlags.doTruth(),
TruthToTrackTool = TruthToTrackTool,
TrackSelectorTool = TrkValTrackSelectorTool)
#########################################################################
