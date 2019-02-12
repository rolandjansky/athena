#!/bin/sh
#
# art-description: G4Cosmic example
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

athena G4AtlasApps/jobOptions.G4Cosmic.py

echo  "art-result: $? simulation"
