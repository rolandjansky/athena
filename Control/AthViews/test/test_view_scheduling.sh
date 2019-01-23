#!/bin/sh
# art-type: build
# art-include: master/Athena
# art-ci: master

athena.py --threads=1 AthViews/ViewScheduling.py | tee ViewScheduling_ath.log
if grep "INFO condTestAlg running with store view_1" ViewScheduling_ath.log; then
  exit 1
fi
if ! grep "INFO condTestAlg running with store view_0" ViewScheduling_ath.log; then
  exit 1
fi
if ! grep "INFO 109" ViewScheduling_ath.log; then
  exit 1
fi
exit 0
