# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: interpolate.py,v 1.1 2005-05-06 22:44:58 ssnyder Exp $
# File: interpolate.py
# Created: sss, 2004.
# Purpose: Polynomial interpolation in a table.
#

"""
Polynomial interpolation in a table.
This is a python version of CaloClusterCorrection/interpolate.
"""

# Helper: do a binary search in a table.
# Note: these are slightly different than the standard python definition
# of bisect, in that the comparison is done with <= rather than <.
# This results in different behavior for the case that a key exactly
# matching x is present in the table.  As it's done here, it matches
# the result of the C++ lower_bound function, used in the C++ implementation.
def _bisect (a, x, lo=0, hi=None):
    if hi is None:
        hi = len(a)
    while lo < hi:
        mid = (lo+hi)//2
        if cmp(x,a[mid]) <= 0: hi = mid
        else: lo = mid+1
    return lo

# Helper: do a binary search in a 2D table.
def _bisect2 (a, x, xcol, lo=0, hi=None):
    if hi is None:
        hi = len(a)
    while lo < hi:
        mid = (lo+hi)//2
        if cmp(x,a[mid][xcol]) <= 0: hi = mid
        else: lo = mid+1
    return lo


#
#  Polynomial interpolation in a table.
#
def interpolate (points, x, degree, ycol=1, regions=[], xcol=0):
    """Polynomial interpolation in a table.
    
  points   Interpolation table.
           The x values are in the first column (column 0),
           and by default, the y values are in the second column (column 1).
           The x values must be in ascending order, with no duplicates.
  x        The value to interpolate.
  degree   The degree of the interpolating polynomial.
  ycol     The column number of the y values.  (0-based.)
  regions  Sometimes, you want to divide the range being interpolated
           into several distinct regions, where the interpolated
           function may be discontinuous at the boundaries
           between regions.  To do this, supply this argument,
           which should contain a list (in ascending order)
           of the x-coordinates of the region boundaries.
           When the interpolation runs against a boundary,
           the algorithm will add a copy of the last point
           before the boundary, positioned at the boundary.
           This helps to control runaway extrapolation
           leading up to the boundary.
  Returns the interpolated value.
 
  The method used is Newtonian interpolation.
  Based on the cernlib routine divdif.
  """
    if len (points) < 2 or degree < 1:
        raise 'bad args!'
    degree = min (degree, len (points) - 1)
    

    # START.  FIND SUBSCRIPT IX OF X IN ARRAY.
    #ix = _bisect (points, x, lambda x, p, xcol=xcol: cmp (x, p[xcol]))# - 1
    ix = _bisect2 (points, x, xcol)

    ir = _bisect (regions, x)

    # Number of points to try for.
    # Either degree+1 or degree+2, whichever is even,
    # to give the same number of points on each side.
    # If we run up against an edge or a boundary, we'll
    # fall back to using just degree+1 points (or fewer if we can't
    # even get that many).
    # If we end up using degree+2 points, we'll do two interpolations
    # of degree degree and average them.
    npts = degree + 2 - (degree%2)
    l = 0
    t = []
    d = []

    # If we run up against the edge of a region boundary,
    # we'll want to add a psuedopoint right at the boundary
    # (copying the point closest to the boundary) instead of the
    # point farthest away from it.
    extralo = 0
    extrahi = 0
    
    # Starting point index, not considering edges or boundaries.
    ilo = ix - npts/2

    # Make sure this point is within the array range and has not
    # crossed a region boundary.
    if ilo < 0:
        ilo = 0
        npts = degree+1
    while ilo < len (points) and ir>0 and points[ilo][xcol] < regions[ir-1]:
        ilo += 1
        npts = degree+1
        extralo = 1

    # Same deal for the right hand edge.
    # ihi is one past the last point to use.
    himoved = 0
    ihi = ilo + npts
    if ihi > len (points):
        ihi = len (points)
        npts = degree+1
        himoved = 1
    while ihi > 0 and ir<len(regions) and points[ihi-1][xcol] >= regions[ir]:
        ihi -= 1
        npts = degree+1
        extrahi = 1
        himoved = 1

    lomoved = 0
    ilo = ihi - npts
    if ilo < 0:
        ilo = 0
        lomoved = 1
    while ilo < len (points) and ir>0 and points[ilo][xcol] < regions[ir-1]:
        ilo += 1
        extralo = 1
        lomoved = 1

    npts = ihi - ilo
    t = []
    d = []

    if extralo and points[ilo][xcol] != regions[ir-1]:
        if not himoved:
            ihi -= 1
        else:
            npts += 1
        t.append (regions[ir-1])
        d.append (points[ilo][ycol])

    if extrahi and points[ihi-1][xcol] != regions[ir]:
        if not lomoved:
            ilo += 1
        else:
            npts += 1
        t.append (regions[ir])
        d.append (points[ihi-1][ycol])

    t += [points[i][xcol] for i in range (ilo, ihi)]
    d += [points[i][ycol] for i in range (ilo, ihi)]

    degree = min (degree, npts-1)
    extra = npts != degree+1

    if extra:
        (t[0], t[npts-2]) = (t[npts-2], t[0])
        (d[0], d[npts-2]) = (d[npts-2], d[0])

    # REPLACE D BY THE LEADING DIAGONAL OF A DIVIDED-DIFFERENCE TABLE,
    # SUPPLEMENTED BY AN EXTRA LINE IF *EXTRA* IS TRUE.
    for l in range(0, degree):
        if extra:
            d[degree+1] = (d[degree+1]-d[degree-1])/(t[degree+1]-t[degree-1-l])
        for i in range (degree, l, -1):
            d[i] = (d[i]-d[i-1])/(t[i]-t[i-1-l])

    # EVALUATE THE NEWTON INTERPOLATION FORMULA AT X, AVERAGING TWO VALUES
    # OF LAST DIFFERENCE IF *EXTRA* IS TRUE.
    sum = d[degree]
    if extra: sum=0.5*(sum+d[degree+1])
    for j in range (degree-1, -1, -1):
        sum = d[j] + (x - t[j]) * sum

    return sum


__test__ = {}
__test__['tests'] = """
>>> def is_different (a, b):
...  den = abs(a) + abs(b)
...  if den == 0: return 0
...  return abs((a-b)/den) > 1e-6
>>> def testit (y2, table, x, degree, regions=[], ycol=1):
...   y1 = interpolate (table, x, degree, ycol, regions)
...   if is_different(y1, y2):
...     return "diff %f %f %f" % (x, y1, y2)
>>> table = [
...  [0.05,   6.264762e-01],
...  [0.15,   6.671484e-01],
...  [0.25,   7.134157e-01],
...  [0.35,   8.211740e-01],
...  [0.45,   9.047978e-01],
...  [0.55,   9.087084e-01],
...  [0.65,   8.923957e-01],
...  [0.75,   9.584507e-01],
...  [0.85,  -5.659607e-01],
...  [0.95,  -6.995252e-01],
...  [1.05,  -8.711259e-01],
...  [1.15,  -1.038724e+00],
...  [1.25,  -1.294152e+00],
...  [1.35,  -1.170352e+00],
...  [1.45,  -9.312256e-01],
...  ]
>>> testit ( 0.912999,  table, 0.5,  3);
>>> testit ( 0.590771,  table, 0.0,  3);
>>> testit (-0.871126,  table, 1.05, 3);
>>> testit (-1.0011166, table, 1.13, 3);
>>> testit (-1.04871,   table, 1.4 , 3);
>>> testit (-9.34768,   table, 2.0 , 3);
>>>
>>> testit ( 0.91485268, table, 0.5,  4);
>>> testit ( 0.55207348, table, 0.0,  4);
>>> testit (-0.87112588, table, 1.05, 4);
>>> testit (-0.99805647, table, 1.13, 4);
>>> testit (-1.0201578,  table, 1.4 , 4);
>>> testit (-78.760239,  table, 2.0 , 4);
>>>
>>> regions = [0.8, 1.05]
>>> testit ( 0.65513462, table, 0.1,  4, regions);
>>> testit ( 0.92242599, table, 0.7,  4, regions);
>>> testit (-0.88620204, table, 1.1,  4, regions);
>>> testit (-0.73959452, table, 1.0,  4, regions);
>>> testit (-0.69952518, table, 1.05, 4, regions);
>>> testit ( 0.95845068, table, 0.8,  4, regions);
>>> testit (-1.2802936,  table, 1.3,  4, regions);
>>>
>>> testit ( 0.64960641, table, 0.1,  3, regions);
>>> testit ( 0.92791027, table, 0.7,  3, regions);
>>> testit (-0.91475517, table, 1.1,  3, regions);
>>> testit (-0.73959452, table, 1.0,  3, regions);
>>> testit (-0.69952518, table, 1.05, 3, regions);
>>> testit ( 0.95845068, table, 0.8,  3, regions);
>>> testit (-1.2631618,  table, 1.3,  3, regions);
>>>
>>> testit (0.1, table, 0.1,  3, regions, 0);
>>>
>>> regions2 = [0.2, 1.2]
>>> testit ( 0.65359104, table, 0.1,  4, regions2);
>>> testit (-1.2503984,  table, 1.3,  4, regions2);
"""


if __name__ == "__main__":
    import doctest
    doctest.testmod()
