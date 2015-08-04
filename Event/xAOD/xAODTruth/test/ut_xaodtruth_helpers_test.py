#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: ut_xaodtruth_helpers_test.py 668406 2015-05-19 15:32:15Z krasznaa $
#
# This is a simple unit test for checking the health of the truth helper
# functions in the standalone analysis environment.
#

## C/C++ style main function
def main():

    # Initialise the environment:
    import ROOT
    ROOT.gROOT.Macro( "$ROOTCOREDIR/scripts/load_packages.C" )

    # Open an input xAOD file:
    FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000031.pool.root.1"
    f = ROOT.TFile.Open( FNAME, "READ" )
    if not f:
        print( "Couldn't open \"%s\"" % FNAME )
        return 1
    print( "Opened: %s" % FNAME )

    # Make a transient tree from it:
    t = ROOT.xAOD.MakeTransientTree( f )
    if not t:
        print( "Couldn't make a transient tree from the input file!" )
        return 1
    import xAODRootAccess.GenerateDVIterators

    # Loop on the first few events:
    for entry in xrange( 10 ):
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

    return 0;

# Execute the main function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
