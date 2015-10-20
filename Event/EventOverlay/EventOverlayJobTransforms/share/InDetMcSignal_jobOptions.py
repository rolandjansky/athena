include.block ( "EventOverlayJobTransforms/InDetMcSignal_jobOptions.py" )

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():

   from OverlayCommonAlgs.OverlayCommonAlgsConf import SaveInDetObjects
   job += SaveInDetObjects()
   if globalflags.DataSource()=='data':
      job.SaveInDetObjects.SCTType = True

