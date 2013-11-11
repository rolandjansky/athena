theApp.Dlls += [ "TrigT1TGCRecRoiSvc" ]
# RoI reconstruction service
theApp.ExtSvc += ["LVL1TGC::TGCRecRoiSvc"]

TGCRecRoiSvc = Service( "LVL1TGC::TGCRecRoiSvc" )
