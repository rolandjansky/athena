# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
  A set of config-time tests to be run on the HLT top sequence

"""

from AthenaCommon.CFElements import isSequence
from TriggerJobOpts.TriggerConfig import collectViewMakers
    
def findViewAlgs( inputNodes, viewNodes ):
    """ Make lists of algorithms that run in views, and those that don't """

    allAlgs = []
    viewAlgs = []

    # Examine all nodes
    for node in inputNodes:

        # If node is a sequence, explore further
        if isSequence( node ):

            # Empty nodes can cause Gaudi scheduler bug https://gitlab.cern.ch/gaudi/Gaudi/-/issues/135
            if len( node.getChildren() ) == 0 and ( hasattr( node, "ModeOR" ) and node.ModeOR ):

              raise RuntimeError( "Empty CF sequence in OR mode can cause Gaudi scheduler bug. Please remove " + node.getName() )

            # Identify view CF nodes
            if node.getName() in viewNodes.keys():

                # Retrieve view algorithms
                # (views don't nest, so will be returned in first list)
                newViewAlgs, dummy = findViewAlgs( node.getChildren(), {} )
                viewAlgs += newViewAlgs

                # Record the fact that the view node is found
                viewNodes[ node.getName() ] = True

            # Explore the tree
            else:
                newAlgs, newViewAlgs = findViewAlgs( node.getChildren(), viewNodes )
                allAlgs += newAlgs
                viewAlgs += newViewAlgs
        
        # Node is an algorithm
        else:
            allAlgs += [ node.getName() ]

    return allAlgs, viewAlgs


def checkVDV( inputNodes, ancestorNames, allEVCAs ):
    """ Try to make sure each VDV has a correctly-configred EVCA upstream """

    for node in inputNodes:

        # Node is a VDV
        if "AthViews__ViewDataVerifier" in type( node ).__name__:

            # Check that VDV has a corresponding, correctly-configured EVCA
            foundEVCA = False
            for name in ancestorNames:

                if name in allEVCAs.keys():

                    # Check EVCA properties
                    parentAllowed = hasattr( allEVCAs[ name ], "RequireParentView" ) and allEVCAs[ name ].RequireParentView
                    eventLevelAllowed = hasattr( allEVCAs[ name ], "ViewFallThrough" ) and allEVCAs[ name ].ViewFallThrough
                    if not ( parentAllowed or eventLevelAllowed ):
                        raise RuntimeError( "ViewDataVerifier alg " + node.name() + " has upstream EventViewCreatorAlgorithm " + allEVCAs[ name ].name() + " with no external data" )
                    foundEVCA = True
                    break

            if not foundEVCA:
                raise RuntimeError( "ViewDataVerifier alg " + node.name() + " has no corresponding upstream EventViewCreatorAlgorithm" )

        # Node is an EVCA
        if "EventViewCreatorAlgorithm" in type( node ).__name__:

            # Store EVCA in a dictionary by the node it refers to
            if node.ViewNodeName in allEVCAs.keys():
                if node.name() != allEVCAs[ node.ViewNodeName ].name():
                    raise RuntimeError( "Found duplicate view node name " + node.ViewNodeName + " configured for EVCAs " + node.name() + " and " + allEVCAs[ node.ViewNodeName ].name() )
            allEVCAs[ node.ViewNodeName ] = node

            if not hasattr(node, "RoITool"):
                raise RuntimeError( "Node name " + node.name() + " was not supplied with a RoITool" )

        # Explore nested CF
        if isSequence( node ):

            checkVDV( node.getChildren(), ancestorNames + [node.name()], allEVCAs )


def testHLTTree( inputSequence ):
    """ Run all config tests """

    viewMakers = collectViewMakers( inputSequence )
    
    # List all CF nodes to be used with EventViews (and whether they are found)
    viewNodes = {}
    for viewMaker in viewMakers:
      viewNodes[ viewMaker.ViewNodeName ] = False
    originalLength = len( viewNodes )

    # Identify the algorithms that will run in EventViews
    wholeEventAlgs, viewAlgs = findViewAlgs( inputSequence.getChildren(), viewNodes )

    # Check that all view nodes are found
    if len( viewNodes ) != originalLength:
        raise RuntimeError( "Something went wrong with view config inspection" )
    for viewNode in viewNodes.keys():
        if not viewNodes[ viewNode ]:
            raise RuntimeError( "EventView CF node " + viewNode + " was not found attached to the test sequence" )

    # Look for view algs in the whole event context
    for viewAlgName in viewAlgs:
        if viewAlgName in wholeEventAlgs:
            from AthenaCommon.AlgSequence import dumpSequence
            dumpSequence( inputSequence )
            raise RuntimeError( viewAlgName + " is attached to an EventView node, but also runs in the whole event context" )

    # Make sure that VDVs are configured correctly
    allEVCAs = {}
    checkVDV( inputSequence.getChildren(), [inputSequence.name()], allEVCAs )

    # Check for inconsistent view maker search results
    if len( allEVCAs ) != originalLength:
      raise RuntimeError( "EventView creator alg search found different results in structured search (" + str(originalLength) + ") versus naive search (" + str( len( allEVCAs ) ) + "). Implies menu structure error" )
