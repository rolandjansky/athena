#!/bin/sh
#
# art-description: Testing LAR pedestal
# art-type: build
# art-include: 21.0/Athena
# art-include: master/Athena
 
athena --CA LArCalibProcessing/LArCalib_PedestalAutoCorrConfig.py
 
echo  "art-result: $? pedestal"
