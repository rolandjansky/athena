# Fragment for setting up the verbose selector based on area.
# Can be used to enable G4 verbose output for a specific event, and for a specific region of the detector if desired.
# This is VERY handy for looking into problems in G4.
# Verbose level is transmitted to several G4 classes to increase their verbosity.
# Note that the event number is the number of events into the G4 run, rather than the ATLAS event number.

def verboseSelectorArea_setup():
    print 'Firing up the verbose selector'
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'VerboseSelectorArea', ['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
    myAction.set_Properties( {"Xmin" : "-25000", "Xmax" : "25000",
                              "Ymin" : "-25000", "Ymax" : "25000",
                              "Zmin" : "-30000", "Zmax" : "30000",
                              "targetEvent" : "10",
                              "verboseLevel" : "1"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", verboseSelectorArea_setup)
