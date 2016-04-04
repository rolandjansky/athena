## Add G4AtlantisDumper to the UserActions

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('G4AtlantisDumper',[ 'BeginOfEvent', 'EndOfEvent', 'Step'])

