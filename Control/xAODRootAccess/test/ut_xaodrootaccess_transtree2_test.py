#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Unit test for the transient tree creating infrastructure
#

# The necessary import(s):
import ROOT

## Helper function creating a vector<string> object out of a list of strings
def toVectorString( stringList ):

    # Create the result object:
    result = ROOT.vector( "string" )()

    # Fill it:
    for element in stringList:
        result.push_back( element )
        pass

    # Return the object:
    return result

## C/C++ style main function
def main():

    # The name of the application:
    APP_NAME = "ut_xaodrootaccess_transtree2_test"

    # Set up a logger object:
    import logging
    logger = logging.getLogger( APP_NAME )
    logger.setLevel( logging.INFO )
    hdlr = logging.StreamHandler( sys.stdout )
    frmt = logging.Formatter( "%(name)-14s%(levelname)8s %(message)s" )
    hdlr.setFormatter( frmt )
    logger.addHandler( hdlr )

    # Set up the environment:
    if not ROOT.xAOD.Init( APP_NAME ).isSuccess():
        logger.error( "Failed to call xAOD::Init(...)" )
        return 1

    # Set up a TFile as input:
    ifile = ROOT.TFile.Open( "$ASG_TEST_FILE_DATA", "READ" )
    if not ifile:
        logger.error( "Couldn't open the test input file" )
        return 1

    # Create the tree manager:
    mgr = ROOT.xAOD.TTreeMgr( ROOT.xAOD.TEvent.kAthenaAccess )
    if not mgr.readFrom( ifile ).isSuccess():
        logger.error( "Couldn't set up the reading of the input file" )
        return 1

    # Filter which containers to read:
    if not mgr.enableEventObj( toVectorString( [ "Electrons",
                                                 "Muons" ] ) ).isSuccess():
        logger.error( "Couldn't set up the filtering of the input" )
        return 1

    # Fix up the DataVector iterators for PyROOT:
    import xAODRootAccess.GenerateDVIterators

    # Loop over a few event:
    for entry in range( 10 ):
        # Load the event:
        if mgr.eventTree().GetEntry( entry ) < 0:
            logger.error( "Couldn't load entry %i" % entry )
            return 1
        logger.info( "Processing entry %i" % entry )
        # Print the properties of the electrons:
        logger.info( "  Number of electrons: %i" % \
                      mgr.eventTree().Electrons.size() )
        for el in mgr.eventTree().Electrons:
            logger.info( "   - eta: %g, phi: %g, pt: %g" % \
                         ( el.eta(), el.phi(), el.pt() ) )
            pass
        pass

    # Return gracefully:
    return 0

# Run the main() function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
