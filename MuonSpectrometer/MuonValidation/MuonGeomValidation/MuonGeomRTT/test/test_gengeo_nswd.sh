#!/bin/sh
#
# art-description: Generate SQL files for NSW description
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

art.py createpoolfile

set -x

get_files stations.v2.09.xml
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_nsw_xml='stations.v2.09.xml';database_layout='MuonSpectrometer-R.09.03';DoNSWDBlob=True;"

echo "art-result: $?"
