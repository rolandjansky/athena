# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from exc2string2     import exc2string2
from RTTSException   import RTTCodingError
from JobGroupKits    import JobGroupKits


import logging
logger = logging.getLogger('rtt')

def getJobGroupKit(jobGroup, jobGroupKits):
    
    jobGroupKit = None
    msg ='Could not obtain a kit with name '

    try:
        kit    = jobGroupKits.kit(jobGroup)
    except:
        msg = 'getJobGroupKit: Failed to  obtain a kit with name %s' % jobGroup
        print msg
    else:
        return kit
        
    if not jobGroupKit:
        msg = 'getJobGroupKit: Attempting to  obtain a kit with name %s' % jobGroup
        print msg
        jobGroup = 'Athena'+jobGroup
        try:
            kit = jobGroupKits.kit(jobGroup)
        except:
            msg = 'getJobGroupKit: Failed to obtain a kit with name %s\nTraceback:\n%s' % (jobGroup, exc2string2())
            logger.error(msg)
            raise RTTCodingError(msg)
        else:
            msg = 'getJobGroupKit: Succeeded in obtain a kit with name %s' % jobGroup
            logger.debug(msg)
            return kit
