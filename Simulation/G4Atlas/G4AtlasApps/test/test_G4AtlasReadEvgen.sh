#!/bin/sh
#
# art-description: G4Atlas Read Evgen example
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation

athena G4AtlasApps/jobOptions.G4Atlas_ReadEvgen.py

echo  "art-result: $? simulation"
