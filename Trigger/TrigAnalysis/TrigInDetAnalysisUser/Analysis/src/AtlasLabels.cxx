/**
 **     @file    AtlasLabels.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#include "AtlasLabels.h"

#include "TLatex.h"
#include "TLine.h"
#include "TPave.h"
#include "TPad.h"
#include "TMarker.h"


#include <string>

void ATLASLabel(Double_t x,Double_t y, const std::string& text, Color_t color, int nx, int ny ) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);

  double delx = 0.115*696*gPad->GetWh()*nx/(472*gPad->GetWw()*ny);

  l.DrawLatex(x,y,"ATLAS");
  if (text!="") {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.DrawLatex(x+delx,y,text.c_str());
    //    p.DrawLatex(x,y,"#sqrt{s}=900GeV");
  }
}


void Label(Double_t x,Double_t y, const std::string& text, Color_t color, int nx, int ny ) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);

  double delx = 0.115*696*gPad->GetWh()*nx/(472*gPad->GetWw()*ny);

  if (text!="") {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.DrawLatex(x+delx,y,text.c_str());
    //    p.DrawLatex(x,y,"#sqrt{s}=900GeV");
  }
}




void ATLASLabelOld(Double_t x,Double_t y,bool Preliminary,Color_t color) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
  if (Preliminary) {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.DrawLatex(x+0.115,y,"Preliminary");
  }
}



void ATLASVersion( const std::string&  version, Double_t x, Double_t y, Color_t color ) 
{

  if (version!="") {
    char versionString[100];
    sprintf(versionString,"Version %s",version.c_str());
    TLatex l;
    l.SetTextAlign(22); 
    l.SetTextSize(0.04); 
    l.SetNDC();
    l.SetTextFont(72);
    l.SetTextColor(color);
    l.DrawLatex(x,y,versionString);
  }
}

