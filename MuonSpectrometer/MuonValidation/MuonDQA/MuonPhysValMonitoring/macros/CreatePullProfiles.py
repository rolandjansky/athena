# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
from array import array
import itertools
import ROOT
from CreateResolutionPlots import GetProfilesFromTH2

#---------------------------------------------------------------------------

#create profile plots for pull parameters vs pt from 2D histograms
def CreateProfile( infile, HistDir, HistName, Var ):
    hist = infile.Get(HistDir).Get(HistName)
    prof_ave, prof_std = GetProfilesFromTH2( hist )

    prof_ave.SetName( hist.GetName().replace( '_vs_', '_ProfMean_vs_' ) )
    prof_ave.SetTitle( Var + ' Pull Profile (Mean)' )
    prof_ave.SetXTitle( 'Transverse Momentum [GeV]' )
    prof_ave.SetYTitle( Var + ' Pull Mean' )

    prof_std.SetName( hist.GetName().replace( '_vs_', '_ProfStDev_vs_' ) )
    prof_std.SetTitle( Var + ' Pull Profile (Std Dev)' )
    prof_std.SetXTitle( 'Transverse Momentum [GeV]' )
    prof_std.SetYTitle( Var + ' Pull StdDev' )

    if infile.Get( HistDir ).WriteTObject( prof_ave, prof_ave.GetName(), "Overwrite" ):
        print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_ave.GetName() )
    if infile.Get( HistDir ).WriteTObject( prof_std, prof_std.GetName(), "Overwrite" ):
        print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_std.GetName() )
    del prof_ave, prof_std

#---------------------------------------------------------------------------

def main( args ):
    if len( args ) > 1:
        filename = args[1]
    else:
        print( 'Usage: python {0} filename'.format( args[0] ) )
        return

    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        return

    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    PullTypes = [ '', 'ID', 'MS' ]
    Variables = [ 'phi', 'theta', 'qOverP', 'd0', 'z0' ]

    for MuonType in MuonTypes:
        if not infile.Get( 'Muons/' + MuonType ):
            print( 'INFO TDirectory not found: Muons/' + MuonType )
            continue
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( MuonType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for Author in Authors:
            DirName = 'Muons/{0}/matched/{1}/Pulls'.format( MuonType, Author )
            Dir = infile.Get( DirName )
            if not Dir:
                print( 'INFO TDirectory not found: ' + DirName )
                continue
            for PullType, var in itertools.product( PullTypes, Variables ):
                HistName = '_'.join( DirName.split('/') ) + '_Pull{0}_'.format(PullType) + var + '_vs_pt'
                if not Dir.Get( HistName ):
                    print( 'INFO Histogram not found: ' + HistName )
                    continue
                CreateProfile( infile, DirName, HistName, var )
    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
