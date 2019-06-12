# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import findViewAlgs
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.CFElements import seqOR
import AthenaCommon.CfgMgr as CfgMgr

import unittest

class ViewCFTest( unittest.TestCase ):
    def runTest( self ):

        topSequence = AlgSequence()
        viewTestAlg1 = CfgMgr.AthViews__ViewTestAlg("viewTestAlg1")
        viewTestAlg2 = CfgMgr.AthViews__ViewTestAlg("viewTestAlg2")

        # Add an algorithm to a sequence
        topSequence += seqOR( "makeViewSequence" )
        topSequence.makeViewSequence += viewTestAlg1

        # Return the algorithm assuming it's in a view, or not
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {} ),
                          ( [ "viewTestAlg1" ], [] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"makeViewSequence":False} ),
                          ( [], [ "viewTestAlg1" ] ) )

        # Add a nested sequence
        topSequence.makeViewSequence += seqOR( "nestedSequence" )
        topSequence.makeViewSequence.nestedSequence += viewTestAlg2

        # Return the algorithms depending on where the view is entered
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {} ),
                          ( [ "viewTestAlg1", "viewTestAlg2" ], [] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"makeViewSequence":False} ),
                          ( [], [ "viewTestAlg1", "viewTestAlg2" ] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"nestedSequence":False} ),
                          ( [ "viewTestAlg1" ], [ "viewTestAlg2" ] ) )

        # Check that the test finds view nodes by name
        viewNodeDict = {"makeViewSequence":False, "aFakeNode":False}
        findViewAlgs( topSequence.getChildren(), viewNodeDict )
        self.assertEqual( viewNodeDict, {"makeViewSequence":True, "aFakeNode":False} )
