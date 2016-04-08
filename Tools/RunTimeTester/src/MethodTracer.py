# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Taken verbatim from the python cookbook (recipe 15.11)
Monitors entering and leaving of object methods
"""


import types

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

def tracing(f, name):
    def traced_f(*a, **k):
        msg = '%s(%s,%s)->'%(name, a, k)
        result = f(*a, **k)
        msg += str(result)
        # logger.info(msg)
        # print '-----------------------------------------------'
        # print msg
        # print '-----------------------------------------------'
        logger.debug(msg)
        return result
    return traced_f

class MetaTracer(type):
    def __new__(self, classname, bases, classdict):
        for f in classdict:
            m = classdict[f]
            if isinstance(m, types.FunctionType):
                classdict[f]=tracing(m, '%s.%s'%(classname,f))
        return type.__new__(self, classname, bases, classdict)

class Tracer:
    __metaclass__ = MetaTracer

if __name__ == '__main__':
    from setupLoggerForDebug import setupLoggerForDebug
    
    setupLoggerForDebug()
    # give a __metaclass__ to all classes without one
    __metaclass__ = MetaTracer

    # a test class
    class A:
        def __init__(self, j): self.j=j
        def a(self):
            return 'hello'

        def b(self, i):
            return i+self.j

    
    a = A(23)
    # demonstrate tracing
    a.a()
    a.b(10)

