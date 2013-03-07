# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import division

from logging import getLogger; log = getLogger("DQUtils.sugar")

from calendar import timegm

from datetime import datetime
date_from_timestamp = datetime.utcfromtimestamp

RUN_MAX = 2**31-1
LB_MAX  = 2**32-1
IOV_MAX = 2**63-1

class IovKey(long):
    __slots__ = ()

class RunLumiType(IovKey):
    """
    Syntactic sugar for an IoV key.
    
    This class intentionally has no constructor so that we can benefit from the
    speed of the builtin long constructor. (Since we use it a lot!)
    
    On the python side though, it is useful to have a constructor which takes 
    (Run, Lumiblock). This is provided by the make() class method, which is
    aliased as RunLumi.
    """
    __slots__ = ()
    
    is_time = False
    
    @classmethod
    def make(cls, *args):
        """
        Create a RunLumiType in a smarter (but slower) way, depending on whether 
        one or two args are passed.
        """
        if len(args) == 1:
            if isinstance(args[0], tuple):
                run, lumi = args[0]
                return cls(run << 32 | lumi)
            return cls(args[0])
        elif len(args) == 2:
            run, lumi = args
            return cls(run << 32 | lumi)
        raise RuntimeError("Invalid argument to RunLumiType.make")
    
    def __repr__(self):
        """
        Pretty representation for an IoV key
        """
        run, lumi = self.run, self.lumi
        if run == RUN_MAX: run = "[MAX]"
        if lumi == LB_MAX: lumi = "[MAX]"
        return "%5s:%5s" % (run, lumi)
    
    def __str__(self):
        return self.__repr__()
    
    def __add__(self, what):
        """ 
        How to add something to an LB: in case of a numeric type, do the normal
        thing. In case of a tuple, use the first element as a run number count
        and the second element as a luminosity block count.
        """
        if isinstance(what, tuple):
            nrun, nlumi = what
            return RunLumiType((self.run + nrun << 32) + (self.lumi + nlumi))
            
        return RunLumiType(long.__add__(self, what))
    
    def __sub__(self, what):
        return RunLumiType(long.__sub__(self, what))
    
    def start_nextrun(self):
        """
        Return a new RunLumiType with run number += 1 and lumi = 1
        """
        return self + (1, -self.lumi) + (0, 1)
    
    @property
    def run(self):
        return self >> 32
        
    @property
    def lumi(self):
        return self & 0xFFFFFFFF

class TimestampType(IovKey):
    __slots__ = ()
    is_time = True
    
    def __repr__(self):
        return str(self.date) + " UTC"
        
    def __str__(self):
        return self.__repr__()
    
    @property
    def date(self):
        if self == IOV_MAX:
            return "([MAX]:[MAX])"
        try:
            return date_from_timestamp(self/1e9)
        except ValueError:
            return "[InvalidDate]"
            
    @classmethod
    def from_date(cls, date):
        return cls((timegm(date.utctimetuple()) * 1000000000) 
                    + 1000*date.microsecond)

    @classmethod
    def from_string(cls, date_string):
        """
        Create a TimestampType IOVKey from a %d/%m/%Y string or %H:%M:%S
        """
        try:
            d = datetime.strptime(date_string, "%d/%m/%Y")
        except ValueError:
            try:
                d = datetime.strptime(date_string, "%d/%m/%Y %H:%M:%S")
            except ValueError:
                log.exception("Invalid")
                raise
            
        return cls.from_date(d)
    
RunLumi = RunLumiType.make
IOVMIN = RunLumiType(0)
IOVMAX = RunLumiType(2**63-1)
