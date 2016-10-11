# Use verbose output for all G4 process.
# Only works in dbg builds.

from G4AtlasApps.SimFlags import simFlags
simFlags.G4Commands += ['/process/verbose 10']
