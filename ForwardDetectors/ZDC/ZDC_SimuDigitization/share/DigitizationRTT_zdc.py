
from AthenaCommon.AthenaCommonFlags import jobproperties

jobproperties.AthenaCommonFlags.PoolHitsInput = ["atlasG4.hits.pool.root"]
jobproperties.AthenaCommonFlags.PoolRDOOutput =  "digi.pool.root"
jobproperties.AthenaCommonFlags.EvtMax = -1

from AthenaCommon.GlobalFlags import jobproperties

jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-18-01-00'

from AthenaCommon.DetFlags import DetFlags

DetFlags.ZDC_setOn()
DetFlags.Truth_setOn()

include("Digitization/Digitization.py")

Service("StoreGateSvc").ActivateHistory = False
Service("GeoModelSvc").IgnoreTagDifference = True

# Digitization parameters (comment the following lines to use default values)
zdc.ScaleStrip=3e-4
zdc.ScalePixel=1e-1
zdc.MaxTimeBin=5
zdc.OutputLevel = DEBUG

# Following lines to add also _ZdcRawChannelCollection_ in the output file (needed for ZdcD3PDMaker)
from ZdcRec.ZdcRecConf import ZdcRec
topSequence += ZdcRec()

if DetFlags.writeRDOPool.ZDC_on():
   Stream1.ItemList+=["ZdcRawChannelCollection#*"]
