# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

File = ROOT.TFile( 'OutputRegion3.root' )
ws = File.Get( 'PreJob1___Region3~BkgFit~MassJpsi~6_0_pt_50_0' )
ds = ws.data( 'tempDataSet_Data' ) 
#pdf = ws.pdf( 'tempCB_MC' ) 
x = ws.var( 'x' ) 
Canvas = ROOT.TCanvas( 'canvas', '', 600, 600 )
Frame = x.frame()
ds.plotOn( Frame )
m = ROOT.RooRealVar( 'm', '#mu', 3.1, 3.05, 3.15 );
s = ROOT.RooRealVar( 's', '#sigma', 0.01, 0.0001, 0.05 );
a = ROOT.RooRealVar( 'a', '#alpha', 2, 0.5, 4 );
e = ROOT.RooRealVar( 'e', '#eta', 10, -100, 100 );
cb = ROOT.RooCBShape( 'cb', 'Crystal-Ball', x, m, s, a, e );
m_g = ROOT.RooRealVar( 'm_g', '#muGauss', 0, 0, 0 );
s_g = ROOT.RooRealVar( 's_g', '#sigmaGauss', 0.1, 0.01, 0.5 );
reso = ROOT.RooGaussian( 'gauss', 'Gaussian', x, m_g, s_g );
#mypdf = ROOT.RooFFTConvPdf( 'mypdf', '', x, cb, reso )
#mypdf = ROOT.RooLandau( 'mypdf', '', x, m, s )
mypdf1 = ROOT.RooGaussian( 'mypdf1', '', x, m, s )
mypdf2 = ROOT.RooGaussian( 'mypdf2', '', x, m, s_g )
mypdf3 = ROOT.RooGaussian( 'mypdf2', '', x, m, s_g )
coef1 = ROOT.RooRealVar( 'coef1', '', 0.9, 0.0, 1 );
coef2 = ROOT.RooRealVar( 'coef2', '', 0.9, 0.0, 1 );
#mypdf = ROOT.RooAddPdf( 'mypdf', '', mypdf1, mypdf2, coef1 ) 
bw = ROOT.RooBreitWigner( 'bw', '', x, m, s ) 
#mypdf = ROOT.RooFFTConvPdf( 'mypdf', '', x, bw, reso )
#mypdf = ROOT.RooGaussian( 'mypdf', '', x, m, s )
mypdf = ROOT.RooCBShape( 'mypdf', '', x, m, s, a, e );

tempBkgNorm = ROOT.RooRealVar( "tempBkgNorm", "Exponential Coefficient", 1000, 0, 10000000 );
tempSigNorm = ROOT.RooRealVar( "tempSigNorm", "Signal Coefficient", 10000, 0, 10000000 );
t = ROOT.RooRealVar( "t", "#tau", -1, -10, 0, "" );
tempExpo = ROOT.RooExponential( "tempExpo", "Exponential", x, t );
tempModel = ROOT.RooAddPdf( "tempModel", "Complete Model", ROOT.RooArgList( tempExpo, mypdf ), ROOT.RooArgList( tempBkgNorm, tempSigNorm ) );

tempModel.fitTo( ds )
tempModel.plotOn( Frame, ROOT.RooFit.LineColor( ROOT.kRed + 1 ), ROOT.RooFit.Name( 'TOTAL' ) )
tempModel.plotOn( Frame, ROOT.RooFit.Components( 'mypdf' ), ROOT.RooFit.LineColor( ROOT.kOrange + 7 ), ROOT.RooFit.LineStyle( ROOT.kDashed ), ROOT.RooFit.Name( 'SIGNAL' ) )
tempModel.plotOn( Frame, ROOT.RooFit.Components( 'tempExpo' ), ROOT.RooFit.LineColor( ROOT.kAzure + 1 ), ROOT.RooFit.LineStyle( ROOT.kDotted ), ROOT.RooFit.Name( 'BACKGROUND' ) )
Frame.Draw()
Canvas.SaveAs( '.pdf' )

