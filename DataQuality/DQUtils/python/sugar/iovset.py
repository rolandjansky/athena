# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import division

from collections import defaultdict
from pickle import load, dump, loads
from sys import stdout

from ..general import interleave
from ..utils import pprint_objects

from .runlumi import RunLumi
from .iovtype import RANGEIOV_VAL, make_iov_type


def restore_iovset(cls, name, variables, elements):
    """
    Rebuild a pickled IOVSet
    """
    iov_type = make_iov_type(name, variables)
    return cls(map(iov_type._make, elements))

class IOVSet(list):
    def __init__(self, *args, **kwargs):
        self.iov_type = kwargs.pop("iov_type", None)
        self.origin = kwargs.pop("origin", None)
        self.parent = kwargs.pop("parent", None)
        from sys import _getframe
        f = _getframe(1)
        self.constructed_at = "%s:%i" % (f.f_code.co_filename, f.f_lineno)
        super(IOVSet, self).__init__(*args, **kwargs)
        if self.iov_type is None and self:
            self.iov_type = type(self.first)
    
    def logical_not(self):
        from ..events import process_iovs
        cls = type(self)
        alliov = cls([RANGEIOV_VAL(RunLumi(0), RunLumi(0xFFFFFFFFFFFFFFFF))])
        events = process_iovs(alliov, self)
        return cls(RANGEIOV_VAL(since, until) 
                   for since, until, (_, state) in events if not state)
    
    @classmethod
    def logical_or(cls, *rhs_iovsets):
        """
        Gives the IOV ranges which are present in both this IOVSet and `rhs`
        """
        # Import goes here to prevent circular imports
        from ..events import process_iovs
        result = cls()
        for since, until, iovsets in process_iovs(*rhs_iovsets):
            if any(iovsets):
                result.add(since, until)
        return result.solidify(RANGEIOV_VAL)
        
    @classmethod
    def logical_and(cls, *rhs_iovsets):
        """
        Gives the IOV ranges which are present in both this IOVSet and `rhs`
        """
        # Import goes here to prevent circular imports
        from ..events import process_iovs
        result = cls()
        for since, until, iovsets in process_iovs(*rhs_iovsets):
            if all(iovsets):
                result.add(since, until)
        return result.solidify(RANGEIOV_VAL)
        
    def __or__(self, rhs):
        # Note: classmethod
        return self.logical_or(self, rhs)
        
    def __and__(self, rhs):
        # Note: classmethod
        return self.logical_and(self, rhs)
    
    @classmethod
    def from_iovsets(cls, iovsets):
        """
        Construct one IOVSet from many iovsets
        """
        iovsets = list(iovsets)
        if not iovsets:
            return cls()
        types = set(iovset.iov_type for iovset in iovsets)
        assert len(types) == 1
        origins = [i.origin for i in iovsets]
        parents = [i.parent for i in iovsets]
        iov_gen = (iov for iovset in iovsets for iov in iovset)
        return cls(iov_gen, iov_type=types.pop(), origin=origins, parent=parents)
    
    def connect_adjacent(self):
        from ..iov_arrangement import connect_adjacent_iovs
        return connect_adjacent_iovs(self)
    
    def pprint(self, where=stdout):
        pprint_objects(self, where)
    
    @property
    def ordered(self):
        prev_until = None
        for iov in self:
            if iov.since > iov.until:
                return False
            if prev_until is not None and iov.since < prev_until:
                return False
            prev_until = iov.until
        return True
    
    def to_file(self, filename):
        with open(filename, "wb") as fd:
            dump(self, fd)
    
    @classmethod
    def from_file(cls, filename):
        with open(filename, "rb") as fd:
            if filename.endswith(".bz2"):
                return loads(fd.read().decode("bz2"))
            return load(fd)
    
    @classmethod
    def from_grl_string(cls, data):
        from ..grl import load_grl_string
        return load_grl_string(data, IOVSet_class=cls)
    
    @classmethod
    def from_grl(cls, filename):
        with open(filename, "rb") as fd:
            return cls.from_grl_string(fd.read())
    
    def to_grl_string(self, name="unknown", version="unknown"):
        from ..grl import make_grl
        return make_grl(self, name, version)
        
    def to_grl(self, filename, name="unknown", version="unknown"):
        with open(filename, "w") as fd:
            fd.write(self.to_grl_string(name, version))
    
    @classmethod
    def from_runs(cls, runs):
        return IOVSet(RANGEIOV_VAL(RunLumi(r, 0), RunLumi(r+1, 0)-1)
                      for r in sorted(runs))
    
    @property
    def chans_iovsets(self):
        """
        Returns ([channel1, c2, c3, ...], [channel 1 iovs, c2iovs, c3iovs, ...])
        """
        if not self:
            return [], []        
        return zip(*sorted(self.by_channel.items()))
            
    def empty(self, content=None):
        """
        Return an empty IOVSet, but keeping any metadata around. (Such as the
        IOVType we were holding..)  
        """
        return self.empty_maker()([] if content is None else content)
    
    def empty_maker(self):
        """
        Indirection is used here in order to create new empty instances on
        demand without keeping a reference to the original object alive.
        """
        iov_type = self.iov_type
        origin = self.origin
        def empty(content=None):
            return type(self)([] if content is None else content, 
                               iov_type=iov_type, origin=origin, parent=self)
        return empty
    
    def __reduce__(self):
        """
        Make an IOVSet pickleable (beware limitations)
        """
        types = set(type(x) for x in self)
        if not types:
            return IOVSet, ()
        
        assert len(types) == 1, (
            "Can only pickle IOVSets with only one type (got types=%r)" % types)
        
        iov_type = type(self.first)
        name = iov_type.__name__
        variables = iov_type._fields[2:]
        
        return restore_iovset, (type(self), name, variables, map(tuple, self))
    
    def __hash__(self):
        return hash(tuple(self))
        
    def __repr__(self):
        if self.time_based:
            args = (len(self), self.duration, len(self.channels), 
                    ("%.2f" % (self.duration / len(self.channels)))
                     if self.channels else "N/A")
            plurals = ["" if x == 1 else "s" for x in args]
            args = tuple(interleave(args, plurals))
            return ("<%i IoV%s %.2f hour%s %i channel%s (avg=%s hr%s/c)>" 
                    % args)
            
        args = len(self), self.lb_counts, len(self.runs), len(self.channels)
        plurals = ["" if x == 1 else "s" for x in args]
        args = tuple(interleave(args, plurals))
        return "<%i IoV%s %i lb%s %i run%s %i channel%s>" % args
    
    def add(self, since, until, *args):
        args = list(args)
        if self and self[-1][1] == since and self[-1][2:] == args:
            # Last until == since and the rest of the thing matches
            #self[-1] = self[-1]._replace(until=until)
            self[-1][1] = until
        else:
            self.append([since, until] + args)
    
    def solidify(self, iov_type):
        """
        Because mutating lists is faster, once building an IOVSet is complete,
        it should be 'solidified' into the desired IOV type
        """
        self.iov_type = type
        return IOVSet(map(iov_type._make, self))
    
    @property
    def trim_iovs(self):
        """
        Ensure all IoVs start on lumiblock 1.
        """
        return self.empty(iov.trimmed for iov in self)
        
    def add_old(self, iov):
        """
        Extends this result set by one iov. If it is adjacent (and equal)
        to the previous IoV, then that IoV is extended instead.
        
        This was deprecated because it is too slow compared to add() and solidify().
        """
        if self and self.last.connected_to(iov):
            self[-1] = self[-1].merge(iov)
        else:
            self.append(iov)
                
    @property
    def time_based(self):
        """
        Looks at the first IoV to see if it is time based
        """
        if not self:
            return False
        try:
            return self.first.is_time_based
        except AttributeError:
            return False
    
    @property
    def lb_counts(self):
        """
        Sum of the LB counts
        """
        return sum(iov.length for iov in self)
    
    @property
    def duration(self):
        """
        Duration of the sum of the time-based IoVs, in hours.
        """
        return self.lb_counts / 1e9 / 3600
            
    def __getslice__(self, *args):
        """
        Slicing an IOVSet should return an IOVSet
        """
        return self.__class__(super(IOVSet, self).__getslice__(*args))
    
    @property
    def runs(self):
        """
        The set of runs present in this IOVSet
        """
        return set(iov.since.run for iov in self)
        
    @property
    def by_run(self):
        """
        Return a dictionary representing {run : iovs in run}
        """
        result = defaultdict(IOVSet)
        for iov in self:
            result[iov.run].append(iov)
        return dict(result)
        
    @property
    def channels(self):
        """
        The set of channels this IOVSet represents
        """
        if not self or not hasattr(self[0], "channel"): 
            return set()
        return set(iov.channel for iov in self)
        
    @property
    def by_channel(self):
        """
        Return a dictionary representing {channels : iovs with channelid}
        """
        # Import here to prevent circular imports
        from ..iov_arrangement import split_by_channel
        return split_by_channel(self)
        
    @property
    def first(self):
        "The first IoV in the set"
        assert self, (".first used in an empty IoV set. Is the range you are "
                       "trying to query valid?")
        return self[0]
    
    @property
    def last(self):
        "The last IoV in the set"
        assert self, (".last used in an empty IoV set. Is the range you are "
                       "trying to query valid?")
        return self[-1]
    
    @property
    def range_iov(self):
        """
        Returns an IoV object which represents the maximum range contained by
        the IoVs.
        """
        since = min(i.since for i in self)
        until = max(i.until for i in self)
        return RANGEIOV_VAL(since, until)
    
    def intersect_range(self, iov_range):
        """
        Returns a new IOVSet intersected with `iov_range` (since, until)
        """
        since, until = iov_range
        iov_range = RANGEIOV_VAL(RunLumi(since), RunLumi(until))
        
        intersected = (iov.intersect(iov_range) for iov in self)
        return self.empty(iov for iov in intersected if iov)
    
    def select_runs(self, *selected):
        """
        Pick IoVs which are in the set of runs `selected`
        """
        selected = set(selected)
        return self.empty(iov for iov in self
                          if any(run in selected for run in iov.runs))
                          
    def select_channels(self, *selected):
        """
        Pick IoVs which are in the set of channels `selected`
        """
        selected = set(selected)
        return self.empty(iov for iov in self if iov.channel in selected)

    def select(self, **what):
        def selected(iov):
            return all(getattr(iov, key) == value 
                       for key, value in what.iteritems())
                       
        return self.empty(iov for iov in self if selected(iov))
        

