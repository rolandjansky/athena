
## Enable the looper killer, with options for overlay
#  Don't kill the event, but flag it as bad
def use_looperkiller():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    lkAction = PyG4Atlas.UserAction('G4UserActions', 'LooperKiller', ['BeginOfRun', 'EndOfRun', 'BeginOfEvent', 'EndOfEvent', 'Step'])
    #lkAction.set_Properties( {"PrintSteps":"0", "MaxSteps":"10", "VerboseLevel":"0", "AbortEvent":"0", "SetError":"1"} ) #to kill most everything
    lkAction.set_Properties( {"PrintSteps":"10", "MaxSteps":"1000000", "VerboseLevel":"1", "AbortEvent":"0", "SetError":"1"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(lkAction)

simFlags.InitFunctions.add_function("postInit", use_looperkiller)

