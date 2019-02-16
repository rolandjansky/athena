#!/bin/sh

# art-description: CaloCosEx/Cosmic ATN test
# art-type: build
# art-include: */Athena

athena.py -c RunFromLocal=True TileRecEx/jobOptions_TileCosmicAtn.py
echo "art-result: $?"
