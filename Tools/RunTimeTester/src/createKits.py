# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from kitFromConfFile   import kitFromConfFile
from exc2string2       import exc2string2
from JobGroupsParser   import makeTopLevelJobGroupKits
from JobGroupKits      import JobGroupKits
from RTTSException     import RTTCodingError

def createKits(cvsPackages, jobGroupConfig, logger):
    try:
        userDefinedKits = kitFromConfFile(cvsPackages)
    except Exception, e:
        m = 'createKits: Exception %s\nTraceback:\n' % (str(e), exc2string2())
        logger.critical(m)
        raise RTTCodingError(m)

    # ---------------------------------------------------------
    # get dictionary for valid rtt supplied conf files
    # ---------------------------------------------------------
    rttDefinedKits = makeTopLevelJobGroupKits(jobGroupConfig)
    
    # ---------------------------------------------------------
    # make a singleton kit distributor
    # ---------------------------------------------------------
    return JobGroupKits(rttDefinedKits, userDefinedKits)
     
