#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer description, just AMDB part from amdc_simrec (using NSW layout)
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

art.py createpoolfile

set -x

echo $AtlasVersion

if [ ${AtlasVersion:0:4} == "21.3" ]; then
    DATABASE_LAYOUT="database_layout='MuonSpectrometer-R.08.01-NSW'"
else
    DATABASE_LAYOUT="database_layout=''"
fi

wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';CheckAMDBTables=True;"$DATABASE_LAYOUT;

echo "art-result: $?"