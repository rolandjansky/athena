# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..utility import RepeatingTimer
import datetime
from functools import wraps

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


def timed(name):
    """! Decorator to output function execution time.

    @param name Name of the process being timed.

    @author James Robinson <james.robinson@cern.ch>
    """
    def wrap(func):
        @wraps(func)
        def wrapped_f(*args, **kwargs):
            start_time = datetime.datetime.now()
            logger.info("=> Preparing to run {} <=".format(name))
            result = func(*args, **kwargs)
            logger.info("=> Running {} took {} <=".format(name, RepeatingTimer.readable_duration(datetime.datetime.now() - start_time)))
            return result
        return wrapped_f
    return wrap
