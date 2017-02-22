#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: ut_xaodrootaccess_transtree2_test.py 796448 2017-02-09 18:28:08Z ssnyder $
#
# Unit test for the transient tree creating infrastructure
#

# The necessary import(s):
import ROOT
import os

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
    if (os.environ.has_key('ROOTCOREDIR') and
        ROOT.gROOT.Macro( "$ROOTCOREDIR/scripts/load_packages.C" )):
        logger.error( "Couldn't load the RootCore packages" )
        return 1
    ROOT.xAOD.TEvent
    if ROOT.xAOD.Init( APP_NAME ).isFailure():
        logger.error( "Failed to call xAOD::Init(...)" )
        return 1

    # Set up a TFile as input:
    fname = os.environ.get ('ASG_TEST_FILE_DATA', None)
    if not fname:
        fpath = os.environ.get ('ATLAS_REFERENCE_DATA',
                                '/afs/cern.ch/atlas/project/PAT')
        fname = os.path.join (fpath,
                              "xAODs/r8601/"
                              "data15_13TeV.00284285.physics_Main.recon.AOD.r8601/AOD.09617210._008306.pool.root.1")

    ifile = ROOT.TFile.Open( fname, "READ" )
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
    for entry in xrange( 10 ):
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
