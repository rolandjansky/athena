/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ###########################################
//            Author: Dengfeng Zhang
//            dengfeng.zhang@cern.ch
// ###########################################
// local includes
#include "SetAttributes.h"
// if want to call following functions, do following
// from ROOT import *; gROOT.ProcessLine("#include \"HistToolKit.h\"")
int DrawLabel(float xstart, float ystart, string label)
{
  // (xstart, ystart): coornidate of the leftbottom of the pavetext
  TLatex m_latex;
  m_latex.SetTextSize(0.04) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(42) ;
  m_latex.DrawLatex(xstart, ystart, label.c_str()) ;
  return 0;
}
int DrawLabels(float xstart, float ystart, string label="")
{
  TLatex m_latex;
  m_latex.SetTextSize(0.04) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(22) ;
  m_latex.DrawLatex(xstart, ystart, label.c_str()) ;
  return 0;
}
#ifndef DrawATLASLabels_Func
#define DrawATLASLabels_Func
int DrawATLASLabels(float xstart, float ystart, int labeltype)
{
  TLatex m_latex;
  m_latex.SetTextSize(0.04) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(72) ;
  m_latex.DrawLatex(xstart, ystart, "ATLAS") ;
  m_latex.SetTextFont(42) ;
  float spacing = 0.13 ;
  switch(labeltype)
  {
    case 0:
          break ;
    case 1:
          m_latex.DrawLatex(xstart+spacing, ystart, "Preliminary") ;
          break ;
    case 2:
          m_latex.DrawLatex(xstart+spacing, ystart, "Internal") ;
          break ;
    case 3:
          m_latex.DrawLatex(xstart+spacing, ystart, "Simulation Preliminary") ;
          break ;
    case 4:
          m_latex.DrawLatex(xstart+spacing, ystart, "Work in Progress") ;
          break ;
   default: break ;
  }
  return 0;
}
#endif
int SetGraphDrawOption(TGraph *gr, bool isdrawline=true, bool isdrawbar=false)
{
  if(!isdrawline&&!isdrawbar)
  {  gr->Draw("AP") ; return 0 ;}
  if(isdrawline)
  {  gr->Draw("ALP") ; return 0 ;}
  if(isdrawbar)
    gr->Draw("AB") ;
  return 0 ;
}

int DrawSingleGraphOnCanvas(string canvasname, TGraph *gr, string drawoption="APL" ,
                   bool logx=false, bool logy=false, bool isrectangle=true)
{  // better not to set drawline or drawbar to be 1 at the same time
  // define canvas
  gStyle->SetOptTitle(0) ;
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set attributes of graph
  SetGraphAttributes(gr, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  // draw graph
  gr->Draw(drawoption.c_str()) ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawTwoGraphsOnCanvas(string canvasname, TGraph *gr1, TGraph *gr2, 
                   string drawoption1="APL", string drawoption2="PL", 
                   bool logx=false, bool logy=false, bool isrectangle=true)
{  // better not to set drawline or drawbar to be 1 at the same time
  // define canvas
  gStyle->SetOptTitle(0) ;
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set attributes of graph
  SetGraphAttributes(gr1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphAttributes(gr2, 1, 2, 2, 1001, 2, 21, 2, 1) ;

  // draw graph
  gr1->Draw(drawoption1.c_str()) ;
  gr2->Draw(drawoption2.c_str()) ;
  // draw TLegend
  TLegend *leg=new TLegend(0.55,0.7,0.89,0.89) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gr1, gr1->GetTitle(), "LP") ;
  leg->AddEntry(gr2, gr2->GetTitle(), "LP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawThreeGraphsOnCanvas(string canvasname, TGraph *gr1, TGraph *gr2, TGraph *gr3,
                   string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",
                   bool logx=false, bool logy=false, bool isrectangle=true)
{  // better not to set drawline or drawbar to be 1 at the same time
  // define canvas
  gStyle->SetOptTitle(0) ;
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set attributes of graph
  SetGraphAttributes(gr1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphAttributes(gr2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphAttributes(gr3, 1, 2, 3, 1001, 3, 22, 3, 1) ;

  // draw graph
  gr1->Draw(drawopt1.c_str()) ;
  gr2->Draw(drawopt2.c_str()) ;
  gr3->Draw(drawopt3.c_str()) ;
  // draw TLegend
  TLegend *leg=new TLegend(0.55,0.7,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gr1, gr1->GetTitle(), "LP") ;
  leg->AddEntry(gr2, gr2->GetTitle(), "LP") ;
  leg->AddEntry(gr3, gr3->GetTitle(), "LP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawFourGraphsOnCanvas(string canvasname, TGraph *gr1, TGraph *gr2, TGraph *gr3,TGraph *gr4,
                   string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",string drawopt4="PL",
                   bool logx=false, bool logy=false, bool isrectangle=true)
{  // better not to set drawline or drawbar to be 1 at the same time
  // define canvas
  gStyle->SetOptTitle(0) ;
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set attributes of graph
  SetGraphAttributes(gr1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphAttributes(gr2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphAttributes(gr3, 1, 2, 3, 1001, 3, 22, 3, 1) ;
  SetGraphAttributes(gr4, 1, 2, 4, 1001, 4, 23, 4, 1) ;
  // draw graph
  gr1->Draw(drawopt1.c_str()) ;
  gr2->Draw(drawopt2.c_str()) ;
  gr3->Draw(drawopt3.c_str()) ;
  gr4->Draw(drawopt4.c_str()) ;
  // draw TLegend
  TLegend *leg=new TLegend(0.55,0.5,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gr1, gr1->GetTitle(), "LP") ;
  leg->AddEntry(gr2, gr2->GetTitle(), "LP") ;
  leg->AddEntry(gr3, gr3->GetTitle(), "LP") ;
  leg->AddEntry(gr4, gr4->GetTitle(), "LP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}
int DrawFiveGraphsOnCanvas(string canvasname, TGraph *gr1, TGraph *gr2, TGraph *gr3,TGraph *gr4,TGraph *gr5,
string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",string drawopt4="PL",string drawopt5="PL",
bool logx=false, bool logy=false, bool isrectangle=true)
{  // better not to set drawline or drawbar to be 1 at the same time
  // define canvas
  gStyle->SetOptTitle(0) ;
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set attributes of graph
  SetGraphAttributes(gr1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphAttributes(gr2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphAttributes(gr3, 1, 2, 3, 1001, 3, 22, 3, 1) ;
  SetGraphAttributes(gr4, 1, 2, 4, 1001, 4, 23, 4, 1) ;
  SetGraphAttributes(gr5, 1, 2, 5, 1001, 5, 24, 5, 1) ;
  // draw graph
  gr1->Draw(drawopt1.c_str()) ;
  gr2->Draw(drawopt2.c_str()) ;
  gr3->Draw(drawopt3.c_str()) ;
  gr4->Draw(drawopt4.c_str()) ;
  gr5->Draw(drawopt5.c_str()) ;
  // draw TLegend
  TLegend *leg=new TLegend(0.55,0.5,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gr1, gr1->GetTitle(), "LP") ;
  leg->AddEntry(gr2, gr2->GetTitle(), "LP") ;
  leg->AddEntry(gr3, gr3->GetTitle(), "LP") ;
  leg->AddEntry(gr4, gr4->GetTitle(), "LP") ;
  leg->AddEntry(gr5, gr5->GetTitle(), "LP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int SetGraphErrorsDrawOption(TGraphErrors *gre, bool isdrawline=true, bool isdrawerrorrectangle=false, bool isdrawerrorband=false)
{
  if(!isdrawline&&!isdrawerrorrectangle&&!isdrawerrorband)
  {  gre->Draw("AP") ; return 0 ;}
  if(isdrawline)
  {  gre->Draw("APL") ; return 0 ;}
  if(isdrawerrorrectangle)
  {
    gre->Draw("A2") ;
    gre->Draw("P") ;
    return 0 ;
  }
  if(isdrawerrorband)
  {
    gre->Draw("A3") ;
    gre->Draw("LX") ;
  }
  return 0 ;
}

int DrawSingleGraphErrorsOnCanvas(string canvasname, TGraphErrors *gre, string drawoption="AP", bool logx=false, bool logy=false, bool isrectangle=true, string label="")
{
  //gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;
  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  DrawLabels(0.5,0.8,label.c_str()) ;

  interpad->cd() ;
  // set options of grapherrors
  SetGraphErrorsAttributes(gre, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  gre->Draw(drawoption.c_str()) ;
  //DrawLabel(0.15,0.15,"#font[22]{ATLAS} #font[12]{Internal}") ;
  // draw legend
  TLegend *leg=new TLegend(0.6,0.7,0.8,0.8) ;
  leg->SetTextSize(0.04) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gre, gre->GetTitle(), "LP") ;
  //leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  return 0 ;
}

int DrawTwoGraphErrorsOnCanvas(string canvasname, TGraphErrors *gre1, TGraphErrors *gre2,
                   string drawoption1="APL", string drawoption2="PL",
                   bool logx=false, bool logy=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;
  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  //DrawLabels(0.5,0.8,label.c_str()) ;

  interpad->cd() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(gre1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphErrorsAttributes(gre2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(gre1, drawoption1.c_str()) ;
  mg->Add(gre2, drawoption2.c_str()) ;
  mg->Draw("A") ;
  gPad->Modified();
  float max=0, min=100000 ;
  for (int i=0 ; i<gre1->GetN() ; i++)
  {
    if(gre1->GetY()[i]>=max) max=gre1->GetY()[i] ;
    if(gre1->GetY()[i]<=min) min=gre1->GetY()[i] ;
  }
  //mg->SetMinimum(min*0.9) ;
  //mg->SetMaximum(max*1.1) ;
  SetMGraphAttributes(mg) ;
  mg->GetXaxis()->SetTitle(gre1->GetXaxis()->GetTitle()) ;
  mg->GetYaxis()->SetTitle(gre1->GetYaxis()->GetTitle()) ;
  // draw legend
  TLegend *leg=new TLegend(0.6,0.7,0.9,0.9) ;
  if(gre1->GetY()[0]<gre1->GetY()[1])
  {
    leg->SetX1(0.5) ;
    leg->SetX2(0.9) ;
    leg->SetY1(0.3) ;
    leg->SetY2(0.5) ;
  }
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gre1, gre1->GetTitle(), "P") ;
  leg->AddEntry(gre2, gre2->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  return 0 ;
}

int DrawThreeGraphErrorsOnCanvas(string canvasname, 
                   TGraphErrors *gre1, TGraphErrors *gre2, TGraphErrors *gre3,
                   string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",
                   bool logx=false, bool logy=false, bool isrectangle=true, string header="", string label="")
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;
  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  //DrawLabels(0.4,0.8,label.c_str()) ;
  DrawATLASLabels(0.25,0.85,2) ;

  interpad->cd() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(gre1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphErrorsAttributes(gre2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphErrorsAttributes(gre3, 1, 2, 3, 1001, 3, 22, 3, 1) ;
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(gre1, drawopt1.c_str()) ;
  mg->Add(gre2, drawopt2.c_str()) ;
  mg->Add(gre3, drawopt3.c_str()) ;
  mg->Draw("A") ;
  gPad->Modified();
  float max=0, min=100000 ;
  for (int i=0 ; i<gre1->GetN() ; i++)
  {
    if(gre1->GetY()[i]>=max) max=gre1->GetY()[i] ;
    if(gre1->GetY()[i]<=min) min=gre1->GetY()[i] ;
  }
  mg->SetMinimum(min*0.9) ;
  mg->SetMaximum(max*1.1) ;
  SetMGraphAttributes(mg) ;
  mg->GetXaxis()->SetTitle(gre1->GetXaxis()->GetTitle()) ;
  mg->GetYaxis()->SetTitle(gre1->GetYaxis()->GetTitle()) ;
  // draw legend
  TLegend *leg=new TLegend(0.55,0.65,0.9,0.9) ;
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetTextSize(0.05);
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First(); 
  theader->SetTextFont(22);
  leg->AddEntry(gre1, gre1->GetTitle(), "P") ;
  leg->AddEntry(gre2, gre2->GetTitle(), "P") ;
  leg->AddEntry(gre3, gre3->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;
}

int DrawFourGraphErrorsOnCanvas(string canvasname, 
   TGraphErrors *gre1, TGraphErrors *gre2, TGraphErrors *gre3, TGraphErrors *gre4,
   string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",string drawopt4="PL",
   bool logx=false, bool logy=false, bool isrectangle=false, string header="", string label="")
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;
  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  //DrawLabels(0.3,0.85,label.c_str()) ;
  DrawATLASLabels(0.25,0.85,2) ;

  interpad->cd() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(gre1, 1, 2, 2, 1001, 1, 21, 2, 1) ;
  SetGraphErrorsAttributes(gre2, 1, 2, 3, 1001, 2, 22, 3, 1) ;
  SetGraphErrorsAttributes(gre3, 1, 2, 4, 1001, 3, 23, 4, 1) ;
  SetGraphErrorsAttributes(gre4, 1, 2, 6, 1001, 4, 24, 6, 1) ;
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(gre1, drawopt1.c_str()) ;
  mg->Add(gre2, drawopt2.c_str()) ;
  mg->Add(gre3, drawopt3.c_str()) ;
  mg->Add(gre4, drawopt4.c_str()) ;
  mg->Draw("A") ;
  gPad->Modified();
  gPad->Update();
  SetMGraphAttributes(mg) ;
  mg->GetXaxis()->SetTitle(gre1->GetXaxis()->GetTitle()) ;
  mg->GetYaxis()->SetTitle(gre1->GetYaxis()->GetTitle()) ;
  if(canvasname.find("pi")!=std::string::npos)
  {
    if(canvasname.find("Ratio")!=std::string::npos)
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.76, 1.26) ;
        mg->GetYaxis()->SetNdivisions(510) ;
      }
      if(canvasname.find("Resolution")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.91, 1.41) ;
        mg->GetYaxis()->SetNdivisions(510) ;
      }
      if(canvasname.find("Response")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.96, 1.11) ;
        mg->GetYaxis()->SetNdivisions(503) ;
      }
    }
    else
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.025, 0.055) ;
        mg->GetYaxis()->SetNdivisions(503) ;
      }
    }
  }
  else
  {
    if(canvasname.find("Ratio")!=std::string::npos)
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.94, 1.39) ;
        mg->GetYaxis()->SetNdivisions(509) ;
      }
      if(canvasname.find("Resolution")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.81, 1.36) ;
        mg->GetYaxis()->SetNdivisions(511) ;
      }
      if(canvasname.find("Response")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.91, 1.16) ;
        mg->GetYaxis()->SetNdivisions(505) ;
      }
    }
    else
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.025, 0.065) ;
        mg->GetYaxis()->SetNdivisions(504) ;
      }
    }
  }
  // draw legend
  TLegend *leg=new TLegend(0.55,0.6,0.9,0.9) ;
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetTextSize(0.04);
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First(); 
  theader->SetTextFont(22);
  if(canvasname.find("Response")!=std::string::npos&&canvasname.find("Ratio")==std::string::npos)
  {
    leg->SetX1(0.55) ;
    leg->SetX2(0.9) ;
    leg->SetY1(0.25) ;
    leg->SetY2(0.5) ;
  }
  leg->AddEntry(gre1, gre1->GetTitle(), "P") ;
  leg->AddEntry(gre2, gre2->GetTitle(), "P") ;
  leg->AddEntry(gre3, gre3->GetTitle(), "P") ;
  leg->AddEntry(gre4, gre4->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;
}

int DrawFiveGraphErrorsOnCanvas(string canvasname, 
TGraphErrors *gre1, TGraphErrors *gre2, TGraphErrors *gre3, TGraphErrors *gre4, TGraphErrors *gre5, string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",string drawopt4="PL",string drawopt5="PL",
bool logx=false, bool logy=false, bool isrectangle=false, string header="", string label="")
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;
  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  //DrawLabels(0.3,0.85,label.c_str()) ;
  DrawATLASLabels(0.25,0.85,2) ;

  interpad->cd() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(gre1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphErrorsAttributes(gre2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphErrorsAttributes(gre3, 1, 2, 3, 1001, 3, 22, 3, 1) ;
  SetGraphErrorsAttributes(gre4, 1, 2, 4, 1001, 4, 23, 4, 1) ;
  SetGraphErrorsAttributes(gre5, 1, 2, 6, 1001, 5, 24, 6, 1) ;
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(gre1, drawopt1.c_str()) ;
  mg->Add(gre2, drawopt2.c_str()) ;
  mg->Add(gre3, drawopt3.c_str()) ;
  mg->Add(gre4, drawopt4.c_str()) ;
  mg->Add(gre5, drawopt5.c_str()) ;
  mg->Draw("A") ;
  SetMGraphAttributes(mg) ;
  mg->GetXaxis()->SetTitle(gre1->GetXaxis()->GetTitle()) ;
  mg->GetYaxis()->SetTitle(gre1->GetYaxis()->GetTitle()) ;
  if(canvasname.find("pi")!=std::string::npos)
  {
    if(canvasname.find("Ratio")!=std::string::npos)
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.76, 1.16) ;
        mg->GetYaxis()->SetNdivisions(508) ;
      }
      if(canvasname.find("Resolution")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.91, 1.21) ;
        mg->GetYaxis()->SetNdivisions(506) ;
      }
      if(canvasname.find("Response")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.96, 1.16) ;
        mg->GetYaxis()->SetNdivisions(504) ;
      }
    }
    else
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.025, 0.055) ;
        mg->GetYaxis()->SetNdivisions(503) ;
      }
    }
  }
  else
  {
    if(canvasname.find("Ratio")!=std::string::npos)
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.94, 1.39) ;
        mg->GetYaxis()->SetNdivisions(509) ;
      }
      if(canvasname.find("Resolution")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.81, 1.21) ;
        mg->GetYaxis()->SetNdivisions(508) ;
      }
      if(canvasname.find("Response")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.91, 1.26) ;
        mg->GetYaxis()->SetNdivisions(507) ;
      }
    }
    else
    {
      if(canvasname.find("LateralSpread")!=std::string::npos)
      {
        mg->GetYaxis()->SetRangeUser(0.025, 0.065) ;
        mg->GetYaxis()->SetNdivisions(504) ;
      }
    }
  }
  gPad->Modified();
  // draw legend
  TLegend *leg=new TLegend(0.55,0.55,0.9,0.9) ;
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetTextSize(0.04);
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  if(canvasname.find("Response")!=std::string::npos&&canvasname.find("Ratio")==std::string::npos)
  {
    leg->SetX1(0.55) ;
    leg->SetX2(0.9) ;
    leg->SetY1(0.2) ;
    leg->SetY2(0.5) ;
  }
  leg->AddEntry(gre1, gre1->GetTitle(), "P") ;
  leg->AddEntry(gre2, gre2->GetTitle(), "P") ;
  leg->AddEntry(gre3, gre3->GetTitle(), "P") ;
  leg->AddEntry(gre4, gre4->GetTitle(), "P") ;
  leg->AddEntry(gre5, gre5->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;
}

int DrawTopFiveGraphErrorsAndBottomFourGraphErrorsOnCanvas(string canvasname, 
TGraphErrors *topgre1, TGraphErrors *topgre2, TGraphErrors *topgre3, TGraphErrors *topgre4, TGraphErrors *topgre5, TGraphErrors *bottomgre1, TGraphErrors *bottomgre2, TGraphErrors *bottomgre3, TGraphErrors *bottomgre4,string topdrawopt1="APL", string topdrawopt2="PL", string topdrawopt3="PL",string topdrawopt4="PL",string topdrawopt5="PL", string bottomdrawopt1="APL", string bottomdrawopt2="PL", string bottomdrawopt3="PL",string bottomdrawopt4="PL",
bool logx=false, bool toplogy=false, bool bottomlogy=false, bool isrectangle=false, string header="")
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;
  outpad->SetFillStyle(4000) ;
  TPad *interpad1 = new TPad("intpad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *interpad2 = new TPad("intpad2","pad2",0,0,1,0.33) ;// For residuals histo
  SetPadAttributes(interpad1, 0, 0, 0.15, 0.05, 0.00001, 0.05, 0, logx, toplogy) ;
  SetPadAttributes(interpad2, 0, 0, 0.15, 0.05, 0.3, 0.00001, 0, logx, bottomlogy) ;

  interpad1->Draw() ;
  interpad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  DrawATLASLabels(0.23,0.9,1) ;
  DrawLabel(0.23,0.85,"Tile Calorimeter") ;

  interpad1->cd() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(topgre1, 1, 2, 1, 1001, 1, 20, 1, 1, 0.05, 1.4, 0.05, 0.05, 1.4, 0.05) ;
  SetGraphErrorsAttributes(topgre2, 1, 2, 2, 1001, 2, 21, 2, 1, 0.05, 1.4, 0.05, 0.05, 1.4, 0.05) ;
  SetGraphErrorsAttributes(topgre3, 1, 2, 3, 1001, 3, 22, 3, 1, 0.05, 1.4, 0.05, 0.05, 1.4, 0.05) ;
  SetGraphErrorsAttributes(topgre4, 1, 2, 4, 1001, 4, 23, 4, 1, 0.05, 1.4, 0.05, 0.05, 1.4, 0.05) ;
  SetGraphErrorsAttributes(topgre5, 1, 2, 6, 1001, 6, 24, 6, 1, 0.05, 1.4, 0.05, 0.05, 1.4, 0.05) ;
  TMultiGraph *topmg = new TMultiGraph();
  topmg->Add(topgre1, topdrawopt1.c_str()) ;
  topmg->Add(topgre2, topdrawopt2.c_str()) ;
  topmg->Add(topgre3, topdrawopt3.c_str()) ;
  topmg->Add(topgre4, topdrawopt4.c_str()) ;
  topmg->Add(topgre5, topdrawopt5.c_str()) ;
  // Draw
  topmg->Draw("A") ;
  SetMGraphAttributes(topmg, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  topmg->GetXaxis()->SetTitle(topgre2->GetXaxis()->GetTitle()) ;
  topmg->GetYaxis()->SetTitle(topgre2->GetYaxis()->GetTitle()) ;
  if(canvasname.find("pi")!=std::string::npos)
  {
    if(canvasname.find("LateralSpread")!=std::string::npos)
    {
      topmg->GetYaxis()->SetRangeUser(0.025, 0.055) ;
      topmg->GetYaxis()->SetNdivisions(503) ;
    }
  }
  else
  {
    if(canvasname.find("LateralSpread")!=std::string::npos)
    {
      topmg->GetYaxis()->SetRangeUser(0.03, 0.055) ;
      topmg->GetYaxis()->SetNdivisions(503) ;
    }
  }
  gPad->Modified();
  // draw legend
  TLegend *leg=new TLegend(0.55,0.45,0.9,0.9) ;
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetTextSize(0.05);
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  if(canvasname.find("Response")!=std::string::npos)
  {
    leg->SetX1(0.55) ;
    leg->SetX2(0.9) ;
    leg->SetY1(0.05) ;
    leg->SetY2(0.5) ;
  }
  leg->AddEntry(topgre1, topgre1->GetTitle(), "P") ;
  leg->AddEntry(topgre2, topgre2->GetTitle(), "P") ;
  leg->AddEntry(topgre3, topgre3->GetTitle(), "P") ;
  leg->AddEntry(topgre4, topgre4->GetTitle(), "P") ;
  leg->AddEntry(topgre5, topgre5->GetTitle(), "P") ;
  leg->Draw("same") ;

  interpad2->cd() ;

  // set options of bottom grapherrors
  SetGraphErrorsAttributes(bottomgre1, 1, 2, 2, 1001, 2, 21, 2, 1, 0.12, 1.2, 0.1, 0.12, 0.7, 0.1) ;
  SetGraphErrorsAttributes(bottomgre2, 1, 2, 3, 1001, 3, 22, 3, 1, 0.12, 1.2, 0.1, 0.12, 0.7, 0.1) ;
  SetGraphErrorsAttributes(bottomgre3, 1, 2, 4, 1001, 4, 23, 4, 1, 0.12, 1.2, 0.1, 0.12, 0.7, 0.1) ;
  SetGraphErrorsAttributes(bottomgre4, 1, 2, 6, 1001, 6, 24, 6, 1, 0.12, 1.2, 0.1, 0.12, 0.7, 0.1) ;
  TMultiGraph *bottommg = new TMultiGraph();
  bottommg->Add(bottomgre1, bottomdrawopt1.c_str()) ;
  bottommg->Add(bottomgre2, bottomdrawopt2.c_str()) ;
  bottommg->Add(bottomgre3, bottomdrawopt3.c_str()) ;
  bottommg->Add(bottomgre4, bottomdrawopt4.c_str()) ;
  bottommg->Draw("A") ;
  SetMGraphAttributes(bottommg, 0.12, 1.4, 0.1, 0.12, 0.7, 0.1) ;
  bottommg->GetXaxis()->SetTitle(bottomgre1->GetXaxis()->GetTitle()) ;
  bottommg->GetYaxis()->SetTitle(bottomgre1->GetYaxis()->GetTitle()) ;

  c->Update() ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;

}

int DrawSixGraphErrorsOnCanvas(string canvasname, TGraphErrors *gre1, TGraphErrors *gre2, TGraphErrors *gre3, TGraphErrors *gre4,TGraphErrors *gre5, TGraphErrors *gre6, string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",string drawopt4="PL",string drawopt5="PL",string drawopt6="PL", bool logx=false, bool logy=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;

  // set options of grapherrors
  SetGraphErrorsAttributes(gre1, 1, 2, 1, 1001, 1, 20, 1, 1) ;
  SetGraphErrorsAttributes(gre2, 1, 2, 2, 1001, 2, 21, 2, 1) ;
  SetGraphErrorsAttributes(gre3, 1, 2, 3, 1001, 3, 22, 3, 1) ;
  SetGraphErrorsAttributes(gre4, 1, 2, 4, 1001, 4, 23, 4, 1) ;
  SetGraphErrorsAttributes(gre5, 1, 2, 5, 1001, 5, 24, 5, 1) ;
  SetGraphErrorsAttributes(gre6, 1, 2, 6, 1001, 6, 25, 6, 1) ;
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(gre1, drawopt1.c_str()) ;
  mg->Add(gre2, drawopt2.c_str()) ;
  mg->Add(gre3, drawopt3.c_str()) ;
  mg->Add(gre4, drawopt4.c_str()) ;
  mg->Add(gre5, drawopt5.c_str()) ;
  mg->Add(gre6, drawopt6.c_str()) ;
  mg->Draw("A") ;
  gPad->Modified();
  float max=0, min=100000 ;
  for (int i=0 ; i<gre1->GetN() ; i++)
  {
    if(gre1->GetY()[i]>=max) max=gre1->GetY()[i] ;
    if(gre1->GetY()[i]<=min) min=gre1->GetY()[i] ;
  }
  mg->SetMinimum(min*0.8) ;
  mg->SetMaximum(max*1.2) ;
  SetMGraphAttributes(mg) ;
  mg->GetXaxis()->SetTitle(gre1->GetXaxis()->GetTitle()) ;
  mg->GetYaxis()->SetTitle(gre1->GetYaxis()->GetTitle()) ;
  // draw legend
  TLegend *leg=new TLegend(0.5,0.5,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(gre1, gre1->GetTitle(), "P") ;
  leg->AddEntry(gre2, gre2->GetTitle(), "P") ;
  leg->AddEntry(gre3, gre3->GetTitle(), "P") ;
  leg->AddEntry(gre4, gre4->GetTitle(), "P") ;
  leg->AddEntry(gre5, gre5->GetTitle(), "P") ;
  leg->AddEntry(gre6, gre6->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  return 0 ;
}

int SetGraphAsymmErrorsDrawOption(TGraphAsymmErrors *grae, bool isdrawline=true, bool isdrawerrorrectangle=false, bool isdrawerrorband=false)
{
  if(!isdrawline&&!isdrawerrorrectangle&&!isdrawerrorband)
  {  grae->Draw("AP") ; return 0 ;}
  if(isdrawline)
  {  grae->Draw("APL") ; return 0 ;}
  if(isdrawerrorrectangle)
  { 
    grae->Draw("A2") ;
    grae->Draw("P") ;
    return 0 ;
  }
  if(isdrawerrorband)
  { 
    grae->Draw("A3") ;
    grae->Draw("LX") ;
  }
  return 0 ;
}

int DrawSigleAsymmGraphErrorsOnCanvas(string canvasname, TGraphAsymmErrors *agre, 
                   string drawoption="APL" , bool logx=false, bool logy=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set options of grapherror
  SetGraphAsymmErrorsAttributes(agre, 1, 2, 1, 1001, 1, 1, 20, 1) ;
  agre->Draw(drawoption.c_str()) ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawTwoAsymmGraphErrorsOnCanvas(string canvasname, TGraphAsymmErrors *agre1, TGraphAsymmErrors *agre2, 
                   string drawopt1="APL", string drawopt2="PL",
                   bool logx=false, bool logy=false, 
                   bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx();
  if(logy) c->SetLogy();
  // set options of grapherror
  SetGraphAsymmErrorsAttributes(agre1, 1, 2, 1, 1001, 1, 20, 1) ;
  SetGraphAsymmErrorsAttributes(agre2, 1, 2, 2, 1001, 2, 21, 2) ;
  agre1->Draw(drawopt1.c_str()) ;
  agre2->Draw(drawopt2.c_str()) ;
  // draw legend
  TLegend *leg=new TLegend(0.55,0.7,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(agre1, agre1->GetTitle(), "LEP") ;
  leg->AddEntry(agre2, agre2->GetTitle(), "LEP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawThreeAsymmGraphErrorsOnCanvas(string canvasname, TGraphAsymmErrors *agre1, TGraphAsymmErrors *agre2, TGraphAsymmErrors *agre3, string drawopt1="APL", string drawopt2="PL", string drawopt3="PL",
                   bool logx=false, bool logy=false, 
                   bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set options of grapherror
  SetGraphAsymmErrorsAttributes(agre1, 1, 2, 1, 1001, 1, 20, 1) ;
  SetGraphAsymmErrorsAttributes(agre2, 1, 2, 2, 1001, 2, 21, 2) ;
  SetGraphAsymmErrorsAttributes(agre3, 1, 2, 3, 1001, 3, 22, 3) ;
  agre1->Draw(drawopt1.c_str()) ;
  agre2->Draw(drawopt2.c_str()) ;
  agre3->Draw(drawopt3.c_str()) ;
  // draw legend
  TLegend *leg=new TLegend(0.55,0.6,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(agre1, agre1->GetTitle(), "LEP") ;
  leg->AddEntry(agre2, agre2->GetTitle(), "LEP") ;
  leg->AddEntry(agre3, agre3->GetTitle(), "LEP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawFourAsymmGraphErrorsOnCanvas(string canvasname, TGraphAsymmErrors *agre1, TGraphAsymmErrors *agre2, TGraphAsymmErrors *agre3, TGraphAsymmErrors *agre4,string drawopt1="APL", string drawopt2="PL", string drawopt3="PL", string drawopt4="PL", bool logx=false, bool logy=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set options of grapherror
  SetGraphAsymmErrorsAttributes(agre1, 1, 2, 1, 1001, 1, 20, 1) ;
  SetGraphAsymmErrorsAttributes(agre2, 1, 2, 2, 1001, 2, 21, 2) ;
  SetGraphAsymmErrorsAttributes(agre3, 1, 2, 3, 1001, 3, 22, 3) ;
  SetGraphAsymmErrorsAttributes(agre4, 1, 2, 4, 1001, 4, 23, 4) ;
  agre1->Draw(drawopt1.c_str()) ;
  agre2->Draw(drawopt2.c_str()) ;
  agre3->Draw(drawopt3.c_str()) ;
  agre4->Draw(drawopt4.c_str()) ;
  // draw legend
  TLegend *leg=new TLegend(0.55,0.5,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(agre1, agre1->GetTitle(), "LEP") ;
  leg->AddEntry(agre2, agre2->GetTitle(), "LEP") ;
  leg->AddEntry(agre3, agre3->GetTitle(), "LEP") ;
  leg->AddEntry(agre4, agre4->GetTitle(), "LEP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawFiveAsymmGraphErrorsOnCanvas(string canvasname, TGraphAsymmErrors *agre1, TGraphAsymmErrors *agre2, TGraphAsymmErrors *agre3, TGraphAsymmErrors *agre4,TGraphAsymmErrors *agre5,string drawopt1="APL", string drawopt2="PL", string drawopt3="PL", string drawopt4="PL", string drawopt5="PL",bool logx=false, bool logy=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0) ;
  // define canvas
  float height = 600, width = 600 ;
  if(isrectangle)
    width = 800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  c->SetTickx() ;
  c->SetTicky() ;
  if(logx) c->SetLogx() ;
  if(logy) c->SetLogy() ;
  // set options of grapherror
  SetGraphAsymmErrorsAttributes(agre1, 1, 2, 1, 1001, 1, 20, 1) ;
  SetGraphAsymmErrorsAttributes(agre2, 1, 2, 2, 1001, 2, 21, 2) ;
  SetGraphAsymmErrorsAttributes(agre3, 1, 2, 3, 1001, 3, 22, 3) ;
  SetGraphAsymmErrorsAttributes(agre4, 1, 2, 4, 1001, 4, 23, 4) ;
  SetGraphAsymmErrorsAttributes(agre5, 1, 2, 5, 1001, 5, 23, 5) ;
  agre1->Draw(drawopt1.c_str()) ;
  agre2->Draw(drawopt2.c_str()) ;
  agre3->Draw(drawopt3.c_str()) ;
  agre4->Draw(drawopt4.c_str()) ;
  agre5->Draw(drawopt5.c_str()) ;
  // draw legend
  TLegend *leg=new TLegend(0.55,0.5,0.89,0.89) ;
  leg->SetTextSize(0.03) ;
  leg->SetFillStyle(0) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(agre1, agre1->GetTitle(), "LEP") ;
  leg->AddEntry(agre2, agre2->GetTitle(), "LEP") ;
  leg->AddEntry(agre3, agre3->GetTitle(), "LEP") ;
  leg->AddEntry(agre4, agre4->GetTitle(), "LEP") ;
  leg->AddEntry(agre5, agre5->GetTitle(), "LEP") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}
