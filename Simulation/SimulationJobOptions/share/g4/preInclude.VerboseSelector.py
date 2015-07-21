# Include the verbose selector based on track ID
# Very handy for debugging crashes on the grid when you know what track caused the crash
# Most powerful in conjunction with the verbose selector area

def verboseSelectorArea_setup():
    print 'Firing up the verbose selector'
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'VerboseSelector', ['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
    #myAction.set_Properties( {"Xmin" : "-25000" , "Xmax":"25000" , "Ymin":"-25000","Ymax":"25000","Zmin":"-30000","Zmax":"30000","targetEvent":"11","verboseLevel":"1"} )
    myAction.set_Properties( {"targetTrack":"1","targetBarcode":"1","targetEvent":"11","verboseLevel":"1"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", verboseSelectorArea_setup)
