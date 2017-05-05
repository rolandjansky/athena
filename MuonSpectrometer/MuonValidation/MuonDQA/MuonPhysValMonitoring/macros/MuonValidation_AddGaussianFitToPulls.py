# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#author: Dan Mori
#Add gaussian curve to pull plots
#if plotdir argument given, will output pdfs in that directory of all pull plots
#usage: python PullFitMacro.py physval_file.root [plotdir]

import ROOT
import sys
import os
import itertools

#---------------------------------------------------------------------------

def AddGaussian( infile, HistDir, HistName ):
    hist = infile.GetDirectory(HistDir).Get(HistName)
    if hist.GetEntries() < 10:
        print( 'INFO Skipping Fit. NumEntries = {0}: '.format(hist.GetEntries()) + HistName )
        return
    gaus2 = ROOT.TF1( "gaus2", "gaus", -2, 2 )
    gaus5 = ROOT.TF1( "gaus", "gaus", -5, 5 )
    if int(hist.Fit(gaus2,"RQ")) != 0:
        print( 'WARNING fit failed for ' + HistName )
        return
    hist.GetListOfFunctions().Add(gaus5)
    f1 = hist.GetFunction("gaus")
    f1.SetParameters( hist.GetFunction("gaus2").GetParameters() )
    hist.GetListOfFunctions().Remove( gaus2 )
    f1.SetLineColor(ROOT.kRed)
    print('INFO Overwriting histogram: ' + HistDir + '/' + HistName )
    hist.GetDirectory().WriteTObject( hist, HistName, "Overwrite" )

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
                HistName = '_'.join( DirName.split('/') ) + '_Pull{0}_'.format(PullType) + var
                if not Dir.Get( HistName ):
                    print( 'INFO Histogram not found: ' + HistName )
                    continue
                AddGaussian( infile, DirName, HistName )
    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    #ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
