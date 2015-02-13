# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import AtlasStyle
import sys
import GetAverageError
import PlottingTools

def main():

  MyDict = {}
  Cases = {  5: '/home/gartoni/Work/MuonCalibrationFit/python/rTest5InvFb_3/Corrections-rTest5InvFb_3.root',
            10: '/home/gartoni/Work/MuonCalibrationFit/python/rTest10InvFb_3/Corrections-rTest10InvFb_3.root',
            50: '/home/gartoni/Work/MuonCalibrationFit/python/rTest50InvFb_3/Corrections-rTest50InvFb_3.root' }
  TheKeys = [] 
  for Key in Cases:
    MyDict[ Key ] = GetAverageError.GetAverage( Cases[ Key ] )
    TheKeys = [ sKey for sKey in MyDict[ Key ] ] 

  NewDict = {}
  for Var in TheKeys:
    SmallDict = {}
    for Key in MyDict:
      for NewVar in MyDict[ Key ]:
        if NewVar == Var:
          SmallDict[ Key ] = MyDict[ Key ][ NewVar ]
    NewDict[ Var ] = SmallDict

  for Var in NewDict:
    C = ROOT.TCanvas( 'average_error_%s' % Var, '', 600, 600 )
    G = ROOT.TGraphErrors( len( Cases ) )
    G.SetMarkerSize( 0.8 )
    G.SetMarkerColor( ROOT.kRed + 1 )
    G.SetLineColor( ROOT.kRed + 1 )
    for Index, Key in enumerate( NewDict[ Var ] ):
      G.SetPoint( Index, Key, NewDict[ Var ][ Key ][ 0 ] )
      G.SetPointError( Index, 0, NewDict[ Var ][ Key ][ 1 ] )
    G.Draw( 'ap' )
    G.GetXaxis().SetTitle( 'Luminosity [fb^{-1}]' )
    G.GetYaxis().SetTitle( 'Error on %s' % PlottingTools.Convert( Var, '' ) )
    #G.GetYaxis().SetLimits( 0, G.GetYaxis().GetXmax() )
    G.SetMinimum( 0 )
    G.Draw( 'ap' )
    ROOT.gPad.Modified()
    ROOT.gPad.Update()
    Lat = ROOT.TLatex()
    Lat.SetNDC()
    Lat.SetTextFont( 42 )
    Lat.SetTextSize( 0.05 )
    Lat.SetTextColor( 1 )
    Lat.SetTextAlign( 32 )
    Lat.DrawLatex( 0.9, 0.9, '#bf{#it{ATLAS}} Internal' ) 
    Lat.SetTextSize( 0.035 )
    Lat.DrawLatex( 0.9, 0.7, 'Injected value for %s #rightarrow %s' % ( PlottingTools.Convert( Var, '' ), GetTrueValue( Var ) ) ) 
    Lat.SetTextSize( 0.03 )
    Lat.DrawLatex( 0.9, 0.6, '(statistical component only)' )
    C.SaveAs( '.pdf' )

def GetTrueValue( Var ):
  if Var == 's1':
    return '-0.001'
  if Var == 'p1':
    return '0.01'
  if Var == 'p2':
    return '0.0005'

if __name__ == '__main__':
  sys.exit( main() )
