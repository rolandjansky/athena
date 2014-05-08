# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: rtime.py,v 1.1 2005-05-06 22:44:59 ssnyder Exp $
# File: normphi.py
# Created: sss, 2004.
# Purpose: Measure the execution time of a function.
#

import resource


def rtime (fn, *args, **kw):
    """Call FN with the additional arguments passed in.
Print a report of the CPU time taken.
"""
    ru0 = resource.getrusage (resource.RUSAGE_SELF)
    ret = None
    try:
        ret = fn (*args, **kw)
    finally:
        ru1 = resource.getrusage (resource.RUSAGE_SELF)
        print "utime: %f, stime: %f" % (ru1.ru_utime-ru0.ru_utime,
                                        ru1.ru_stime-ru0.ru_stime)
    return ret

    
