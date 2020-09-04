#!/bin/sh
#
# art-description: Generate SQL files for muon spectrometer description, just NSW AGDD .xml part
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.data

art.py createpoolfile

set -x

get_files stations.v2.07.xml

athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_nsw_xml='stations.v2.07.xml';DoNSWDBlob=True;"

echo "art-result: $?"
