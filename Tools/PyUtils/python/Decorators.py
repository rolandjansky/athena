# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @author: Sebastien Binet <binet@cern.ch>
# @date:   March 2008
# @purpose: a set of decorators. Most of them (if not all) have been stolen
#           from here:
#           http://www.phyast.pitt.edu/~micheles/python/documentation.html
#
from __future__ import with_statement

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"

__all__ = [
    'memoize',
    'forking',
    'async',
    ]

from PyCmt.Decorators import *
