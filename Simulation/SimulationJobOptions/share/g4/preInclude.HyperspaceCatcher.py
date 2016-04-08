# Fragment for setting up the hyperspace catcher
# This code watches out for particles that have entered hyperspace and kill them
# This is VERY handy for looking into problems in G4 - it should NOT be used for standard production.
# Set "killAfter" in order to crash the job after a certain number of hyperspace particles

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool

catcher=getPublicTool('HyperspaceCatcher',tryDefaultConfigurable=True)
catcher.TreatmentLevel=1
catcher.KillAfter=-1

UAStore.addAction(catcher,['BeginOfRun','Step'])
