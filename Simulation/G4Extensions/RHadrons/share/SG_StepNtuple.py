
from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('SG_StepNtuple',['BeginOfEvent','EndOfEvent','Step'])

theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='SG_StepNtuple.root' TYP='ROOT' OPT='NEW'" ]

