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
from ROOT import *

#---------------------------------------------------------------------------

def SetYrange( hist ):
    #adjust y range so negative bins will show
    ymax = hist.GetBinContent( hist.GetMaximumBin() ) + hist.GetBinError( hist.GetMaximumBin() )
    ymin = hist.GetBinContent( hist.GetMinimumBin() ) - hist.GetBinError( hist.GetMinimumBin() )
    #c = 0.2*( ymax - ymin )
    #ymax += c
    #ymin -= c
    ymax += 0.2*ymax
    ymin -= 0.2*ymax
    hist.GetYaxis().SetRangeUser( ymin, ymax )

#---------------------------------------------------------------------------
def GetProfilesFromTH2( hist, doAverage ): 
    binedges = [ hist.GetXaxis().GetBinLowEdge(1) ]
    binedges += [ hist.GetXaxis().GetBinUpEdge(i) for i in range( 1, hist.GetNbinsX()+1 ) ]
    if hist.GetXaxis().GetBinUpEdge(hist.GetNbinsX()) == 1000 and hist.GetBinContent(hist.GetNbinsX()) < 3:    #remove last bin (200-1000) if nothing there
        binedges = binedges[:-1]
    nBins = len(binedges) - 1

    prof_ave = ROOT.TH1D( 'ave', 'ave', nBins, array( 'f', binedges ) )
    prof_std = ROOT.TH1D( 'std', 'std', nBins, array( 'f', binedges ) )
    binnedRes = {}
    for i in range( 1, nBins + 1 ):
        h = hist.ProjectionY( hist.GetName()+'_p_y', i, i )
        n = h.GetEntries()
        if n<20:
            continue
        ## fit range
        xmin = h.GetMean()-3*h.GetRMS()
        xmax = h.GetMean()+3*h.GetRMS()
        h.GetXaxis().SetRangeUser(xmin,xmax)
        print '>>> ' , h.GetName()
        mean, meanErr, sigma, sigmaErr, frame = fit(h, -0.5, 0.5) ## call the fit method
        prof_ave.SetBinContent( i, mean )
        prof_ave.SetBinError( i, meanErr )
        prof_std.SetBinContent( i, sigma )
        prof_std.SetBinError( i, sigmaErr )
        name = '{0}_{1}'.format( binedges[i-1], binedges[i] )
        binnedRes[name] = frame.Clone(name)
        #del h, fit
    SetYrange( prof_ave )
    SetYrange( prof_std )
    return prof_ave,prof_std,binnedRes
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# Simple fit method ................................................
#  F = f1 * Gauss(mean,sigma) + (1-f1) Gauss(mean,sigma) x Exp(tau)
# ..................................................................

def fit(h, emin,emax):
    x = RooRealVar('x','x',emin,emax);
    if (h.GetEntries()<20):
        return 0,0,0,0,x.frame()
    mean = RooRealVar('mean','mean',-0.001,-0.01,0.01);
    sigma = RooRealVar('sigma','#sigma',0.02, 0.01, 0.3);
    Gauss = RooGaussian('Gauss','Gauss',x,mean,sigma);
    f1 = RooRealVar('f1','f1',0.8,0.,1.);
    
    GaussModel = RooGaussModel('GaussModel','Gauss',x,mean,sigma)
    tau = RooRealVar('tau','tau',0.02,0.,1.)
    tailmodel = RooDecay('tailmodel','ExpxGaus',x,tau,GaussModel,RooDecay.DoubleSided);
    model = RooAddPdf('model','G1+E2',Gauss,tailmodel,f1)
    
    hdata = RooDataHist('hdata','hist', RooArgList(x), h);
    model.fitTo(hdata,RooFit.PrintLevel(-1),RooFit.Verbose(kFALSE));
    frame = x.frame();

    hdata.plotOn(frame)
    model.plotOn(frame,RooFit.LineColor(ROOT.kRed))
    model.plotOn(frame,RooFit.Components('tailmodel'),RooFit.DrawOption('F'),RooFit.FillColor(ROOT.kOrange),RooFit.MoveToBack())
    return mean.getVal(), mean.getError(), sigma.getVal(), sigma.getError(), frame
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

#create pt res and scale profiles vs pt, eta, phi from 2D histograms
def CreateProfile( infile, HistDir, HistName, Var, MuonType, doAverage = False, doBinned = False ):
    hist = infile.Get(HistDir).Get(HistName)
    if hist.IsA().InheritsFrom(ROOT.TProfile.Class()):
        return
    xtitle = MuonType + ' Muon '
    if 'pT' in Var:
         xtitle += 'pT [GeV]'
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
        canv = ROOT.TCanvas("canv","",800,800);
        canv.Divide(3,3)
        ipad = 0
        icanv = 0
        storePlots = 'resPlots/'+os.path.split(infile.GetName())[1].replace('.root','')
        if not os.path.exists(storePlots):
            os.makedirs(storePlots)
            print('Creating directory: '+storePlots)

        for x1,name,plot in sorted([ (float(name.split('_')[0]), name, plot) for name, plot in binnedRes.iteritems() ]):
            plot.SetName( 'bin_' + name.replace('-','m') )
            plot.SetTitle( 'pT Resolution {0} < {1} < {2}'.format( name.split('_')[0], Var, name.split('_')[1] ) )
            plot.SetYTitle( 'Entries' )
            t=ROOT.TCanvas()
#            if not bindir.WriteTObject( plot, plot.GetName(), "Overwrite" ):
#                print('WARNING failed to write histogram to file: ' + bindirpath + '/' + plot.GetName() )
            ipad+=1
            if ipad>9:
                canv.cd()
                canv.SaveAs(storePlots+'/'+HistDir.replace('/','_')+'_PtResFits_{0}_bins_{1}.pdf'.format(Var,icanv))
                canv.Clear()
                canv.Divide(3,3)
                icanv+=1
                ipad=1
            canv.cd(ipad)
            t = ROOT.TLatex()
            t.SetNDC(); t.SetTextColor(1);
            tit = name.replace('m','-').replace('_','<'+Var+'<')
            mu = mu_err = sigma = sigma_err = 0
            canv.cd(ipad)
            plot.Draw()
            t.DrawLatex(0.2,0.96,plot.GetTitle())
            resultMeanLab ='#mu = {0:0.2g} #pm {1:0.2g}'.format( mu, mu_err )
            resultSigmaLab ='#sigma = {0:0.2g} #pm {1:0.2g}'.format( sigma, sigma_err )
            t.DrawLatex(0.2,0.85,'#splitline{'+resultMeanLab+'}{'+resultSigmaLab+'}')

        canv.cd()
        canv.SaveAs(storePlots+'/'+HistDir.replace('/','_')+'_PtResFits_{0}_bins_{1}.pdf'.format(Var,icanv))
        canv.Close()

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
    doBinned = True
    if len(args) > 2:
        doAverage = bool( 'doAverage' in args[2:] )
        doBinned  = bool( 'doBinned'  in args[2:] )

    print( 'Opening file: ' + filename )
    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'Prompt' ]
    BinVars = [ 'pT','lowpT','highpT','eta', 'phi' ]

    for MuonType in MuonTypes:
        if not infile.Get( 'Muons/' + MuonType ):
            print( 'INFO TDirectory not found: Muons/' + MuonType )
            continue
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( MuonType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for Author in Authors:
            if Author=='MSTrackParticles' or Author=='CaloTag' or Author=='AllMuons' or Author=='Tight' or Author=='Loose' or Author=='VeryLoose':
                continue
            HistDir = 'Muons/{0}/matched/{1}/resolution'.format( MuonType, Author )
            FileDir = infile.Get( HistDir )
            if not FileDir:
                print( 'INFO TDirectory not found: ' + HistDir )
                continue
            for Var in BinVars:
                resTypes= [ '','ID','MS' ]
                for resType in resTypes:
                    HistName = '_'.join( HistDir.split('/') ) + '_Res{0}_pT_vs_{1}'.format( resType, Var )
                    if not FileDir.Get( HistName ):
                        #print( 'INFO Histogram not found: ' + HistName )
                        continue
                    CreateProfile( infile, HistDir, HistName, Var, MuonType, doAverage = doAverage, doBinned = doBinned )
                
    infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    #ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
