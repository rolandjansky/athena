# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Some useful decorators.
"""

import sys
from decorator import decorator

def _reraise_exception(new_exc, exc_info=None):
    if exc_info is None:
        exc_info = sys.exc_info()
    _exc_class, _exc, tb = exc_info
    raise new_exc.__class__ (new_exc, tb)
    
@decorator
def forking(func, *args, **kwargs):
    """
    This decorator implements the forking patterns, i.e. it runs the function
    in a forked process.
    see:
     http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/511474
    """
    import os
    try:
        import cPickle as pickle
    except ImportError:
        import pickle
        
    # create a pipe which will be shared between parent and child
    pread, pwrite = os.pipe()

    # do fork
    pid = os.fork()

    ## parent ##
    if pid > 0:
        os.close(pwrite)
        with os.fdopen(pread, 'rb') as f:
            status, result = pickle.load(f)
        os.waitpid(pid, 0)
        if status == 0:
            return result
        else:
            remote_exc = result[0]
            _reraise_exception(remote_exc)
            
    ## child ##
    else:
        os.close(pread)
        try:
            result = func(*args, **kwargs)
            status = 0
        except (Exception, KeyboardInterrupt) as exc:
            import traceback
            exc_string = traceback.format_exc(limit=10)
            for l in exc_string.splitlines():
                print ("[%d]"%os.getpid(),l.rstrip())
            sys.stdout.flush()
            result = exc, exc_string
            status = 1
        with os.fdopen(pwrite, 'wb') as f:
            try:
                pickle.dump((status,result), f, pickle.HIGHEST_PROTOCOL)
            except pickle.PicklingError as exc:
                pickle.dump((2,exc), f, pickle.HIGHEST_PROTOCOL)
        os._exit(0)
    pass # forking

