# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import types
def tracing(f, name):
    def traced_f(*a, **k):
        # print '%s(%s,%s) -> '%(name, a,k),
        print '\n-- Trace:%s --'%(name)
        result = f(*a, **k)
        print '-- Trace:%s result %s --'%(name, result)
        return result
    return traced_f

class meta_tracer(type):
    def __new__(self, classname, bases, classdict):
        for f in classdict:
            m = classdict[f]
            if isinstance(m, types.FunctionType):
                classdict[f]=tracing(m, 'Trace: %s.%s' % (classname, f))
        return type.__new__(self, classname, bases, classdict)

class Tracer:
    __metaclass__ = meta_tracer
