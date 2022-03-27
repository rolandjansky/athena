# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# @brief Switch database to using FRONTIER, but with a fallback
#        to DBRelease if FRONTIER_SERVER is undefined (e.g., on HPC)

import os
from AthenaCommon.Logging import logging
log = logging.getLogger('UseFrontier.py')

if(os.environ.get('FRONTIER_SERVER')):
    log.info('Enabling FRONTIER DB access')
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    ServiceMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="DBRelease")
else:
    log.info('Using default DB access')
