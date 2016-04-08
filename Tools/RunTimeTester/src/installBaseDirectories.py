# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseDirectoryMaker import BaseDirectoryMakerFactory
from exc2string2                import exc2string2

def installBaseDirectories(paths, logger):
    try:
        factory = BaseDirectoryMakerFactory(logger)
        directoryMaker = factory.create(paths)
        directoryMaker.makeDirs()
    except Exception, e:        
        m = 'Fatal error while making directories: %s\n' % str(e)
        m += exc2string2()
        logger.critical(m)

