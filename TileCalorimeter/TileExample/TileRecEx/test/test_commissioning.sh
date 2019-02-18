#!/bin/sh

# art-description: CaloCosEx/Commissioning ATN test
# art-type: build
# art-include: */Athena

athena.py TileRecEx/jobOptions_TileCommissioningAtn.py
echo "art-result: $?"
