#
# $Id$
#
# File: xAODCaloEventCnv/share/ClusterCreator_test.py
# Author: snyder@bnl.gov
# Date: Nov 2015
# Purpose: Unit test for calo cluster aod -> xaod conversion.
#

import AthenaCommon.AtlasUnixStandardJob
import AthenaPoolCnvSvc.ReadAthenaPool

# Define input
import os
testdata = os.environ.get ('D3PDTESTDATA',
                           '/afs/cern.ch/atlas/maxidisk/d33/referencefiles')
svcMgr.EventSelector.InputCollections        = [ testdata + '/aod/AOD-17.2.9.1/AOD-17.2.9.1-full.pool.root' ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator, ClusterDumper
topSequence += ClusterCreator(AODContainerNames=["CaloCalTopoCluster"])
topSequence += ClusterDumper("TopoDumper",ContainerName="CaloCalTopoCluster",FileName="TopoCluster.txt")

def compare():
    import os
    reffile = 'TopoCluster.txt.bz2'
    refpath = '../test/%s' % reffile
    if not os.path.exists (refpath):
        refpath = '../../test/%s' % reffile
    os.system ('bzdiff -u %s %s' % ('TopoCluster.txt', refpath))
import atexit
atexit.register (compare)

