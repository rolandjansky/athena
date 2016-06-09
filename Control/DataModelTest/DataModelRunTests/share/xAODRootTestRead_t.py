#
# $Id$
#
# File: share/xAODRootTestRead_t.py
# Author: snyder@bnl.gov
# Date: Jun 2014
# Purpose: Test reading xAOD objects directly from root.
#

from __future__ import print_function
import ROOT
import cppyy

ROOT.xAOD.TReturnCode.enableFailure()


from AthenaCommon.Include import Include
include = Include(show = False)
include('DataModelRunTests/xAODRootTest.py')

xAODInit()
ana = Analysis('xaoddata.root', 'xaodroot.root')
ana.add (xAODTestRead())
ana.add (xAODTestCopy(writePrefix='copy_'))
ana.add (xAODTestDecor(decorName = 'dint2', offset=600))
#ana.add (xAODTestPDecor(decorName = 'dpint3', offset=700))
ana.run()
ana.finalize()
