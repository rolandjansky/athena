# Job options to configure the Geant4 setup checker tool
# Only add the setup checker in the case that we are in a validated configuration
# That requires all of ATLAS to be on as well
from G4AtlasApps.SimFlags import simFlags
from AthenaCommon.DetFlags import DetFlags
if 'ATLAS' in simFlags.SimLayout() and\
   not '_VALIDATION' in simFlags.SimLayout() and\
   DetFlags.ID_allOn() and DetFlags.Calo_allOn() and DetFlags.Muon_allOn() and not DetFlags.Forward_on() and\
   0==simFlags.LArParameterization():
    simFlags.OptionalUserActionList.addAction('G4UA::Geant4SetupCheckerTool',['BeginOfRun'])

# This flag disables checking so that you can write your own reference file
#simFlags.UserActionConfig.addConfig('G4UA::Geant4SetupCheckerTool','RunTest',False)
