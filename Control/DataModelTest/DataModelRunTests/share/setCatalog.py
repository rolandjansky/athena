#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
#
# File: DataModelRunTests/share/setCatalog.py
# Author: snyder@bnl.gov
# Date: Mar 2018
# Purpose: Helper to set the file catalog being written.
#
# Needed to avoid races when running tests in parallel.
#

PoolSvc = Service( 'PoolSvc' )
PoolSvc.WriteCatalog = 'file:' + FILECATALOG

import os
try:
    os.remove (FILECATALOG)
except OSError:
    pass

