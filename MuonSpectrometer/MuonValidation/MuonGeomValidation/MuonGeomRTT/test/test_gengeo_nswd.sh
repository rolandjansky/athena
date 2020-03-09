#!/bin/sh
#
# art-description: Generate SQL files for muon spectrometer description, just AGDD part from amdc_simrec (for NSW)
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

art.py createpoolfile

set -x

get_files stations.v2.06.xml
wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.09.00
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_nsw_xml='stations.v2.06.xml'; input_amdb_simrec='amdb_simrec.r.09.00';database_layout='MuonSpectrometer-R.09.00.NSW';DoNSWDBlob=True;"

echo "art-result: $?"