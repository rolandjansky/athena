#!/bin/sh

# art-description: CaloCosEx/Cosmic14 ATN test
# art-type: grid
# art-include: */Athena

athena.py -c RunNumber=239908 TileRecEx/jobOptions_TileCosmicAtn.py
echo "art-result: $?"
