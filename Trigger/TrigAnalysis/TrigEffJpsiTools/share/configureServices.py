###
### extrapolator
###
include("TrkDetDescrSvc/AtlasTrackingGeometrySvc.py")
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator = AtlasExtrapolator()
ToolSvc += theAtlasExtrapolator


###
### TrigEffJpsiTools
###
from TrigEffJpsiTools.TrigEffJpsiToolsConf import TrigEffJpsiTools
theTrigEffJpsiTools = TrigEffJpsiTools("TrigEffJpsiTools")

# barrel and endcap enlarged.
# not to lose curly tracks escaping through barrel/end-cap transition regions
theTrigEffJpsiTools.EndcapPivotPlaneMaximumRadius = 20000.
theTrigEffJpsiTools.BarrelPivotPlaneHalfLength = 16000.

ToolSvc += theTrigEffJpsiTools

# eof
