include.block ( "EventOverlayJobTransforms/InDetMcSignal_jobOptions.py" )

from AthenaCommon.GlobalFlags import globalflags
from OverlayCommonAlgs.OverlayFlags import OverlayFlags
if OverlayFlags.doPixel() or OverlayFlags.doSCT() or OverlayFlags.doTRT():

   from OverlayCommonAlgs.OverlayCommonAlgsConf import SaveInDetObjects
   job += SaveInDetObjects()
   if globalflags.DataSource()=='data':
      job.SaveInDetObjects.SCTType = True

