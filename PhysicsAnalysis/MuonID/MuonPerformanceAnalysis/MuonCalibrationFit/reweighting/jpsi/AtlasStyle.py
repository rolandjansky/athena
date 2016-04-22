# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

class AtlasStyle( ROOT.TStyle ):

  def __init__( self, name = 'AtlasStyle', title = 'ATLAS style object' ):
    ROOT.TStyle.__init__( self, name, title )
    self.SetName( name )
    self.SetTitle( title )
    self.configure()
    return

  def configure( self ):
    self.Info( 'configure', 'Configuring default ATLAS style' )
    icol = 0
    self.SetFrameBorderMode( 0 )
    self.SetFrameFillColor( icol )
    self.SetFrameFillStyle( 0 )
    self.SetCanvasBorderMode( 0 )
    self.SetPadBorderMode( 0 )
    self.SetPadColor( icol )
    self.SetCanvasColor( icol )
    self.SetStatColor( icol )
    self.SetPaperSize( 20, 26 )
    self.SetPadTopMargin( 0.05 )
    self.SetPadRightMargin( 0.065 )
    self.SetPadBottomMargin( 0.16 )
    self.SetPadLeftMargin( 0.145 )
    self.SetTitleXOffset(1.4);
    self.SetTitleYOffset(1.4);
    font_type = 43
    font_size = 26
    self.SetTextFont( font_type )
    self.SetTextSize( font_size )
    self.SetLabelFont( font_type, 'x' )
    self.SetLabelSize( font_size - 1, 'x' )
    self.SetTitleFont( font_type, 'x' )
    self.SetTitleSize( font_size, 'x' )
    self.SetLabelFont( font_type, 'y' )
    self.SetLabelSize( font_size - 1, 'y' )
    self.SetTitleFont( font_type, 'y' )
    self.SetTitleSize( font_size, 'y' )
    self.SetLabelFont( font_type, 'z' )
    self.SetLabelSize( font_size - 1, 'z' )
    self.SetTitleFont( font_type, 'z' )
    self.SetTitleSize( font_size, 'z' )
    self.SetMarkerStyle( 20 )
    self.SetMarkerSize( 1.2 )
    self.SetHistLineWidth( 2 )
    self.SetLineStyleString( 2, '[12 12]' )
    self.SetOptTitle( 0 )
    self.SetOptStat( 0 )
    self.SetOptFit( 0 )
    self.SetPadTickX( 1 )
    self.SetPadTickY( 1 )
    return

style = AtlasStyle()
ROOT.SetOwnership( style, False )
ROOT.gROOT.SetStyle( style.GetName() )
ROOT.gROOT.ForceStyle()
ROOT.TGaxis.SetMaxDigits( 3 )
