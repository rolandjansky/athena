# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: combo.py,v 1.3 2005-05-12 16:36:59 ssnyder Exp $
# File: combo.py
# Created: sss, Mar 2005, from http://aspn.activestate.com/.
# Purpose: Generators for combinations and permutations.
#

"""Generators for combinations and permutations.

This module contains functions for
generating all combinations and permutations from a list of
items.  These functions return generators, so you may iterate
over them or retrieve successive values with
next().  Given a sequence items,
you can iterate over the list of unique pairwise combinations with

   for (i1, i2) in combo.xuniqueCombinations (items, 2):

The functions contained in this module are:

  combinations(items,n)
    Yields all unique subsequences of length n from items.

  all_combinations(items,n)
    Similar, but differences in ordering are considered significant.
    For example, all_combinations([1,2,3],2) will yield both [1,2] and [2,1].

  permutations(items)
    Yields all permutations of items.
    Equivalent to all_combinations(items,len(items)).

  selections(items,n)
    Returns all possible ways of picking n items from
    items, where any given item may be picked multiple times.
"""

__version__ = "1.0"

"""combo.py
Generators for calculating a) the permutations of a sequence and
b) the combinations and selections of a number of elements from a
sequence. Uses Python 2.2 generators.

Similar solutions found also in comp.lang.python

Keywords: generator, combination, permutation, selection

See also: http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/105962
See also: http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/66463
See also: http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/66465
"""

def all_combinations(items, n):
    """Return all subsequences of length n from items.
Differences in ordering are considered significant.
For example, all_combinations([1,2,3],2) will yield both [1,2] and [2,1].

>>> for c in all_combinations(['l','o','v','e'],2):
...   print ''.join(c)
lo
lv
le
ol
ov
oe
vl
vo
ve
el
eo
ev
"""
    if n==0: yield []
    else:
        for i in xrange(len(items)):
            for cc in all_combinations(items[:i]+items[i+1:],n-1):
                yield [items[i]]+cc

def combinations(items, n):
    """Yields all unique subsequences of length n from items.

>>> for uc in combinations(['l','o','v','e'],2):
...   print ''.join(uc)
lo
lv
le
ov
oe
ve
"""
    if n==0: yield []
    else:
        for i in xrange(len(items)):
            for cc in combinations(items[i+1:],n-1):
                yield [items[i]]+cc
            
def selections(items, n):
    """Returns all possible ways of picking n items from
items, where any given item may be picked multiple times.

>>> for s in selections(['l','o','v','e'],2):
...   print ''.join(s)
ll
lo
lv
le
ol
oo
ov
oe
vl
vo
vv
ve
el
eo
ev
ee
"""
    if n==0: yield []
    else:
        for i in xrange(len(items)):
            for ss in selections(items, n-1):
                yield [items[i]]+ss

def permutations(items):
    """ Yields all permutations of items.
Equivalent to all_combinations(items,len(items)).

>>> for p in permutations(['l','o','v','e']):
...   print ''.join(p)
love
loev
lvoe
lveo
leov
levo
olve
olev
ovle
ovel
oelv
oevl
vloe
vleo
vole
voel
velo
veol
elov
elvo
eolv
eovl
evlo
evol
"""
    return all_combinations(items, len(items))


__test__ = {}
__test__['tests'] = """
>>> # Permutations of 'love'
>>> print map(''.join, list(permutations('done')))
['done', 'doen', 'dnoe', 'dneo', 'deon', 'deno', 'odne', 'oden', 'onde', 'oned', 'oedn', 'oend', 'ndoe', 'ndeo', 'node', 'noed', 'nedo', 'neod', 'edon', 'edno', 'eodn', 'eond', 'endo', 'enod']
"""


if __name__ == "__main__":
    import doctest
    doctest.testmod()
