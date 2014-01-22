from AthenaCommon.AthenaCommonFlags import jobproperties

jobproperties.AthenaCommonFlags.PoolHitsInput = ["atlasG4.hits.pool.root"]
jobproperties.AthenaCommonFlags.PoolRDOOutput =  "digi.pool.root"
jobproperties.AthenaCommonFlags.EvtMax = -1

from AthenaCommon.GlobalFlags import jobproperties

jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-20-00-01'


from AthenaCommon.DetFlags import DetFlags

DetFlags.AFP_setOn()
DetFlags.Truth_setOn()

include("Digitization/Digitization.py")

Service("StoreGateSvc").ActivateHistory = False
Service("GeoModelSvc").IgnoreTagDifference = True

