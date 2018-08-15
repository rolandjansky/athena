#!/bin/sh
#
# art-description: Test of core pile-up digitization code using an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

athena Digitization/NightlyPileUp.py

echo "art-result: $? digitization"
