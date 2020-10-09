#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer description, just AGDD part from amdc_simrec (only passive materials)
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.txt
# art-output: *.data

art.py createpoolfile

set -x

# download an amdb file to test
wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
# run CheckAGDDBlob which will create Out.AmdcOracle.AM.AGDDtemp.data from the AGDD part of the amdb file, and Generated_AGDD_pool.txt from the AGDD blob in the DB
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';database_layout='MuonSpectrometer-R.08.01-NSW';CheckAGDDBlob=True;"

exit_code=$?
echo  "art-result: ${exit_code} AmdcAth_GeometryTasks.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

# now, compare the output of AGDD from the local file with the blob from the db
diff Generated_AGDD_pool.txt Out.AmdcOracle.AM.AGDDtemp.data &> diff_database_local.txt
echo  "art-result: ${exit_code} diff-check"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

echo "art-result: $?"
