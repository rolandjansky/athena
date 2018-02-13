from G4AtlasApps.SimFlags import simFlags
# new MT actions. Note that this will only work with one thread.
simFlags.OptionalUserActionList.addAction('G4UA::StepNtupleTool',['Event','Run','Step'])
# configure the NTupleSvc
theApp.HistogramPersistency = "ROOT"
svcMgr.NTupleSvc = Service( "NTupleSvc" )
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='stepntuple.root' TYP='ROOT' OPT='NEW'" ]

