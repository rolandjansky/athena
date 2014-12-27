
def add_SG_StepNtuple():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    mySGStepNtupleAction = PyG4Atlas.UserAction('RHadrons', 'SG_StepNtuple', ['BeginOfRun', 'EndOfRun', 'BeginOfEvent', 'EndOfEvent', 'Step'])
    #mySGStepNtupleAction.set_Properties({"Edep" : "0.1", "Length" : "1.0", "P" : "10.0", "KE" : "10.0"})
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(mySGStepNtupleAction)

from G4AtlasApps.SimFlags import SimFlags
SimFlags.InitFunctions.add_function("preInitG4", add_SG_StepNtuple)

theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='SG_StepNtuple.root' TYP='ROOT' OPT='NEW'" ]

