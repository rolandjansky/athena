# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import division

from collections import namedtuple

from ..sugar import IOVMIN, IOVMAX, RunLumi

def restore_iov_type(name, fields, bases, content, empty, _memoized={}):
    """
    This would be better as a class method, but it's not possible to pickle 
    those, hence why this exists.
    _memoized is used to store the type to avoid a function call to rebuild the
    type un-necessarily
    """
    args = name, fields, bases, empty
    if args not in _memoized: 
        # Don't take the last argument
        iov_type = make_iov_type(*args[:-1])
        _memoized[args] = iov_type._emptycls if empty else iov_type 
    return _memoized[args]._make(content)
        
class IOVType(object):
    """
    A type which encapsulates an IoV
    """
    __slots__ = ()
    _is_empty = False
    
        
    def __reduce__(self):
        """
        Make IOVType pickleable
        """
        # Take all but the first base class, which is unpicklable
        bases = type(self).__bases__[1:] 
        name = type(self).__name__
        args = name, self._fields[2:], bases, tuple(self), self._is_empty
        return restore_iov_type, args
        
    @classmethod
    def empty(cls, since=None, until=None):
        empty_fields = [since, until] + [None] * (len(cls._fields)-2)
        return cls._emptycls(*empty_fields)
    
    @property
    def runs(self):
        """
        Return a generator of runs
        """
        if self.until == IOVMAX:
            return [self.since.run]
        return xrange(self.since.run, self.until.run+1)

    @property
    def run(self):
        """
        Returns the run the IoV started in
        """
        return self.since.run
    
    def contains_point(self, iovkey):
        return self.since <= iovkey < self.until
    
    def intersect(self, iov):
        if not self.intersects(iov):
            return None
        since, until = max(self.since, iov.since), min(self.until, iov.until)
        if since == until:
            return None
        return self._replace(since=since, until=until)
    
    def intersects(self, iov):
        return self.since < iov.until and self.until >= iov.since
        
    @property
    def length(self):
        return self.until - self.since if self.until != IOVMAX else 0
        
    def connected_to(self, rhs):
        return self.until == rhs.since and self._contents == rhs._contents
    
    @property
    def _contents(self):
        return self[2+self._has_insertion_time:]
    
    def merge(self, rhs):
        """
        Assuming this and `rhs` are `connected_to` each other, then extend.
        """
        return self._replace(until=rhs.until)
    
    @property
    def trimmed(self):
        """
        If this IoV starts on lumiblock 0, instead make it start from 1.
        """
        if self.since.lumi == 0:
            return self._replace(since=self.since+1)
        return self
        
    @property
    def is_time_based(self):
        return self.since.is_time

    def intersect_run(self, run_number):
        runpart = run_number << 32
        sincelo, sincehi = RunLumi(runpart | 1), RunLumi(runpart | 0xFFFFFFFF)
        return self._replace(since=max(self.since, sincelo),
                              until=min(self.until, sincehi))

def make_iov_type(name, variables, bases=(IOVType,), _memoized={}):
    """
    Create an IOV type and its empty type
    """
    args = name, tuple(variables), bases
    if args in _memoized:
        return _memoized[args]
    
    has_insertion_time = "insertion_time" in variables
    has_channel = "channel" in variables
    all_fields = " ".join(["since", "until"] + list(variables))
    
    class cls(namedtuple(name, all_fields)):
        "Named tuple to store IoV types"
    
    cls.__name__ = name
    cls.__bases__ += bases
    cls._has_channel = has_channel
    cls._has_insertion_time = has_insertion_time
    
    from new import classobj
    cls._emptycls = classobj(name + "_EMPTY", (cls,), dict(
        _is_empty=True,
        __nonzero__ = lambda self: False,
    ))   
    
    _memoized[args] = cls
    return cls

def define_iov_type(func):
    """
    A decorator which creates a lightweight iov type.
    """
    co_code = func.__code__ if hasattr(func, "__code__") else func.func_code
    name = func.__name__
    variables = co_code.co_varnames
    
    return make_iov_type(name, variables)

@define_iov_type
def RANGEIOV_VAL():
    "Just store a since, until"
