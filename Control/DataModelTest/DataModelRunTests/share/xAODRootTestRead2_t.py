#
# $Id$
#
# File: share/xAODRootTestRead2_t.py
# Author: snyder@bnl.gov
# Date: Jun 2014
# Purpose: Test reading xAOD objects directly from root.
#

from __future__ import print_function
import ROOT
import cppyy


from AthenaCommon.Include import Include
include = Include(show = False)
include('DataModelRunTests/xAODRootTest.py')

xAODInit()
ana = Analysis('xaodroot.root')
ana.add (xAODTestRead())
ana.add (xAODTestRead(readPrefix = 'copy_'))
ana.run()

