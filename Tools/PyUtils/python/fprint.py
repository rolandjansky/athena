# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
##
# @file PyUtils/python/fprint.py
# @author sss
# @date Dec 2019
# @brief Print functions emulating python2 softspace rules.
#
# In python2, doing
#
#    print >>f, 'a',
#    print >>f, 'b'
#
# would insert a space between `a' and `b', while
#
#    print >>f, 'a',
#    print >>f, '\nb'
#
# would not put a space after the `a'.
#
# This was implemented by an `internal' attribute of file objects
# called `softspace'.  If non-zero, then the next print should
# emit a space at the start.
#
# This functionality is not present in the python 3 print function.
#
# In the case where one has existing python 2 output formatting where
# one wants to preserve the spacing and have it work consistently
# in both py2 and py3, the functions here may be used.
# Use fprint to print a string with no terminating newline, and
# use fprintln to print a string with a terminating newline.
# Calls to f.write() should be replaced with fwrite().
#
# In addition to spacing, py2 and py3 format floating-point numbers differently.
# These functions try to produce py2-default foramtting.


from __future__ import print_function
import string


def _formatFloat (x):
    """Format a value using py2 float formatting.

>>> print (_formatFloat ('asd'))
asd
>>> print (_formatFloat (1.234567801234567))
1.23456780123
>>> print (_formatFloat (2.0))
2.0
>>> print (_formatFloat (2e30))
2e+30
>>> print (_formatFloat (float('nan')))
nan
>>> print (_formatFloat (float('inf')))
inf
"""
    # No change if it's not a float.
    if not isinstance (x, float):
        return x
    # default formatting for floats in py2 was effectively the g format
    # with 12 significant figures --- except that the g format removes
    # a trailing `.0' while py2 preserves it.
    s = '%.12g' % x
    if s.find('.') < 0 and s.find('e') < 0 and s[-1] in string.digits:
        return s + '.0'
    return s


def _formatFloats (l):
    """Convert an argument list to use py2 formatting.
>>> print (_formatFloats ((1, 'asd', 3.0)))
(1, 'asd', '3.0')
"""
    return tuple ([_formatFloat(x) for x in l])


def fprint (f, *args):
    """Print a string with no terminating newline.

Compatible with the python 2 print statement.

>>> import sys
>>> fprint (sys.stdout, 'a'); fprint (sys.stdout, 'b')
a b
>>> fwrite (sys.stdout, '\\n')
<BLANKLINE>
>>> fprint (sys.stdout, 'a'); fprint (sys.stdout, 2.0, '\\n'); fprint (sys.stdout, 'b')
a 2.0 
b
"""
    if getattr (f, 'softspace', 0):
        f.write (' ')
    print (*_formatFloats (args), file=f, end='')
    if len(args) > 0 and isinstance (args[-1], str) and args[-1].endswith('\n'):
        f.softspace = 0
    else:
        f.softspace = 1
    return


def fprintln (f, *args):
    """Print a string with a terminating newline.

Compatible with the python 2 print statement.

>>> import sys
>>> sys.stdout.softspace = 0
>>> fprint (sys.stdout, 'a'); fprintln (sys.stdout, 'b'); fprintln (sys.stdout, 'c'); fprint (sys.stdout, 'd')
a b
c
d
"""
    if getattr (f, 'softspace', 0):
        f.write (' ')
    print (*_formatFloats(args), file=f)
    f.softspace = 0
    return


def fwrite (f, *args):
    """Write a string to a file.

Compatible with the python 2 print statement space handling.
>>> import sys
>>> sys.stdout.softspace = 0
>>> fprint (sys.stdout, 'a'); fwrite (sys.stdout, '\\n'); fprint (sys.stdout, 'c')
a
c
"""
    f.write (*args)
    f.softspace = 0
    return


if __name__ == "__main__":
    print ('PyUtils/fprint.py')              #pragma: NO COVER
    from PyUtils import coverage             #pragma: NO COVER
    c = coverage.Coverage ('PyUtils.fprint') #pragma: NO COVER
    c.doctest_cover()                        #pragma: NO COVER
