# Fragment for setting up the verbose selector based on area.
# Can be used to enable G4 verbose output for a specific event, and for a specific region of the detector if desired.
# This is VERY handy for looking into problems in G4.
# Verbose level is transmitted to several G4 classes to increase their verbosity.
# Note that the event number is the number of events into the G4 run, rather than the ATLAS event number.

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicToolClone,getPublicTool

vsArea=getPublicTool('VerboseSelector/VerboseSelectorArea',tryDefaultConfigurable=True)

vsArea.Xmin=-25000
vsArea.Xmax=25000
vsArea.Ymin=-25000
vsArea.Ymax=25000
vsArea.Zmin=-30000
vsArea.Zmax=30000
vsArea.TargetEvent=1
vsArea.VerboseLevel=1

UAStore.addAction(vsArea,['EndOfEvent','Step'])

