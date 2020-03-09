#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: AthExStoreGateExample/share/HandleTest_jobOptions.py
# Author: scott snyder
# Date: Oct, 2019
# Brief: Test for cross-component circular dependency warning suppression
#        of WriteDecorHandleKey.
#

import AthenaCommon.AtlasUnixGeneratorJob
theApp.EvtMax = 1

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from AthExStoreGateExample.AthExStoreGateExampleConf import \
    AthEx__HandleTestAlg, AthEx__HandleTestTool1,  AthEx__HandleTestTool2

# This should not get a circular dependency warning.
topSeq += AthEx__HandleTestAlg ('testalg1')

# But this should.
topSeq += AthEx__HandleTestAlg ('testalg2',
                                Tool1 = AthEx__HandleTestTool2 ('testool2'),
                                Tool2 = AthEx__HandleTestTool1 ('testool1'))


