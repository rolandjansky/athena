#!/bin/sh

# art-description: CaloCosEx/Commissioning ATN test
# art-type: grid
# art-include: */Athena

athena.py TileRecEx/jobOptions_TileCommissioningAtn.py
echo "art-result: $?"
