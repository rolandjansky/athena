#!/bin/bash
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# test_thinning
#


# Run the test:
mkdir -p test_thinning
cd test_thinning
chappy.py AthExThinning/test_thinning.py

#athena.py --evtMax=5 -c 'OUTPUT="my.data.tothin.pool"' AthExThinning/AthExThinning_makeData.py 2>&1 | tee  AthExThinning_makeData.log

## Validate  AthExThinning_makeData.ref

#athena.py --evtMax=5 -c 'INPUT=["my.data.tothin.pool"]; OUTPUT="thinned.data.pool"' AthExThinning/WriteThinnedData_jobOptions.py 2>&1 | tee log


