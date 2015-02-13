# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, os, re, sys, ROOT, AtlasStyle
from optparse import OptionParser

def main():
  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 0
  reg = re.compile( 'test_time_output_split_(\d+).root' )
  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-f', '--folder',  dest = 'folder',  action = 'store', type = 'string', default = '/afs/cern.ch/user/g/gartoni/FitTest/RunDir/', help = '' )
  ( options, args ) = parser.parse_args()
  FileNames = [ Name for Name in os.listdir( options.folder ) if 'root' in Name ]
  List = []
  for FileName in FileNames:
    File = ROOT.TFile( options.folder + FileName )
    Match = reg.match( FileName )
    if Match:
      List.append( ( float( Match.group( 1 ) ), File.Get( 'ProcessingTime1' )[ 0 ] ) )
  List = sorted( List, key = lambda Pair: Pair[ 0 ] )
  X = array.array( 'd', [ Pair[ 0 ] for Pair in List ] )
  Y = array.array( 'd', [ Pair[ 1 ] for Pair in List ] )
  Canvas = ROOT.TCanvas( 'Times', '', 600, 600 )
  G = ROOT.TGraph( len( X ), X, Y )
  G.SetLineWidth( 3 )
  G.SetLineColor( ROOT.kRed + 1 )
  G.SetMarkerColor( ROOT.kRed + 1 )
  G.SetLineStyle( 8 )
  F = ROOT.TF1( 'F', '[0] + [1]*x + [2]*x*x', 0, 1000 ) 
  F.SetLineColor( ROOT.kRed + 2 )
  F.SetLineStyle( 8 )
  G.Draw( 'ap' )
  G.GetXaxis().SetTitle( 'Number of bin splittings' )
  G.GetYaxis().SetTitle( 'Time (seconds)' )
  G.Fit( 'F' ) 
  G.Draw( 'ap' )
  F.Draw( 'same' )
  legend = ROOT.TLegend( 0.2, 0.9, 0.6, 0.6 )
  legend.SetFillStyle( 0 )
  legend.SetBorderSize( 0 )
  legend.SetTextFont( 42 )
  legend.SetTextSize( 0.035 )
  legend.AddEntry( G, 'Function evaluation time', 'p' )
  legend.AddEntry( F, 'a + bx + cx^{2} fit', 'l' )
  legend.AddEntry( None, 'a = %.8f' % F.GetParameter( 0 ), '' )
  legend.AddEntry( None, 'b = %.8f' % F.GetParameter( 1 ), '' )
  legend.AddEntry( None, 'c = %.8f' % F.GetParameter( 2 ), '' )
  legend.Draw()
  Canvas.SaveAs( '.pdf' )
    
if __name__ == '__main__':
  sys.exit( main() )
