# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, math, sys
import ROOT
import AtlasStyle
from optparse import OptionParser

def main():

  parser = OptionParser()
  parser.add_option( '-c', '--create', dest = 'create', action = 'store_true', default = False, help = '' )
  ( options, args ) = parser.parse_args()

  ROOT.gROOT.SetBatch( True )
  ROOT.gErrorIgnoreLevel = 0

  File = ROOT.TFile( 'rew.root' )
  Tree = File.Get( 'z_rew_tree' )

  Vars = [ ( 'Pair_Mass', 'm_{#mu#mu} [GeV]', 100, 66, 116 ),
           ( 'Pair_Pt', 'p_{T}^{#mu#mu} [GeV]', 100, 0, 90 ),
           ( 'Pair_y', 'y^{#mu#mu}', 100, -3, 3 ),
           ( 'Pair_DeltaR', '#DeltaR^{#mu#mu}', 100, 0, 5 ),
           ( 'Pair_DeltaPhi', '#Delta#phi^{#mu#mu}', 100, -math.pi, math.pi ),
           ( 'Pair_CosTheta', 'cos(#theta)', 100, -1, 1 ),
           ( 'Lead_Pt', 'p_{T}^{#mu1} [GeV]', 90, 10, 100 ),
           ( 'Sub_Pt', 'p_{T}^{#mu2} [GeV]', 90, 10, 100 ),
         ]

  Selection = 'Sub_Pt>15 && Pair_Mass>66 && Pair_Mass<116'

  if not options.create:
    for Var in Vars:
      name = Var[ 0 ]
      title = Var[ 1 ]
      nbins = Var[ 2 ]
      min = Var[ 3 ]
      max = Var[ 4 ]
      locals()[ 'data_%s' % name ] = ROOT.TH1F( 'data_%s' % name, '', nbins, min, max )
      locals()[ 'mc_%s' % name ] = ROOT.TH1F( 'mc_%s' % name, '', nbins, min, max )
      Tree.Draw( '%s>>data_%s' % ( name, name ), '( %s && Type==1 )' % Selection )
      Tree.Draw( '%s>>mc_%s' % ( name, name ), '( %s && Type==0 )' % Selection )
      locals()[ 'ratio_%s' % name ] = DoPlot( name, 'Unweighted', title, locals()[ 'data_%s' % name ], locals()[ 'mc_%s' % name ] )

    for Var in Vars:
      name = Var[ 0 ]
      title = Var[ 1 ]
      nbins = Var[ 2 ]
      min = Var[ 3 ]
      max = Var[ 4 ]
      locals()[ 'data_w_%s' % name ] = ROOT.TH1F( 'data_w_%s' % name, '', nbins, min, max )
      locals()[ 'mc_w_%s' % name ] = ROOT.TH1F( 'mc_w_%s' % name, '', nbins, min, max )
      Tree.Draw( '%s>>data_w_%s' % ( name, name ), '( %s && Type==1 )' % Selection )
      Tree.Draw( '%s>>mc_w_%s' % ( name, name ), '( %s && Type==0 )*Weight' % Selection )
      locals()[ 'ratio_w_%s' % name ] = DoPlot( name, 'Weighted', title, locals()[ 'data_w_%s' % name ], locals()[ 'mc_w_%s' % name ] )

  if options.create:
    data2d = ROOT.TH2F( 'data2d', '', 100, 0, 150, 100, -2.6, 2.6 )
    mc2d = ROOT.TH2F( 'mc2d', '', 100, 0, 150, 100, -2.6, 2.6 )
    Tree.Draw( 'Pair_y:Pair_Pt>>data2d', '%s && Type==1' % Selection )
    Tree.Draw( 'Pair_y:Pair_Pt>>mc2d', '%s && Type==0' % Selection )
    Out = ROOT.TFile( 'hists.root', 'recreate' )
    mc2d.Scale( data2d.Integral() / mc2d.Integral() )
    ratio2d = GetRatio2D( data2d, mc2d )
    data2d.Write()
    mc2d.Write()
    ratio2d.Write()
    Out.Close()

def DoPlot( Name, Name2, Title, Data, MC ):
  Can = ROOT.TCanvas( 'plot_%s_%s' % ( Name2, Name ), '', 800, 800 ) 
  Top = ROOT.TPad( 'Top', '', 0.0, 0.3, 1.0, 1.0 )
  Top.SetTopMargin( 0.05 )
  Top.SetBottomMargin( 0.03 )
  Top.Draw()
  Bottom = ROOT.TPad( 'Bottom', '', 0.0, 0.0, 1.0, 0.3 )
  Bottom.SetTopMargin( 0.00 )
  Bottom.SetBottomMargin( 0.35 )
  Bottom.Draw()
  ### Top Pad
  Top.cd()
  MC.Scale( Data.Integral() / MC.Integral() )
  MC.SetMaximum( 1.2 * max( MC.GetMaximum(), Data.GetMaximum() ) )
  MC.GetXaxis().SetLabelColor( ROOT.kWhite )
  MC.GetXaxis().SetTitleColor( ROOT.kWhite )
  MC.GetYaxis().SetTitle( 'Events' )
  MC.GetYaxis().SetTitleOffset( 2.2 )
  MC.SetFillColor( ROOT.kOrange + 7 )
  MC.Draw()
  Data.Draw( 'esame' )
  Lat = ROOT.TLatex()
  Lat.SetNDC()
  Lat.SetTextFont( 42 )
  Lat.SetTextSize( 0.05 )
  Lat.SetTextColor( 1 )
  Lat.SetTextAlign( 12 )
  Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
  Lat.SetTextSize( 0.045 )
  Lat.DrawLatex( 0.2, 0.83, '#sqrt{s} = 8 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % 4 ) 
  Lat.SetTextSize( 0.035 )
  Leg = ROOT.TLegend( 0.7, 0.9, 0.85, 0.75 )
  Leg.SetFillStyle( 0 )
  Leg.SetBorderSize( 0 )
  Leg.SetTextFont( 42 )
  Leg.SetTextSize( 0.04 )
  Leg.AddEntry( Data, 'Data', 'pe' )
  Leg.AddEntry( MC, 'Z #rightarrow #mu#mu', 'f' )
  Leg.Draw()
  ### Bottom Pad
  Bottom.cd()
  Ratio = GetRatio( Data, MC ) 
  Ratio.GetXaxis().SetTitle( Title )
  Ratio.GetXaxis().SetTitleOffset( 4 )
  Ratio.GetYaxis().SetTitleOffset( 2.2 )
  Ratio.Draw()
  Bottom.Update()
  Line = ROOT.TLine( Bottom.GetUxmin(), 1, Bottom.GetUxmax(), 1 )
  Line.SetLineWidth( 3 )
  Line.SetLineStyle( 2 )
  Line.Draw()
  Ratio.Draw( 'same' )
  Can.SaveAs( '.pdf' )
  return Ratio

def GetRatio( HistNum, HistDen ):
  Result = HistNum.Clone( 'RatioHistogramFor' + HistNum.GetName() )
  AverageRatio = 0
  NumberOfPoints = 0
  for i in range( 1, Result.GetNbinsX() ):
    if HistDen.GetBinContent( i ) > 0:
      Result.SetBinContent( i, HistNum.GetBinContent( i ) / HistDen.GetBinContent( i ) )
      Result.SetBinError( i, math.sqrt( math.pow( HistNum.GetBinError( i ) / HistDen.GetBinContent( i ), 2 ) + math.pow( HistNum.GetBinContent( i ) * HistDen.GetBinError( i ) / math.pow( HistDen.GetBinContent( i ), 2 ), 2 ) ) )
      AverageRatio += HistNum.GetBinContent( i ) / HistDen.GetBinContent( i )
      NumberOfPoints += 1.
  Result.SetLineStyle( 1 );
  Result.SetLineColor( ROOT.kBlack );
  Result.SetMarkerSize( 1 );
  Result.GetYaxis().SetNdivisions( 5, 3, 0 )
  Result.SetMaximum( 1.23 )
  Result.SetMinimum( 0.77 )
  Result.GetYaxis().SetTitle( '#frac{Data}{MonteCarlo}' );
  Result.GetYaxis().CenterTitle( True )
  return Result

def GetRatio2D( HistNum, HistDen ):
  Result = HistNum.Clone( 'Ratio2d' )
  for i in range( 1, Result.GetNbinsX() ):
    for j in range( 1, Result.GetNbinsY() ):
      if HistDen.GetBinContent( i, j ) > 0:
        Result.SetBinContent( i, j, HistNum.GetBinContent( i, j ) / HistDen.GetBinContent( i, j ) )
        Result.SetBinError( i, j, math.sqrt( math.pow( HistNum.GetBinError( i, j ) / HistDen.GetBinContent( i, j ), 2 ) + math.pow( HistNum.GetBinContent( i, j ) * HistDen.GetBinError( i, j ) / math.pow( HistDen.GetBinContent( i, j ), 2 ), 2 ) ) )
  return Result

if __name__ == '__main__':
  sys.exit( main() )
