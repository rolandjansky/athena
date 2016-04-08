# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, os.path, exc2string2

# ----- Some useful decorator functions

def timeit(func): # time the function
    def wrapped(*args, **kwds):
        import time
        start = time.time()
        ret = None
        try:
            ret = func(*args,**kwds)
        except Exception, e:
            print '%s threw an exception!' % func.__name__
            print str(e)
            print exc2string2.exc2string2()
        print '%f secs to run %s.' % (time.time()-start, func.__name__)
        return ret
    return wrapped

def log(func): # log the result of a check
    def wrapped(*a,**k):
        ret = None
        try:
            ret = func(*a,**k)
        except Exception, e:
            print 'FAIL : %s [%s]' % (func.__name__, 'Test code broken? Raised an exception: %s' % str(e))
        else:            
            print '%s%s' % ({True:'PASS : ', False:'FAIL : '}.get(ret), func.__name__)
        return ret
    return wrapped

def output(what, where):
    h = open(where, 'w')
    h.write(what)
    h.close()

# --- Sys path manipulations

class SysPath:
    def __contains__(self, path):
        return path in sys.path

    def __iadd__(self, path):
        path = os.path.abspath(path)
        if not path in self:
            sys.path = [path] + sys.path
        return self

    def __isub__(self, path):
        path = os.path.abspath(path)
        sys.path = [s for s in sys.path if s!=path]
        return self

    def __str__(self):
        return '%d: %s' % (len(sys.path), str(sys.path))

# --- Sys Modules manipulations
class SysModules:
    def __contains__(self, modulename):
        return modulename in sys.modules.keys()

    def __isub__(self, modulename):
        if modulename in self:
            del sys.modules[modulename]
        return self

