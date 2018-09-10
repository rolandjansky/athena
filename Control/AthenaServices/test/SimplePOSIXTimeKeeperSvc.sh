#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# Script running the SimplePOSIXTimeKeeperSvc_test.py test jobOptions for CTest.
#

# Run the test:
athena.py AthenaServices/SimplePOSIXTimeKeeperSvc_test.py

# We expect this test to return an error.

if [ "$?" -eq 65 ]; then
  exit 0 
fi
exit 1
