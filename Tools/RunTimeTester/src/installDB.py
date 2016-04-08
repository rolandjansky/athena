# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, anydbm
def installDB(dbName, logger):
    if os.path.exists(dbName): return
    
    cfile = anydbm.open(dbName, 'c')
    cfile.close()
    statusText = 'moni db created'
    logger.info(statusText)
