#!/bin/sh
#
# art-description: Generate SQL files for muon spectrometer description, just AGDD part from amdb_simrec file
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.data

art.py createpoolfile

set -x

wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';DoAGDDBlob=True;"

echo "art-result: $?"
