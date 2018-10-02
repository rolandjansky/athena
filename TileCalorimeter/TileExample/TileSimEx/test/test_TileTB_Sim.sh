#!/bin/sh
#
# art-description: Tile Test Beam Simulation example
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation

athena TileSimEx/jobOptions_TileTB_Sim.py

echo  "art-result: $? simulation"
