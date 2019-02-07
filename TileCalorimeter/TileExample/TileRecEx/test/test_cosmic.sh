#!/bin/sh

# art-description: CaloCosEx/Cosmic ATN test
# art-type: grid
# art-include: */Athena

athena.py TileRecEx/jobOptions_TileCosmicAtn.py
echo "art-result: $?"
