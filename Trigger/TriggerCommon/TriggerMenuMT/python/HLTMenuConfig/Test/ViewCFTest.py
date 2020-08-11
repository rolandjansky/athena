# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.CFValidation import findViewAlgs, checkVDV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.CFElements import seqOR
import AthenaCommon.CfgMgr as CfgMgr
import six

import unittest

class ViewCFTest( unittest.TestCase ):
    if six.PY2:
        assertRaisesRegex = unittest.TestCase.assertRaisesRegexp        

    def runTest( self ):

        topSequence = AlgSequence()
        evca1 = CfgMgr.EventViewCreatorAlgorithm("evca1")
        evca2 = CfgMgr.EventViewCreatorAlgorithm("evca2")
        vdv1 = CfgMgr.AthViews__ViewDataVerifier("vdv1")
        vdv2 = CfgMgr.AthViews__ViewDataVerifier("vdv2")

        # Test error for empty sequence
        topSequence += seqOR( "makeViewSequence" )
        with self.assertRaisesRegex( RuntimeError, "Please remove makeViewSequence" ):
            findViewAlgs( topSequence.getChildren(), {} )

        # Add an algorithm to the sequence
        topSequence.makeViewSequence += evca1
        #topSequence.makeViewSequence += evca2

        # Return the algorithm assuming it's in a view, or not
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {} ),
                          ( [ "evca1" ], [] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"makeViewSequence":False} ),
                          ( [], [ "evca1" ] ) )

        # Add a nested sequence
        topSequence.makeViewSequence += seqOR( "viewSequence" )
        topSequence.makeViewSequence.viewSequence += vdv1

        # Return the algorithms depending on where the view is entered
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {} ),
                          ( [ "evca1", "vdv1" ], [] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"makeViewSequence":False} ),
                          ( [], [ "evca1", "vdv1" ] ) )
        self.assertEqual( findViewAlgs( topSequence.getChildren(), {"viewSequence":False} ),
                          ( [ "evca1" ], [ "vdv1" ] ) )

        # Check that the test finds view nodes by name
        viewNodeDict = {"makeViewSequence":False, "aFakeNode":False}
        findViewAlgs( topSequence.getChildren(), viewNodeDict )
        self.assertEqual( viewNodeDict, {"makeViewSequence":True, "aFakeNode":False} )

        # Check misconfigured EVCA
        evca1.ViewNodeName = "aFakeNode"

        with self.assertRaisesRegex( RuntimeError, "no corresponding upstream EventViewCreatorAlgorithm" ):
            checkVDV( topSequence, [topSequence.name()], {} )
        evca1.ViewNodeName = "viewSequence"
        with self.assertRaisesRegex( RuntimeError, "no external data" ):
            checkVDV( topSequence, [topSequence.name()], {} )
        evca1.RequireParentView = True
        checkVDV( topSequence, [topSequence.name()], {} )

        # Check for nested view CF working
        topSequence.makeViewSequence.viewSequence += seqOR( "nestedSequence" )
        topSequence.makeViewSequence.viewSequence.nestedSequence += vdv2
        checkVDV( topSequence, [topSequence.name()], {} )

        # Check duplicate EVCA config
        evca2.ViewNodeName = "aFakeNode"
        topSequence.makeViewSequence += evca2
        checkVDV( topSequence, [topSequence.name()], {} )
        evca2.ViewNodeName = "viewSequence"
        with self.assertRaisesRegex( RuntimeError, "Found duplicate view node name" ):
            checkVDV( topSequence, [topSequence.name()], {} )

