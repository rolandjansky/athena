#!/bin/sh
# art-type: build
# art-ci: master

# We expect this command to fail, so invert the exit code
if athena.py --threads=1 AthViews/ViewNest.py; then
  exit 1
else
  exit 0
fi
