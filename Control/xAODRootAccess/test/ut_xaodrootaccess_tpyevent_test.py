#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: ut_xaodrootaccess_tpyevent_test.py 741414 2016-04-19 17:06:16Z krasznaa $
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
    if ROOT.gROOT.Macro( "$ROOTCOREDIR/scripts/load_packages.C" ):
        logger.error( "Couldn't load the RootCore packages" )
        return 1
    if ROOT.xAOD.Init( APP_NAME ).isFailure():
        logger.error( "Failed to call xAOD::Init(...)" )
        return 1

    # Create the objects to test:
    from xAODRootAccess.TPyEvent import TPyEvent
    event = TPyEvent()
    from xAODRootAccess.TPyStore import TPyStore
    store = TPyStore()

    # Create a transient tree from a test file:
    FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/" \
            "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon." \
            "AOD.e1727_s1933_s1911_r5591/AOD.01494881._105458.pool.root.1"
    ifile = ROOT.TFile.Open( FNAME, "READ" )
    if not ifile:
        logger.error( "Couldn't open input file: %s" % FNAME )
        return 1
    tree = ROOT.xAOD.MakeTransientTree( ifile )
    if not tree:
        logger.error( "Failed to make transient tree from file: %s" % FNAME )
        return 1

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
    for entry in xrange( 10 ):

        # Load the event:
        tree.GetEntry( entry )
        logger.info( "Processing entry %i" % entry )

        # Clear the store:
        store.clear()

        # Copy the EventInfo payload:
        if not event.record( tree.EventInfo,
                             "EventInfo" ).isSuccess():
            logger.error( "Failed to record xAOD::EventInfo from the "
                          "input file" )
            return 1
        if not event.record( tree.EventInfo.getConstStore(),
                             "EventInfoAux." ).isSuccess():
            logger.error( "Faiedl to record xAOD::EventAuxInfo from the "
                          "input file" )
            return 1

        # Check that the electrons can be accessed:
        logger.info( "  Number of electrons: %i" %
                     tree.ElectronCollection.size() )

        # Copy the electrons into the output:
        if not event.record( tree.ElectronCollection,
                             "AllElectrons" ).isSuccess():
            logger.error( "Failed to record xAOD::ElectronContainer from the "
                          "input file" )
            return 1
        if not event.record( tree.ElectronCollection.getConstStore(),
                             "AllElectronsAux." ).isSuccess():
            logger.error( "Failed to record xAOD::ElectronAuxContainer from "
                          "the input file" )
            return 1

        # Create a container of just the central electrons:
        cElectrons = ROOT.xAOD.ElectronContainer_v1()

        # And record the container into the output right away. (In order to get
        # a proper auxiliary container for it.)
        cElectrons.setNonConstStore( event.recordAux( "CentralElectronsAux." ) )
        if not event.record( cElectrons,
                             "CentralElectrons" ).isSuccess():
            logger.error( "Failed to record central electrons into the output" )
            return 1

        # Now put all central electrons into this container, with just a few
        # properties. Since deep copying doesn't work this easily... :-(
        for i in xrange( tree.ElectronCollection.size() ):
            el = tree.ElectronCollection.at( i )
            if abs( el.eta() ) < 1.0:
                newEl = ROOT.xAOD.Electron_v1()
                cElectrons.push_back( newEl )
                newEl.setP4( el.pt(), el.eta(), el.phi(), el.m() )
                pass
            pass

        # Print how many central electrons got selected:
        logger.info( "  Number of central electrons: %i" %
                     cElectrons.size() )

        # Put an object into the transient store:
        trElectrons = ROOT.xAOD.ElectronContainer_v1()
        if not store.record( trElectrons, "TransientElectrons" ).isSuccess():
            logger.error( "Failed to record transient electrons into the "
                          "transient store" )
            return 1

        # Check that it is now available through TPyEvent:
        if not event.contains( "TransientElectrons",
                               ROOT.xAOD.ElectronContainer_v1 ):
            logger.error( "Transient electrons not visible through TPyEvent" )
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
