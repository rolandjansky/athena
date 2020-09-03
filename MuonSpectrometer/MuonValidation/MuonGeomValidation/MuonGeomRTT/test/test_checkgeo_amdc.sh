#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer description, just AMDB part from amdb_simrec
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.txt
# art-output: *.data

art.py createpoolfile

set -x

wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
# database_layout needed for AmdcDb/AmdcDb_TableCheck.py
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';CheckAMDBTables=True;database_layout=''"

echo "art-result: $?"
