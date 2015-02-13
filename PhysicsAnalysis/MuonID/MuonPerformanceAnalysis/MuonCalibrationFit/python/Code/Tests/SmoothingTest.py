# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, ROOT, AtlasStyle

def main():
  File = ROOT.TFile( '../../RunDir/output.root' )
  Keys = ROOT.gDirectory.GetListOfKeys()
  Canvas = ROOT.TCanvas( 'test_smoothing', '', 600, 600 )
  Hists = {}
  Colors = [ ROOT.kRed + 1, ROOT.kAzure + 2, ROOT.kBlack, ROOT.kOrange + 7, ROOT.kGreen, ROOT.kViolet ]
  Leg = ROOT.TLegend( 0.2, 0.9, 0.4, 0.6 )
  for Key in Keys:
    Name = Key.GetName()
    #if 'Test' in Name and '___SingleRegion__MassJpsi__MC__9_0_pt_11_0' in Name:
    if 'Test' in Name and '___SingleRegion__MassZ__MC__30_0_pt_40_0' in Name:
      Hists[ Name ] = File.Get( Name )
  Max = max( [ Hists[ Key ].GetMaximum() for Key in Hists ] )
  for Index, Key in enumerate( Hists ):
    Hist = Hists[ Key ]
    Hist.SetLineColor( Colors[ Index ] ) 
    if Index == 0:
      Hist.SetMaximum( Max )
      Hist.Draw( 'hist' )
    else:
      Hist.Draw( 'hist same' )
    Leg.AddEntry( Hist, Hist.GetName().replace( '___SingleRegion__MassZ__MC__30_0_pt_40_0', '' ), 'l' )
    #Leg.AddEntry( Hist, Hist.GetName().replace( '___SingleRegion__MassJpsi__MC__9_0_pt_11_0', '' ), 'l' )
  Leg.Draw()
  #Canvas.SetLogy()
  Canvas.SaveAs( '.pdf' )

if __name__ == '__main__':
  sys.exit( main() )
