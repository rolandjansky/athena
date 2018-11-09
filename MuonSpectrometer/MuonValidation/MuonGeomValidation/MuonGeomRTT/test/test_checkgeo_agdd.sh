#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer description, just AGDD part from amdc_simrec (for NSW, using NSW layout)
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

art.py createpoolfile

set -x

wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';database_layout='MuonSpectrometer-R.08.01-NSW';CheckAGDDBlob=True;"

echo "art-result: $?"
