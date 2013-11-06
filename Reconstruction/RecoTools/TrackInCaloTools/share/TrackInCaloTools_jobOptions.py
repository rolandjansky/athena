#--------------------------------------------------------
#
include.block ( "TrackInCaloTools/TrackInCaloTools_jobOptions.py" )

from TrackInCaloTools import TrackInCaloTools
ToolSvc += TrackInCaloTools.TrackInCaloTools()

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ToolSvc += AtlasExtrapolator()
#--------------------------------------------------------
