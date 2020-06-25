#!/bin/sh
#
# art-description: Check SQL files of muon spectrometer (NSW) description, just AGDD part from NSW
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

art.py createpoolfile

set -x

# get the NSW AGDD xml file
get_files stations.v2.07.xml

# run DoNSWDBlob which will create Out.AmdcOracle.AM.NSWDtemp.data and Out.AmdcOracle.AM.NSWD.PREsql from the NSW AGDD file
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_nsw_xml='stations.v2.07.xml';database_layout='MuonSpectrometer-R.09.02.NSW';DoNSWDBlob=True;"

exit_code=$?
echo  "art-result: ${exit_code} AmdcAth_GeometryTasks.py-DoNSWDBlob"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

# run CheckNSWDBlob which will create Generated_NSWD_pool.txt from the AGDD blob in the DB
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_nsw_xml='stations.v2.07.xml';database_layout='MuonSpectrometer-R.09.02.NSW';CheckNSWDBlob=True;"

exit_code=$?
echo  "art-result: ${exit_code} AmdcAth_GeometryTasks.py-CheckNSWDBlob"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

# now, compare the output of AGDD from the local file with the blob from the db
diff Generated_NSWD_pool.txt Out.AmdcOracle.AM.NSWDtemp.data
echo  "art-result: ${exit_code} diff-check"
if [ ${exit_code} -ne 0 ]
then
    exit ${exit_code}
fi

echo "art-result: $?"
