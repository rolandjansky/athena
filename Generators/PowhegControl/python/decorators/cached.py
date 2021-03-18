# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from functools import wraps


def cached(func):
    """! Decorator to cache function return value.

    @param func Function being cached

    @author James Robinson <james.robinson@cern.ch>
    """
    cache = {}

    @wraps(func)
    def wrapped_f(*args):
        if args in cache:
            return cache[args]
        else:
            return_value = func(*args)
            cache[args] = return_value
            return return_value
    return wrapped_f
