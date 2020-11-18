"""pre- and post-includes for New style transform integrations

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
import os
from AthenaConfiguration.ComponentFactory import CompFactory

def postInclude_UseFrontier(ConfigFlags):
    """based on share/UseFrontier.py
    ## @brief Switch database to using FRONTIER, but with a fallback
    # to DBRelease if FRONTIER_SERVER is undefined (e.g., on HPC)

    # Move from RecJobTransforms to PyJobTransforms to enable use
    # in simulation ATN and KV jobs
    # $Id: UseFrontier.py 605683 2014-07-09 17:22:17Z graemes $
    """

    #check ConfigFlags are locked for postExec
    if not ConfigFlags.locked():
        raise RuntimeError('Running a postExec before locking ConfigFlags')

    acc = ComponentAccumulator()
    if(os.environ.get('FRONTIER_SERVER')):
        print('UseFrontier.py: Enabling FRONTIER DB access')
        acc.addService(CompFactory.DBReplicaSvc(COOLSQLiteVetoPattern="DBRelease"))
    else:
        print('UseFrontier.py: Using default DB access')

    return acc