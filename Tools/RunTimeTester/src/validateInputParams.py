# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTSException import RTTInputError
import os.path
def validateInputParams(logger, RTTSrcDir, RTTLibDir, topLevelRTTconfFileLocation):
    #----------------------------------------------
    # check for the ok-ness of the passed parameter
    #----------------------------------------------        
    if not os.path.isabs(topLevelRTTconfFileLocation):
        m = 'topLevelRTTconfFileLocation %s is not an absolute path; it must be. Exiting...' % topLevelRTTconfFileLocation
        logger.critical(m)
        raise RTTInputError(m)
    if not os.path.exists(topLevelRTTconfFileLocation):
        m = 'topLevelRTTconfFileLocation %s is not an absolute path; it must be. Exiting...' % topLevelRTTconfFileLocation
        logger.critical(m)
        raise RTTInputError(m)
        
