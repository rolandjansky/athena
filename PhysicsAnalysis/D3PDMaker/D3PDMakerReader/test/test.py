# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def eventInfo( nt ):
    print ""
    print "Run: %i, Event: %i" % ( nt.eventinfo.RunNumber(), nt.eventinfo.EventNumber() )
    print "Number of electrons: %i" % nt.el.n()
    print "  Their pT: " + str( [ nt.el[ i ].pt() for i in range( nt.el.n() ) ] )
    print "Number of muons: %i" % nt.mu_staco.n()
    print "  Their eta: " + str( [ eta for eta in nt.mu_staco.eta() ] )
    print ""
    return

import ROOT
ROOT.gSystem.Load( "libD3PDReader" )
f = ROOT.TFile.Open( "../myNTUP_SMWZ.root", "READ" )
t = f.Get( "physics" )
ntup = ROOT.D3PDReader.NTUP_SMWZ()
ntup.ReadFrom( t )

for i in range( t.GetEntries() ):
    ntup.GetEntry( i )
    eventInfo( ntup )
    pass
