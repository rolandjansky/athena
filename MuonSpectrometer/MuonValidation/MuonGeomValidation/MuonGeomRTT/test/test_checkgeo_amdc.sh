#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer description, just AMDB part from amdb_simrec
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.txt
# art-output: *.data
# art-output: *.PREsql

art.py createpoolfile

set -x

# download an amdb file to test
wget http://atlas.web.cern.ch/Atlas/GROUPS/MUON/AMDB/amdb_simrec.r.08.01
# database_layout needed for AmdcDb/AmdcDb_TableCheck.py
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_amdb_simrec='amdb_simrec.r.08.01';CheckAMDBTables=True;database_layout='MuonSpectrometer-R.08.01'"

exit_code=$?
echo  "art-result: ${exit_code} AmdcAth_GeometryTasks.py"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

# now check if the created DB files are non-empty
aGoodFile=false
for file in Out.AmdcOracle.DB.*
do
    nLines=$(cat "$file"|wc -l)
    echo "${file} has ${nLines} lines" >> dbCheck.txt
    if [ "$nLines" -gt 1 ] ; then
        aGoodFile=true
    fi
done
if [ "$aGoodFile" = false ] ; then
    exit 1
fi

echo  "art-result: ${exit_code} file-check"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

echo "art-result: $?"
