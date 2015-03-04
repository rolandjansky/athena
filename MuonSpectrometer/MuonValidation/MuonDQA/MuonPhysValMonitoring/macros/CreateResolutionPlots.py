# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
from array import array
import itertools
import ROOT


#---------------------------------------------------------------------------

def SetYrange( hist ):
    #adjust y range so negative bins will show
    ymax = hist.GetBinContent( hist.GetMaximumBin() ) + hist.GetBinError( hist.GetMaximumBin() )
    ymin = hist.GetBinContent( hist.GetMinimumBin() ) - hist.GetBinError( hist.GetMinimumBin() )
    c = 0.2*( ymax - ymin )
    ymax += c
    ymin -= c
    hist.GetYaxis().SetRangeUser( ymin, ymax )

#---------------------------------------------------------------------------

def GetProfilesFromTH2( hist ):
    #get bin edges from x-axis (use same binning as original)
    binedges = [ hist.GetXaxis().GetBinLowEdge(1) ]
    binedges += [ hist.GetXaxis().GetBinUpEdge(i) for i in range( 1, hist.GetNbinsX()+1 ) ]
    #remove last bin (200-1000) if nothing there
    if hist.GetXaxis().GetBinUpEdge(hist.GetNbinsX()) == 1000 and hist.GetBinContent(hist.GetNbinsX()) < 10:
        binedges = binedges[:-1]
    nBins = len(binedges) - 1

    prof_ave = ROOT.TH1F( 'ave', 'ave', nBins, array( 'f', binedges ) )
    prof_std = ROOT.TH1F( 'std', 'std', nBins, array( 'f', binedges ) )

    for i in range( 1, nBins + 1 ):
        h = hist.ProjectionY( '_p_y', i, i )
        prof_ave.SetBinContent( i, h.GetMean() )
        prof_ave.SetBinError(   i, h.GetMeanError() )
        prof_std.SetBinContent( i, h.GetStdDev() )
        prof_std.SetBinError(   i, h.GetStdDevError() )
        h.Delete()

    SetYrange( prof_ave )
    SetYrange( prof_std )

    #Get overall ave and stdev and plot line on each profile
    h = hist.ProjectionY( '_p_y', 1, nBins )
    ave = h.GetMean()
    sig = h.GetStdDev()
    h.Delete()

    tot_ave = ROOT.TF1( 'aveline', "[0]", binedges[0], binedges[-1] )
    prof_ave.GetListOfFunctions().Add( tot_ave )
    f1 = prof_ave.GetFunction( "aveline" )
    f1.SetParameter( 0, ave )
    f1.SetLineColor( ROOT.kRed )

    tot_sig = ROOT.TF1( 'sigline', "[0]", binedges[0], binedges[-1] )
    prof_std.GetListOfFunctions().Add( tot_sig )
    f2 = prof_std.GetFunction( 'sigline' )
    f2.SetParameter( 0, sig )
    f2.SetLineColor( ROOT.kRed )

    return ( prof_ave, prof_std )

#---------------------------------------------------------------------------

#create pt res and scale profiles vs pt, eta, phi from 2D histograms
def CreateProfile( infile, HistDir, HistName, Var, xtitle ):
    hist = infile.Get(HistDir).Get(HistName)
    prof_ave, prof_std = GetProfilesFromTH2( hist )

    prof_ave.SetName( HistName.replace( 'pT_vs', 'PtScale_vs' ) )
    prof_ave.SetTitle( 'pT Scale vs ' + Var )
    prof_ave.SetXTitle( xtitle )
    prof_ave.SetYTitle( 'pT scale' )

    prof_std.SetName( HistName.replace( 'pT_vs', 'PtResol_vs' ) )
    prof_std.SetTitle( 'pT Resolution vs ' + Var )
    prof_std.SetXTitle( xtitle )
    prof_std.SetYTitle( 'pT resolution' )

    if infile.Get( HistDir ).WriteTObject( prof_ave, prof_ave.GetName(), "Overwrite" ):
        print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_ave.GetName() )
    if infile.Get( HistDir ).WriteTObject( prof_std, prof_std.GetName(), "Overwrite" ):
        print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_std.GetName() )
    del prof_ave, prof_std

#---------------------------------------------------------------------------

def main( args ):
    if len( args ) > 1:
        filename = args[1]
        if len( args ) > 2:
            PlotDir = os.path.dirname( args[2] )
            print( 'INFO Plots will be saved to {0}'.format( PlotDir ) )
            if not os.path.exists(PlotDir):
                os.makedirs(PlotDir)
        else:
            PlotDir = ''

    else:
        print( 'Usage: python {0} filename [plot directory]'.format( args[0] ) )
        return

    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        return

    print( 'Opening file: ' + filename )
    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    ResTypes = [ '', 'ID', 'MS' ]
    BinVars = [ 'pT', 'eta', 'phi' ]

    for MuonType in MuonTypes:
        if not infile.Get( 'Muons/' + MuonType ):
            print( 'INFO TDirectory not found: Muons/' + MuonType )
            continue
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( MuonType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for Author in Authors:
            HistDir = 'Muons/{0}/matched/{1}/resolution'.format( MuonType, Author )
            FileDir = infile.Get( HistDir )
            if not FileDir:
                print( 'INFO TDirectory not found: ' + HistDir )
                continue
            for ResType, Var in itertools.product( ResTypes, BinVars ):
                xtitle = MuonType + ' Muon '
                if Var == 'pT':
                     xtitle += 'Transverse Momentum [GeV]'
                else:
                    xtitle += Var
                HistName = '_'.join( HistDir.split('/') ) + '_Res{0}_pT_vs_{1}'.format( ResType, Var )
                if not FileDir.Get( HistName ):
                    print( 'INFO Histogram not found: ' + HistName )
                    continue

                CreateProfile( infile, HistDir, HistName, Var, xtitle )

    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
