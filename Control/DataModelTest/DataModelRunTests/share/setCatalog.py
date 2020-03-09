#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
#
# File: DataModelRunTests/share/setCatalog.py
# Author: snyder@bnl.gov
# Date: Mar 2018
# Purpose: Helper to set the file catalog being written.
#
# Needed to avoid races when running tests in parallel.
#

if 'FILECATALOG' in globals():
    from PoolSvc import PoolSvcConf
    PoolSvc = PoolSvcConf.PoolSvc()
    PoolSvc.WriteCatalog = 'file:' + FILECATALOG

    import os
    try:
        os.remove (FILECATALOG)
    except OSError:
        pass

