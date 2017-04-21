# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
from array import array
import itertools
import ROOT
from MuonValidation_CreateResolutionProfiles import *

#---------------------------------------------------------------------------

#create profile plots for pull parameters vs pt from 2D histograms
def CreateProfile( infile, HistDir, HistName, Var, doAverage = False ):
    hist = infile.Get(HistDir).Get(HistName)
    if not hist:
        return
    prof_ave, prof_std, projHists = GetProfilesFromTH2( hist, doAverage = doAverage )    

    prof_ave.SetName( hist.GetName().replace( '_vs_', '_ProfMean_vs_' ) )
    prof_ave.SetTitle( Var + ' Pull vs pt (Mean)' )
    prof_ave.SetXTitle( 'Transverse Momentum [GeV]' )
    prof_ave.SetYTitle( Var + ' Pull Mean' )

    prof_std.SetName( hist.GetName().replace( '_vs_', '_ProfStDev_vs_' ) )
    prof_std.SetTitle( Var + ' Pull vs pt (StDev)' )
    prof_std.SetXTitle( 'Transverse Momentum [GeV]' )
    prof_std.SetYTitle( Var + ' Pull StdDev' )

    if not infile.Get( HistDir ).WriteTObject( prof_ave, prof_ave.GetName(), "Overwrite" ):
        print( 'WARNING Failed to write histogram to file: ' + HistDir + '/' + prof_ave.GetName() )
    if not infile.Get( HistDir ).WriteTObject( prof_std, prof_std.GetName(), "Overwrite" ):
        print( 'WARNING Failed to write histogram to file: ' + HistDir + '/' + prof_std.GetName() )
    del prof_ave, prof_std

#---------------------------------------------------------------------------

def main( args ):
    doAverage = False
    if len( args ) > 1:
        filename = args[1]
        if len(args) > 2 and args[2] == 'doAverage':
            doAverage = True
    else:
        print( 'Usage: python {0} filename [doAverage]'.format( args[0] ) )
        return

    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        return

    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'All', 'Prompt' ] #, 'InFlight', 'NonIsolated' ]
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
            for var in Variables:
                HistName = '_'.join( DirName.split('/') ) + '_Pull_{0}_vs_pt'.format(var)
                if not Dir.Get( HistName ):
                    print( 'INFO Histogram not found: ' + HistName )
                    continue
                CreateProfile( infile, DirName, HistName, var, doAverage = doAverage )
    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    #ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
