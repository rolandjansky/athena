include.block( "TrigSteering/jobOfragment_TrigSteering_New.py" )
theApp.Dlls += [ "TrigSteering" ]

theApp.CreateSvc += [ "HLT::ScalerSvc/ScalerSvc" ]
ScalerSvc = Service( "ScalerSvc" )

#ScalerSvc.DefaultType = "HLT::RandomScaler"
ScalerSvc.DefaultType = "HLT::PeriodicScaler"

# configuration service
include("TrigSteering/jobOfragment_CONFIG_New.py")



