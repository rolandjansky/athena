# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Dan Mori <dmori@cern.ch>
#This macro creates muon resolution profiles from 2D histograms
#Usage: python CreateResolutionProfiles.py filename [doAverage doBinned]
#Adding the flag doAverage adds a TF1 showing the overall pT scale/resolution to the profiles
#Adding the flag doBinned creates resolution plots for each pt, eta, phi bin for prompt muons

import sys
import os
from array import array
import itertools
from math import sqrt
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

def AddGaussian( hist ):
    if hist.GetEntries() < 2:
        return False
    gaus2 = ROOT.TF1( "gaus2", "gaus", hist.GetMean()-1.5*hist.GetRMS(), hist.GetMean()+1.5*hist.GetRMS() )
    gaus = ROOT.TF1( "gaus", "gaus", hist.GetXaxis().GetBinLowEdge(1), hist.GetXaxis().GetBinUpEdge(hist.GetNbinsX()) )
    if int(hist.Fit(gaus2,"RQ")) != 0:
        return False
    hist.GetListOfFunctions().Add(gaus)
    f1 = hist.GetFunction("gaus")
    f1.SetParameters( hist.GetFunction("gaus2").GetParameters() )
    hist.GetListOfFunctions().Remove( gaus2 )

    f1.SetLineColor(ROOT.kRed)
    return f1

#---------------------------------------------------------------------------

def GetProfilesFromTH2( hist, doAverage = False ):
    #get bin edges from x-axis (use same binning as original)
    binedges = [ hist.GetXaxis().GetBinLowEdge(1) ]
    binedges += [ hist.GetXaxis().GetBinUpEdge(i) for i in range( 1, hist.GetNbinsX()+1 ) ]
    #remove last bin (200-1000) if nothing there
    if hist.GetXaxis().GetBinUpEdge(hist.GetNbinsX()) == 1000 and hist.GetBinContent(hist.GetNbinsX()) < 3:
        binedges = binedges[:-1]
    nBins = len(binedges) - 1

    prof_ave = ROOT.TH1D( 'ave', 'ave', nBins, array( 'f', binedges ) )
    prof_std = ROOT.TH1D( 'std', 'std', nBins, array( 'f', binedges ) )

    binnedRes = {}
    for i in range( 1, nBins + 1 ):
        h = hist.ProjectionY( hist.GetName()+'_p_y', i, i )
        n = h.GetEntries()
        fit = AddGaussian( h )
        if fit:
            prof_ave.SetBinContent( i, fit.GetParameter(1) )
            prof_std.SetBinContent( i, fit.GetParameter(2) )
            prof_ave.SetBinError( i, prof_ave.GetBinContent(i)/sqrt(n) )
            if n > 10:
                prof_std.SetBinError( i, prof_std.GetBinContent(i)/sqrt(2*(n-1)) )
            else:
                prof_std.SetBinError( i, h.GetRMSError() )

        name = '{0}_{1}'.format( binedges[i-1], binedges[i] )
        binnedRes[name] = h.Clone(name)
        del h, fit

    SetYrange( prof_ave )
    SetYrange( prof_std )

    #Get overall ave and stdev and plot line on each profile
    if doAverage:
        h = hist.ProjectionY( hist.GetName()+'_p_y', 1, nBins )
        fit = AddGaussian( h )
        if fit:
            ave = fit.GetParameter(1)
            sig = fit.GetParameter(2)
            h.Delete()

            tot_ave = ROOT.TF1( 'aveline', "[0]", binedges[0], binedges[-1] )
            prof_ave.GetListOfFunctions().Add( tot_ave )
            f1 = prof_ave.GetFunction( 'aveline' )
            f1.SetParameter( 0, ave )
            f1.SetLineColor( ROOT.kRed )

            tot_sig = ROOT.TF1( 'sigline', "[0]", binedges[0], binedges[-1] )
            prof_std.GetListOfFunctions().Add( tot_sig )
            f2 = prof_std.GetFunction( 'sigline' )
            f2.SetParameter( 0, sig )
            f2.SetLineColor( ROOT.kRed )
        del h,fit

    return prof_ave, prof_std, binnedRes

#---------------------------------------------------------------------------

#create pt res and scale profiles vs pt, eta, phi from 2D histograms
def CreateProfile( infile, HistDir, HistName, Var, MuonType, doAverage = False, doBinned = False ):
    hist = infile.Get(HistDir).Get(HistName)
    xtitle = MuonType + ' Muon '
    if Var == 'pT':
         xtitle += 'Transverse Momentum [GeV]'
    else:
        xtitle += Var
    prof_ave, prof_std, binnedRes = GetProfilesFromTH2( hist, doAverage = doAverage )

    prof_ave.SetName( HistName.replace( 'pT_vs', 'PtScale_vs' ) )
    prof_ave.SetTitle( 'pT Scale vs ' + Var )
    prof_ave.SetXTitle( xtitle )
    prof_ave.SetYTitle( 'pT scale' )

    prof_std.SetName( HistName.replace( 'pT_vs', 'PtResol_vs' ) )
    prof_std.SetTitle( 'pT Resolution vs ' + Var )
    prof_std.SetXTitle( xtitle )
    prof_std.SetYTitle( 'pT resolution' )

    if not infile.Get( HistDir ).WriteTObject( prof_ave, prof_ave.GetName(), "Overwrite" ):
        # print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_ave.GetName() )
        print( 'WARNING failed to write histogram to file: ' + HistDir + '/' + prof_ave.GetName() )
    if not infile.Get( HistDir ).WriteTObject( prof_std, prof_std.GetName(), "Overwrite" ):
        # print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_std.GetName() )
        print( 'WARNING failed to write histogram to file: ' + HistDir + '/' + prof_std.GetName() )

    #create binned res plots
    bindirname = Var.lower() + 'Bins'
    bindirpath = HistDir + '/' + bindirname
    bindir = infile.GetDirectory( bindirpath )
    if not bindir:
        bindir = infile.GetDirectory( HistDir ).mkdir( bindirname )

    if doBinned:
        for name, plot in binnedRes.iteritems():
            split = HistName.find('resolution')+10
            plot.SetName( 'bin_' + name.replace('-','m') )
            plot.SetTitle( 'pT Resolution {0} < {1} < {2}'.format( name.split('_')[0], Var, name.split('_')[1] ) )
            plot.SetYTitle( 'Entries' )
            if not bindir.WriteTObject( plot, plot.GetName(), "Overwrite" ):
                print('WARNING failed to write histogram to file: ' + bindirpath + '/' + plot.GetName() )
                # print( 'INFO Writing histogram to file: ' + HistDir + '/' + prof_std.GetName() )

    del prof_ave, prof_std, binnedRes
#---------------------------------------------------------------------------

def main( args ):
    if len( args ) > 1:
        filename = args[1]
    else:
        print( 'Usage: python {0} filename [doAverage doBinned]'.format( args[0] ) )
        return

    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        return

    doAverage = False
    doBinned = False
    if len(args) > 2:
        doAverage = bool( 'doAverage' in args[2:] )
        doBinned = bool( 'doBinned' in args[2:] )

    print( 'Opening file: ' + filename )
    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'Prompt' ]
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
                HistName = '_'.join( HistDir.split('/') ) + '_Res{0}_pT_vs_{1}'.format( ResType, Var )
                if not FileDir.Get( HistName ):
                    print( 'INFO Histogram not found: ' + HistName )
                    continue

                CreateProfile( infile, HistDir, HistName, Var, MuonType, doAverage = doAverage, doBinned = doBinned )

    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
