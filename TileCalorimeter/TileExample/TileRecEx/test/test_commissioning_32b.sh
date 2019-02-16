#!/bin/sh

# art-description: CaloCosEx/Commissioning)32b ATN test
# art-type: build
# art-include: */Athena

athena.py  -c RunFromLocal=True TileRecEx/jobOptions_TileCommissioningAtn.py
echo "art-result: $?"
