# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import inspect
import warnings

class HashTool:
    def __init__(self, *args, **parse):
        self.argvals = tuple()
        self.objs = tuple()
        self.signatures = args
        self.parse = parse

    def vars_wrapper(self,*args,**kwargs):
        argspec = inspect.getargspec(self.func)
        args_name,varargs_name = argspec[:-2]
        argval = dict(zip(args_name, args[:len(args_name)]) + [(varargs_name, args[len(args_name):])] + kwargs.items())
        signatures = self.signatures if self.signatures else argval.keys()
        pattern = dict((s, argval[s]) for s in signatures)
        try:
            i = self.argvals.index(pattern)
            warnings.warn("Hash index founded. Stored object is used instead of creating a new one.")
            return self.objs[i]
        except ValueError:
            self.argvals += (pattern,)
            try:
                obj = self.func(*args,**dict(kwargs, **self.parse))
            except TypeError:
                obj = self.func(*args, **kwargs)
            self.objs += (obj,)
            return obj     
    def __call__(self, func):
        self.func = func
        self.vars_wrapper.__func__.__doc__ = self.func.__doc__
        return self.vars_wrapper

class Do:
    def __init__(self, do):
        self.do = do
    def vars_wrapper(self, *args, **kwargs):
        ret = self.func(*args, **kwargs)
        self.do(ret)
        return ret
    def __call__(self, func):
        self.func = func
        return self.vars_wrapper
