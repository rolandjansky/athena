# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, re, sys, ROOT, AtlasStyle
from optparse import OptionParser

def main():
  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 0
  reg = re.compile( 'output_split_(\d+)_random_(\d+).root' )
  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-i', '--inputs',  dest = 'inputs',  action = 'store', type = 'string', default = '', help = '' )
  parser.add_option( '-f', '--folder',  dest = 'folder',  action = 'store', type = 'string', default = '/home/gartoni/Work/RunDir/', help = '' )
  parser.add_option( '-p', '--prefix',  dest = 'prefix',  action = 'store', type = 'string', default = '', help = '' )
  ( options, args ) = parser.parse_args()
  FileNames = options.inputs.split( ',' )
  Values = {}
  Graphs = {}
  Vars = [ 's1', 'p1', 'p2' ]
  #Vars = [ 's1' ]
  Colors = [ ROOT.kRed + 1, ROOT.kAzure + 2, ROOT.kOrange + 7, ROOT.kBlack, ROOT.kViolet - 5, ROOT.kPink + 2 ]
  ProcTime = {}
  Min = {}
  Max = {}
  for Index, FileName in enumerate( FileNames ):
    File = ROOT.TFile( options.folder + FileName )
    Tree = File.Get( 'tree_SingleRegion' )
    Values[ Index ] = {}
    ProcTime[ Index ] = 0 #File.Get( 'ProcessingTime2' )[ 0 ]
    for Var in Vars: 
      Values[ Index ][ Var ] = []
      Min[ Var ] = 1000000
      Max[ Var ] = -1000000
    MaxIteration = 0
    for Entry in Tree:
      MaxIteration = max( MaxIteration, Entry.scan_iter )
    for Entry in Tree:
      for SecondIndex, Var in enumerate( Vars ): 
        if Entry.scan_par == SecondIndex and Entry.scan_iter == MaxIteration:
          Values[ Index ][ Var ].append( ( getattr( Entry, Var ), Entry.chi2 ) )
          print Var, getattr( Entry, Var ), Entry.chi2
          Min[ Var ] = min( Min[ Var ], getattr( Entry, Var ) )
          Max[ Var ] = max( Max[ Var ], getattr( Entry, Var ) )
  for Index in Values:
    Graphs[ Index ] = {}
    for Var in Vars:
      MyList = sorted( Values[ Index ][ Var ], key = lambda Pair: Pair[ 0 ] )
      X = array.array( 'd', [ Pair[ 0 ] for Pair in MyList ] )
      Ytemp = array.array( 'd', [ Pair[ 1 ] for Pair in MyList ] )
      Y = array.array( 'd', [ Val - min( Ytemp ) for Val in Ytemp ] )
      Graphs[ Index ][ Var ] = ROOT.TGraph( len( X ), X, Y )
  for Var in Vars: 
    Canvas = ROOT.TCanvas( '%s_Scan_%s' % ( options.prefix, Var ), '', 600, 600 )
    TempGraphs = [ Graphs[ Index ][ Var ] for Index in range( len( Graphs ) ) ]
    Leg = ROOT.TLegend( 0.16, 0.95, 0.9, 0.75 )
    Leg.SetFillStyle( 1000 )
    Leg.SetFillColor( 0 )
    Leg.SetBorderSize( 0 )
    Leg.SetTextFont( 42 )
    Leg.SetTextSize( 0.035 )
    for I, G in enumerate( TempGraphs ):
      G.SetLineWidth( 2 )
      G.SetLineColor( Colors[ I ] )
      G.SetLineStyle( Index )
      if I == 0:
        G.Draw( 'al' )
        G.GetXaxis().SetLimits( Min[ Var ], Max[ Var ] )
        G.SetMaximum( 5 )
        G.GetXaxis().SetTitle( Convert( Var ) ) 
        G.GetYaxis().SetTitle( '#Delta#chi^{2}' ) 
        G.Draw( 'al' )
      else:
        G.Draw( 'l same' )
      Match = reg.match( FileNames[ I ] )
      if Match:
        Leg.AddEntry( G, 'N_{Split} = %d, N_{Rnd} = %d, Time(sec) = %.1f' % ( int( Match.group( 1 ) ), int( Match.group( 2 ) ), ProcTime[ I ] ), 'l' )
    #Leg.Draw()
    Canvas.SaveAs( '.pdf' )

def Convert( Name ):
  if Name == 's1':
    return 's_{1}'
  if Name == 'p1':
    return 'p_{1}'
  if Name == 'p2':
    return 'p_{2}'

def GetMax( graph ):
  myMax = 0
  for Bin in range( graph.GetN() ):
    X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
    graph.GetPoint( Bin, X, Y )
    Err = graph.GetErrorYhigh( Bin )
    myMax = max( [Y + Err, myMax ] )
  return myMax

if __name__ == '__main__':
  sys.exit( main() )
