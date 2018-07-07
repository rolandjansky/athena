#!/bin/sh
# art-type: build
# art-ci: master

athena.py --threads=1 AthViews/ViewScheduling.py | tee ViewScheduling.log
if grep "INFO condTestAlg running with store view1" ViewScheduling.log; then
  exit 1
fi
if ! grep "INFO condTestAlg running with store view0" ViewScheduling.log; then
  exit 1
fi
if ! grep "INFO 109" ViewScheduling.log; then
  exit 1
fi
exit 0
