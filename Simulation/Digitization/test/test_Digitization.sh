#!/bin/sh
#
# art-description: Run digitization of an MC16a ttbar sample with 2016a geometry and conditions, without pile-up
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

athena Digitization/DigitizationRTT_all_noLVL1.py

echo "art-result: $? digitization"
