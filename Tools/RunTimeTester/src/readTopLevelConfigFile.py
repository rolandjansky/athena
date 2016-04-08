# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from exc2string2      import exc2string2
from RTTConfigParser  import RTTConfiguration
from RTTSException    import RTTInputError

def readTopLevelConfigFile(confFilePath, logger):
    try:
        rttConfig = RTTConfiguration(confFilePath)
    except Exception, e:
        m  = 'Exception thrown while reading  top level RTT Config file: %s:\nTraceBack:\n %s' % (str(e), exc2string2())
        logger.critical(m)
        raise RTTInputError(m)


    
    if rttConfig.configIsValid():
        msg = 'Top level configuration %s is valid ' % str(rttConfig)
        logger.debug(msg)
        return rttConfig

    
    msg = 'Top level configuration %s is invalid: %s' % (str(rttConfig), rttConfig.text())
    logger.critical(msg)
    raise RTTInputError(msg)
                
