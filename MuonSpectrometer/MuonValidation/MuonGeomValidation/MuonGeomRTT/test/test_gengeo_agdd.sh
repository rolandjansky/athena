#!/bin/sh
#
# art-description: Generate SQL files for muon spectrometer description, just AGDD part from .xml
#
# art-type: grid
# art-include: master/Athena
#
# art-output: *.data

art.py createpoolfile

set -x

# download the passive material AGDD xml file
get_files passiveMat_r.08.02.xml
# create the AGDD blob from it
athena.py AmdcAth/AmdcAth_GeometryTasks.py -c "input_agdd_xml='passiveMat_r.08.02.xml';DoAGDDBlob=True;amdc_name='R.08'"

echo "art-result: $?"
