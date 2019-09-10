#
# $Id$
#
# File: share/xAODTestRead3MT_jo.py
# Author: snyder@bnl.gov
# Date: May 2019
# Purpose: Testing dependency propagation.
#

FILECATALOG = 'xAODTestRead3MT_catalog.xml'
include ('DataModelRunTests/xAODTestRead3_jo.py')

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
