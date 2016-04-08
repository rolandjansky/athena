# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
# File: root_pickle_t.py
# Created: Oct, 2012
# Purpose: Regression tests for root_pickle.py
#


def _regr_basic():
    """Very basic root_pickle test.
    >>> import ROOT
    >>> from PyAnalysisUtils import root_pickle
    >>> h1 = ROOT.TH1F ('h1', 'h1', 10, 0, 10)
    >>> h2 = ROOT.TH1F ('h2', 'h2', 10, 0, 10)
    >>> l1 = [h1, h2]
    >>> root_pickle.dump_root (l1, 'test.root')
    >>> l2 = root_pickle.load_root ('test.root')
    >>> assert len(l2) == 2
    >>> print [h.GetName() for h in l2]
    ['h1', 'h2']
    >>> import os
    >>> os.remove ('test.root')
"""
    


#from PyUtils import coverage
#c = coverage.Coverage ('PyAnalysisUtils.pydraw')
#c.doctest_cover ()

import doctest
print ''
doctest.testmod()

