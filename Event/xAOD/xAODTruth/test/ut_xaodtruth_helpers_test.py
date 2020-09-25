#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This is a simple unit test for checking the health of the truth helper
# functions in the standalone analysis environment.
#

## C/C++ style main function
def main():

    # Initialise the environment:
    import ROOT
    if not ROOT.xAOD.Init().isSuccess():
        print( "Couldn't initialise the xAOD reading environment" )
        return 1

    # Open an input xAOD file:
    import os
    FNAME = os.getenv( "ASG_TEST_FILE_MC", "FileNotSpecifiedInEnvironment" )
    f = ROOT.TFile.Open( FNAME, "READ" )
    if not f:
        print( "Couldn't open \"%s\"" % FNAME )
        return 1
    print( "Opened: %s" % FNAME )

    # Make a transient tree from it:
    treeMgr = ROOT.xAOD.TTreeMgr()
    if not treeMgr.readFrom( f ).isSuccess():
        print( "Couldn't make a transient tree from the input file!" )
        return 1
    import xAODRootAccess.GenerateDVIterators
    t = treeMgr.eventTree()

    # Loop on the first few events:
    for entry in range( 10 ):
        # Load the event:
        if t.GetEntry( entry ) < 0:
            print( "Couldn't load entry %i from the input!" % entry )
            return 1
        # Print some header info:
        print( "Processing run #%i event #%i (%i events processed so far)" % \
               ( t.EventInfo.runNumber(), t.EventInfo.eventNumber(), entry ) )
        # Exercise the functions on electrons:
        for el in t.Electrons:
            print( " - Electron: eta = %g, phi = %g, pt = %g" % \
                   ( el.eta(), el.phi(), el.pt() ) )
            print( "             truthType = %i, truthOrigin = %i" % \
                   ( ROOT.xAOD.TruthHelpers.getParticleTruthType( el ),
                     ROOT.xAOD.TruthHelpers.getParticleTruthOrigin( el ) ) )
            tp = ROOT.xAOD.TruthHelpers.getTruthParticle( el )
            if tp:
                print( "   - TruthParticle: eta = %g, phi = %g, pt = %g" % \
                       ( tp.eta(), tp.phi(), tp.pt() ) )
                pass
            pass
        pass

    return 0

# Execute the main function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
