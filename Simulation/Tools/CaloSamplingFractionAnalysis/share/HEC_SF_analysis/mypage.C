/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
#include "TCanvas.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TPad.h"

TPad *pad;

void mypage (Char_t const *tt="Text to Test", Int_t ix=1, Int_t iy=1) 
{
//
//  Page title
//
   TPaveText *title = new TPaveText(0.01,0.955,0.99,0.995,"NDC");
///   title->SetFillColor(18);
   title->SetFillColor(10);
   title->SetTextFont(72);
   title->AddText(tt);
   title->SetBorderSize(0);
   title->Draw();
//
//  Main pad
//
///   pad = new TPad("pad","MainPad", 0.005,0.005, 0.995,0.950, 20,0,0);   
   pad = new TPad("pad","MainPad", 0.005,0.005, 0.995,0.950, 10,0,0);   
//
//  Divide main pad
//
///   pad->Divide(ix,iy, 0.005, 0.005, 18);
///   pad->Divide(ix,iy, 0.000, 0.000, 10);
   pad->Divide(ix,iy, 0.0001, 0.0001, 10);
   pad->Draw();
//
}
////////////////////////////////////////////////////////////////////////

