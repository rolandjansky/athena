#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Unit test for the xAOD::TPyEvent class.
#

## C/C++ style main function
def main():

    # The name of the application:
    APP_NAME = "ut_xaodrootaccess_tpyevent_test"

    # Set up a logger object:
    import logging
    logger = logging.getLogger( APP_NAME )
    logger.setLevel( logging.INFO )
    hdlr = logging.StreamHandler( sys.stdout )
    frmt = logging.Formatter( "%(name)-14s%(levelname)8s %(message)s" )
    hdlr.setFormatter( frmt )
    logger.addHandler( hdlr )

    # Set up the environment:
    import ROOT
    if not ROOT.xAOD.Init( APP_NAME ).isSuccess():
        logger.error( "Failed to call xAOD::Init(...)" )
        return 1

    # Pre-load some dictionaries. To avoid ROOT-10940.
    ROOT.xAOD.L2CombinedMuonContainer()
    ROOT.xAOD.TrigElectronContainer()
    ROOT.xAOD.MuonContainer()

    # Create the objects to test:
    from xAODRootAccess.TPyEvent import TPyEvent
    event = TPyEvent()
    from xAODRootAccess.TPyStore import TPyStore
    store = TPyStore()

    # Create a transient tree from a test file:
    import os
    ifile = ROOT.TFile.Open( "$ASG_TEST_FILE_MC", "READ" )
    if not ifile:
        logger.error( "Couldn't open input file: %s" %
                      os.environ.get( "ASG_TEST_FILE_MC",
                                      "!ASG_TEST_FILE_MC!" ) )
        return 1
    tree = ROOT.xAOD.MakeTransientTree( ifile )
    if not tree:
        logger.error( "Failed to make transient tree from file: %s" %
                      os.environ.get( "ASG_TEST_FILE_MC",
                                      "!ASG_TEST_FILE_MC!" ) )
        return 1
    import xAODRootAccess.GenerateDVIterators

    # Connect the TPyEvent object to an output file:
    ofile = ROOT.TFile.Open( "test.root", "RECREATE" )
    if not ofile:
        logger.error( "Couldn't create test output file" )
        return 1
    if not event.writeTo( ofile ).isSuccess():
        logger.error( "Couldn't connect xAOD::TPyEvent object to the output "
                      "file" )
        return 1

    # Loop over 10 events from the input file:
    for entry in range( 10 ):

        # Load the event:
        tree.GetEntry( entry )
        logger.info( "Processing entry %i" % entry )

        # Clear the store:
        store.clear()

        # Copy the Kt4EMTopoOriginEventShape payload:
        if not event.record( tree.Kt4EMTopoOriginEventShape,
                             "Kt4EMTopoOriginEventShape" ).isSuccess():
            logger.error( "Failed to record xAOD::EventShape from the "
                          "input file" )
            return 1
        if not event.record( tree.Kt4EMTopoOriginEventShape.getConstStore(),
                             "Kt4EMTopoOriginEventShapeAux." ).isSuccess():
            logger.error( "Faiedl to record xAOD::EventShapeAuxInfo from the "
                          "input file" )
            return 1

        # Check that the muons can be accessed:
        logger.info( "  Number of muons: %i" %
                     tree.Muons.size() )

        # Copy the muons into the output:
        if not event.record( tree.Muons,
                             "AllMuons" ).isSuccess():
            logger.error( "Failed to record xAOD::MuonContainer from the "
                          "input file" )
            return 1
        if not event.record( tree.Muons.getConstStore(),
                             "AllMuonsAux." ).isSuccess():
            logger.error( "Failed to record xAOD::MuonAuxContainer from "
                          "the input file" )
            return 1

        # Create a container of just the central muons:
        cMuons = ROOT.xAOD.MuonContainer()

        # And record the container into the output right away. (In order to get
        # a proper auxiliary container for it.)
        cMuons.setNonConstStore( event.recordAux( "CentralMuonsAux." ) )
        if not event.record( cMuons,
                             "CentralMuons" ).isSuccess():
            logger.error( "Failed to record central muons into the output" )
            return 1

        # Now put all central muons into this container, with just a few
        # properties. Since deep copying doesn't work this easily... :-(
        for mu in tree.Muons:
            if abs( mu.eta() ) < 1.0:
                newMu = ROOT.xAOD.Muon()
                cMuons.push_back( newMu )
                newMu.setP4( mu.pt(), mu.eta(), mu.phi() )
                pass
            pass

        # Print how many central muons got selected:
        logger.info( "  Number of central muons: %i" %
                     cMuons.size() )

        # Put an object into the transient store:
        trMuons = ROOT.xAOD.MuonContainer()
        if not store.record( trMuons, "TransientMuons" ).isSuccess():
            logger.error( "Failed to record transient muons into the "
                          "transient store" )
            return 1

        # Check that it is now available through TPyEvent:
        if not event.contains( "TransientMuons",
                               ROOT.xAOD.MuonContainer ):
            logger.error( "Transient muons not visible through TPyEvent" )
            return 1

        # Record the event:
        event.fill()
        pass

    # Close the output file:
    if not event.finishWritingTo( ofile ).isSuccess():
        logger.error( "Couldn't call finishWritingTo(...)" )
        return 1
    ofile.Close()

    # Clean up:
    ROOT.xAOD.ClearTransientTrees()

    # Return gracefully:
    return 0

# Run the main() function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
