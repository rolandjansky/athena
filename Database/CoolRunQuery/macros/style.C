/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1.h"
#include "TStyle.h"
#include "/Users/andreashoecker/ROOT/colours.C"

#ifndef STYLE__
#define STYLE__

// set frame styles
void SetFrameStyle( TH1* frame, Float_t scale = 1.0 )
{
   frame->SetLabelOffset( 0.012, "X" );// label offset on x axis
   frame->SetLabelOffset( 0.012, "Y" );// label offset on x axis
   frame->GetXaxis()->SetTitleOffset( 1.25 );
   frame->GetYaxis()->SetTitleOffset( 0.94 );
   frame->GetXaxis()->SetTitleSize( 0.045*scale );
   frame->GetYaxis()->SetTitleSize( 0.045*scale );
   Float_t labelSize = 0.04*scale;
   frame->GetXaxis()->SetLabelSize( labelSize );
   frame->GetYaxis()->SetLabelSize( labelSize );

   // global style settings
   gPad->SetTicks();
   gPad->SetBottomMargin( 0.120*scale  );

   // some style elements
   gStyle->SetTitleFillColor( c_VLightGray );
   gStyle->SetTitleTextColor( c_VDarkGray );
   gStyle->SetTitleBorderSize( 1 );
   gStyle->SetTitleH( 0.05 );
   gStyle->SetTitleX( gPad->GetLeftMargin() );
   gStyle->SetTitleY( 1 - gPad->GetTopMargin() + gStyle->GetTitleH() );
   gStyle->SetTitleW( 1 - gPad->GetLeftMargin() - gPad->GetRightMargin() );
}

#endif
