
## Enable the looper killer, with options for overlay
#  Don't kill the event, but flag it as bad
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::LooperKillerEventOverlayTool', ['Step'])
