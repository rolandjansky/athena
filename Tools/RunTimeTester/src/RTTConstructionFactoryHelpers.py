# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Helper functions for RTT construction factories
"""
import string

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

def makeArgDict(inlist, obj):

    inlist = [a for a in inlist if a in obj.__dict__.keys()]
    outDict = {}
    for a in inlist: outDict[a] = obj.__dict__[a]
    return outDict
                
class ArgBag:
    def __init__(self, dict):
        self.__dict__.update(dict)
    def __str__(self):
        longest = 0
        for s in self.__dict__.keys():
            if len(s) > longest: longest = len(s)
        
        s = ''
        longest += 5
        keys =  self.__dict__.keys()
        keys.sort()

        for v in keys:
            vv = v+':'
            vv = string.ljust(v, longest)
            s += '%s%s\n' % (vv, self.__dict__[v])
        return s

    def pop(self, *args):
        assert(len(args) in range(1,3))
        if len(args) == 1:
            return self.__dict__.pop(args[0])
        return self.__dict__.pop(args[0], args[1])

    def duplicate(self):
        from copy import copy
        new_ = {}
        for k,v in self.__dict__.items():
            new_[copy(k)] = copy(v)
        return ArgBag(new_)

def makeArgBag(inlist, obj):

    missing = [a for a in inlist if a not in obj.__dict__.keys()]
    if missing:
        logger.info('makeArgBag Warning: cannot find %s' % str(missing))
        
    inlist = [a for a in inlist if a in obj.__dict__.keys()]
    outDict = {}
    for a in inlist: outDict[a] = obj.__dict__[a]
    bag = ArgBag(outDict)
    return bag
                

