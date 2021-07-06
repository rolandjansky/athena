# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# @file D3PDMakerCoreComps/python/StackedDict.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Allow lookups in multiple dictionaries while writing to one.
#


import collections


class StackedDict (collections.Mapping):
    """Allow lookups in multiple dictionaries while writing to one.

    A StackedDict references a list of other dictionaries.
    Reads try each dictionary in sequence, succeeding with the first dictionary
    that contains the target key.  Modifications go to the last dictionary
    in the list.

    Notes: No attempt is made to deal with keys duplicated between dictionaries;
    len() and __iter__ will process them all.  __delitem__ is not implemented.

    Examples:
    >>> from pprint import pprint
    >>> d1 = {'a':'b'}
    >>> d2 = {'c':'d'}
    >>> d3 = {'e':'f'}
    >>> d = StackedDict (d1, d2, d3)
    >>> d['a']
    'b'
    >>> d['c']
    'd'
    >>> d['e']
    'f'
    >>> len(d)
    3
    >>> [x for x in d]
    ['a', 'c', 'e']
    >>> 'c' in d
    True
    >>> 'c' in d
    True
    >>> 'd' in d
    False
    >>> d['c'] = 10
    >>> d['c']
    10
    >>> d2
    {'c': 10}
    >>> d['x'] = 20
    >>> d['x']
    20
    >>> pprint(d3)
    {'e': 'f', 'x': 20}
    >>> pprint(d.copy())
    {'a': 'b', 'c': 10, 'e': 'f', 'x': 20}
"""

    def __init__ (self, *dicts):
        self.dicts = dicts
        return


    def __getitem__ (self, k):
        for d in self.dicts:
            if k in d:
                return d[k]
        raise KeyError (k)


    def __setitem__ (self, k, v):
        for d in self.dicts[:-1]:
            if k in d:
                d[k] = v
                return
        self.dicts[-1][k] = v
        return


    def __len__ (self):
        return sum([len(d) for d in self.dicts])


    def __iter__ (self):
        for d in self.dicts:
            for x in d: yield x
        return


    def has_key (self, k):
        """Some code still uses this.
        >>> d1 = {'a':'b'}
        >>> d = StackedDict (d1)
        >>> d.has_key ('a')
        True
        >>> d.has_key ('z')
        False
        """
        return k in self


    def copy (self):
        d = {}
        for i in range(len(self.dicts)-1, -1, -1):
            d.update(self.dicts[i])
        return d
        
