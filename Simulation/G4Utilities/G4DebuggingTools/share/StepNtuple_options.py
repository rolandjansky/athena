from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('StepNtuple',['BeginOfEvent','Step','EndOfEvent'])

# configure the NTupleSvc
theApp.HistogramPersistency = "ROOT"
svcMgr.NTupleSvc = Service( "NTupleSvc" )
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='stepntuple.root' TYP='ROOT' OPT='NEW'" ]
