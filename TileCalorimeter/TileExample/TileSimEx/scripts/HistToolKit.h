/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ##########################################
//          Author: Dengfeng Zhang
//          dengfeng.zhang@cern.ch
// ##########################################
// std include
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
// local include 
#include "SetAttributes.h"
// if want to call following functions, do following
// from ROOT import *; gROOT.ProcessLine("#include \"HistToolKit.h\"")

// define some functions in head
int DrawLabels(float xstart, float ystart, string label, bool isbold=true) ;
int DrawATLASLabels(float xstart, float ystart, int labelstyle) ;
int DrawCMEAndLumi(float xstart, float ystart, string lumiInFb, string CME) ;
bool sort_by_histmax(TH1F* hist1, TH1F* hist2) ;
bool sort_by_vechistmax(std::pair<TH1F*, bool> p1, std::pair<TH1F*, bool> p2) ;

TH1F* bookTH1F(std::string name, std::string title,
               std::string xlabel,std::string ylabel,
               int xbins, double xlow, double xhigh,
               bool sumw2 = true, bool overflow=true)
{
  TH1F* tmp = new TH1F( name.c_str(), title.c_str(), xbins, xlow, xhigh);
  tmp->GetXaxis()->SetTitle(xlabel.c_str()) ;
  tmp->GetYaxis()->SetTitle(ylabel.c_str()) ;
  if(sumw2) tmp->Sumw2() ; 
  if(overflow) tmp->StatOverflows() ;
  return tmp ;
}
TH1F* bookTH1F(string name, string title,
               string xlabel,string ylabel,
               int nbins, float *bininterval,
               bool sumw2 = true)
{
  TH1F* tmp = new TH1F( name.c_str(), title.c_str(), nbins, bininterval);
  tmp->GetXaxis()->SetTitle(xlabel.c_str()) ;
  tmp->GetYaxis()->SetTitle(ylabel.c_str()) ;
  if(sumw2) tmp->Sumw2() ;
  tmp->StatOverflows() ;
  return tmp ;
}

TH1F* bookTH1F(string name, string title,
               string xlabel,string ylabel,
               TH1F* hist,
               bool sumw2 = true)
{
  TH1F* tmp = new TH1F( name.c_str(), title.c_str(), hist->GetNbinsX(), hist->GetXaxis()->GetXbins()->GetArray());
  tmp->GetXaxis()->SetTitle(xlabel.c_str()) ;
  tmp->GetYaxis()->SetTitle(ylabel.c_str()) ;
  if(sumw2) tmp->Sumw2() ;
  tmp->StatOverflows() ;
  return tmp ;
}

TH1I* bookTH1I(std::string name, std::string title,
               std::string xlabel,std::string ylabel,
               int xbins, double xlow, double xhigh,
               bool sumw2 = true)
{
  TH1I* tmp = new TH1I( name.c_str(), title.c_str(), xbins, xlow, xhigh);
  tmp->GetXaxis()->SetTitle(xlabel.c_str()) ;
  tmp->GetYaxis()->SetTitle(ylabel.c_str()) ;
  if(sumw2) tmp->Sumw2() ;
  tmp->StatOverflows() ;
  return tmp ;
}

// Get Effective Histogram
TH1F* getEffectiveEntriesHistogram(TH1F* hist, string name = "hee")
{
  std::cout<<"Get Effective Entries."<<std::endl ;
  TH1F *hee = new TH1F(name.c_str(), name.c_str(), hist->GetXaxis()->GetNbins(), 
                       hist->GetXaxis()->GetXbins()->GetArray()) ;
  hee->SetDirectory(0) ;
  for(int bin=1 ; bin<hist->GetNbinsX()+1 ; bin++)
  {
    float nee ;
    if (hist->GetBinError(bin) != 0.)
        nee = pow(hist->GetBinContent(bin),2)/pow(hist->GetBinError(bin),2) ;
    else
        nee = 0. ;
    hee->SetBinContent(bin, nee);
    hee->SetBinError(bin, sqrt(nee)) ;
  }
  return hee ;
}
// Get DataLike Histogram
TH1F* getDataLikeHist(TH1F* eff, TH1F* scaled, string name, int jobSeed = 10 )
{
  std::cout<<"Get DataLike Entries."<<std::endl ;
  TH1F *dataLike = new TH1F(name.c_str(), name.c_str(), eff->GetXaxis()->GetNbins(),
                            eff->GetXaxis()->GetXbins()->GetArray()) ;
  dataLike->SetDirectory(0) ;
  //random number generator
  TRandom3 rand3(1986) ; //1986 #ORIGINAL

  //loop over bins
  for( int bin=1 ; bin<eff->GetNbinsX()+1 ; bin++)
  {
    //enough effective entries?
    float nee = eff->GetBinContent(bin) ;
    if(nee>=scaled->GetBinContent(bin))
    {
      //set seed
      //NOTE the seed for each bin must be always the same
      int binSeed = int(eff->GetBinCenter(bin) + jobSeed*1e5 );
      rand3.SetSeed(binSeed);
      //get data-like bin content by drawing entries
      //NOTE weights are poissonian by construction
      for( int jj=1 ; jj<=int(nee) ; jj++)
        if(rand3.Uniform()<(float)scaled->GetBinContent(bin)/nee)
          dataLike->Fill(dataLike->GetBinCenter(bin)) ;
    }
  }
  return dataLike ;
}


// Get map from binary file
int GetNumberOfEvents(std::string filename, std::map<string, int> *channelvsbumber)
{
  ifstream fileIn(filename.c_str());
  std::string line;
  std::string subStr;
  std::string channel ;
  int number ;
  while (getline(fileIn, line))
  {
    istringstream iss(line);
    iss >> channel ;
    if( channel.find_first_of('#')==0 )
      continue ;
    iss >> subStr;
    iss >> subStr;
    iss >> subStr;
    sscanf(subStr.c_str(), "%i", &number);
    (*channelvsbumber)[channel]=number ;
    cout << "channelvsbumber[" << channel << "]: " << (*channelvsbumber)[channel] << endl;
  }
  return 0 ;
}

int DrawSingleHistOnCanvas(string canvasname, TH1F* hist, string drawoption="PE", bool logx=false, bool logy=true, bool isrectangle=false, string label="")
{
  gStyle->SetOptTitle(0);
  float height=600,width=600 ;
  if(isrectangle)
    width=800 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  //DrawLabels(0.5, 0.8, label) ;
  // Draw ATLAS Label
  DrawATLASLabels(0.2, 0.8, 2) ;
  // draw center of mass energy and integrated luminosity
  //DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;
  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist, 1, 2, 1, 1001, 0, 20, 1, 1) ;
  // draw histogram
  hist->Draw(drawoption.c_str()) ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  return 0 ;
}
int SetHistDrawOption(TH1F* hist, TLegend *leg, bool DrawHistorNot=false)
{
  if(DrawHistorNot)
  {
    hist->Draw("HISTSAME") ;
    leg->AddEntry(hist, hist->GetTitle(), "L") ;
  }
  else
  {
    hist->Draw("PESAME") ;
    leg->AddEntry(hist, hist->GetTitle(), "LPE") ;
  }
  leg->Draw("same") ;
  return 0 ;
}
int SetHistDrawOption(std::vector< std::pair< TH1F*, bool > > histvec, TLegend *leg)
{
  for(std::vector< std::pair< TH1F*, bool > >::iterator itr=histvec.begin() ; itr!=histvec.end() ; ++itr)
  {
    if(itr->second)
    { 
      itr->first->Draw("HISTSAME") ;
      leg->AddEntry(itr->first, itr->first->GetTitle(), "L") ;
    }
    else
    {
      itr->first->Draw("PESAME") ;
      leg->AddEntry(itr->first, itr->first->GetTitle(), "LPE") ;
    }
  }
  leg->Draw("same") ;
  return 0 ;
}
int DrawTwoHistsOnCanvas(string canvasname, TH1F* hist1, TH1F* hist2, string drawoption1="HIST", string drawoption2="HISTSame", bool logx=false, bool logy=true, bool isrectangle=true)
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  DrawATLASLabels(0.2, 0.8, 2) ;
  // draw center of mass energy and integrated luminosity
  //DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;

  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist1, 1, 2, 1, 0, 1, 20, 1) ;
  SetHistAttributes(hist2, 1, 2, 2, 0, 1, 22, 2) ;
  hist1->Draw(drawoption1.c_str()) ;
  hist2->Draw(drawoption2.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.5,0.7,0.89,0.89) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.04) ;
  leg->SetBorderSize(0) ;
  if(drawoption1.find("HIST")!=string::npos)
    leg->AddEntry(hist1, hist1->GetTitle(), "L") ;
  else
    leg->AddEntry(hist1, hist1->GetTitle(), "LPE") ;
  if(drawoption2.find("HIST")!=string::npos)
    leg->AddEntry(hist2, hist2->GetTitle(), "L") ;
  else
    leg->AddEntry(hist2, hist2->GetTitle(), "LPE") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  return 0 ;
}

int DrawThreeHistsOnCanvas(string canvasname, TH1F* hist1, TH1F* hist2, TH1F* hist3, string drawopt1="PE",string drawopt2="PESame", string drawopt3="PESame", bool logx=false, bool logy=true, bool isrectangle=true, string header="", string label="")
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  if(canvasname.find("LongitudinalProfile")!=std::string::npos&&canvasname.find("Ratio")==std::string::npos)
    DrawLabels(0.2, 0.4, label, true) ;
  else
    DrawLabels(0.2, 0.85, label, true) ;
  // Draw ATLAS Label
  DrawATLASLabels(0.2, 0.25, 2) ;
  // draw center of mass energy and integrated luminosity
//  DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;

  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist1, 1, 2, 3, 3004, 1, 22, 3, 1) ;
  SetHistAttributes(hist2, 1, 2, 4, 3005, 1, 23, 4, 1) ;
  SetHistAttributes(hist3, 1, 2, 6, 3006, 1, 24, 6, 1) ;
  // Draw Histograms
  hist1->Draw(drawopt1.c_str()) ;
  hist2->Draw(drawopt2.c_str()) ;
  hist3->Draw(drawopt3.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.57,0.65,0.92,0.9) ;
  string PhysicsLists[4] = {"FTFP_BERT", "FTFP_BERT_ATL", "QGSP_BERT", "QGSP_BIC"} ;
  for(int i=0 ; i<4 ; i++)
  { 
    if(canvasname.find(PhysicsLists[i])!=std::string::npos)
    {
      leg->SetX1(0.65) ;
      leg->SetX2(0.9) ;
      leg->SetY1(0.65) ;
      leg->SetY2(0.9) ;
    }
  }
  leg->SetTextFont(42) ;
  leg->SetTextAlign(22);
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.04) ;
  leg->SetBorderSize(0) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  leg->AddEntry(hist1, hist1->GetTitle(), "P") ;
  leg->AddEntry(hist2, hist2->GetTitle(), "P") ;
  leg->AddEntry(hist3, hist3->GetTitle(), "P") ;
  leg->Draw("same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;
}

int DrawFourHistsOnCanvas(string canvasname, TH1F* hist1, TH1F* hist2, TH1F* hist3, TH1F* hist4, string drawopt1="PE",string drawopt2="PESame", string drawopt3="PESame",string drawopt4="PESame",bool logx=false, bool logy=true, bool isrectangle=false, string header="", string label="")
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  if(canvasname.find("LongitudinalProfile")!=std::string::npos&&canvasname.find("Ratio")==std::string::npos)
    DrawLabels(0.2, 0.4, label, true) ;
  else
    DrawLabels(0.2, 0.85, label, true) ;
  //Draw ATLAS Label
  DrawATLASLabels(0.2, 0.25, 2) ;
  //draw center of mass energy and integrated luminosity
  //DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;

  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist1, 1, 2, 2, 0, 1, 21, 2, 1) ;
  SetHistAttributes(hist2, 1, 2, 3, 0, 1, 22, 3, 1) ;
  SetHistAttributes(hist3, 1, 2, 4, 0, 1, 23, 4, 1) ;
  SetHistAttributes(hist4, 1, 2, 6, 0, 1, 24, 6, 1) ;
  // Draw Histograms
  hist1->Draw(drawopt1.c_str()) ;
  hist2->Draw(drawopt2.c_str()) ;
  hist3->Draw(drawopt3.c_str()) ;
  hist4->Draw(drawopt4.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.57,0.6,0.92,0.9) ;
  string PhysicsLists[4] = {"FTFP_BERT", "FTFP_BERT_ATL", "QGSP_BERT", "QGSP_BIC"} ;
  for(int i=0 ; i<4 ; i++)
  { 
    if(canvasname.find(PhysicsLists[i])!=std::string::npos)
    {
      leg->SetX1(0.65) ;
      leg->SetX2(0.9) ;
      leg->SetY1(0.6) ;
      leg->SetY2(0.9) ;
    }
  }
  leg->SetTextFont(42) ;
  leg->SetTextAlign(22);
  leg->SetBorderSize(0) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.04) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  leg->AddEntry(hist1, hist1->GetTitle(), "P") ;
  leg->AddEntry(hist2, hist2->GetTitle(), "P") ;
  leg->AddEntry(hist3, hist3->GetTitle(), "P") ;
  leg->AddEntry(hist4, hist4->GetTitle(), "P") ;
  leg->Draw("Same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;

  return 0 ;
}

int DrawFiveHistsOnCanvas(string canvasname, TH1F* hist1, TH1F* hist2, TH1F* hist3, TH1F* hist4, TH1F* hist5,
string drawopt1="PE",string drawopt2="PESame",string drawopt3="PESame",string drawopt4="PESame", string drawopt5="PESame",bool logx=false, bool logy=true, bool isrectangle=true, string header="", string label="")
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.16, 0.04, 0.15, 0.05, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  if(canvasname.find("LongitudinalProfile")!=std::string::npos&&canvasname.find("Ratio")==std::string::npos)
    DrawLabels(0.2, 0.4, label, true) ;
  else
    DrawLabels(0.2, 0.85, label, true) ;
  // Draw ATLAS Label
  DrawATLASLabels(0.2, 0.25, 2) ;
  // draw center of mass energy and integrated luminosity
//  DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;

  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist1, 1, 2, 1, 0, 1, 20, 1, 1) ;
  SetHistAttributes(hist2, 1, 2, 2, 0, 1, 21, 2, 1) ;
  SetHistAttributes(hist3, 1, 2, 3, 0, 1, 22, 3, 1) ;
  SetHistAttributes(hist4, 1, 2, 4, 0, 1, 23, 4, 1) ;
  SetHistAttributes(hist5, 1, 2, 6, 0, 1, 24, 6, 1) ;
  // Draw Histograms
  hist1->Draw(drawopt1.c_str()) ;
  hist2->Draw(drawopt2.c_str()) ;
  hist3->Draw(drawopt3.c_str()) ;
  hist4->Draw(drawopt4.c_str()) ;
  hist5->Draw(drawopt5.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.57,0.55,0.92,0.9) ;
  string PhysicsLists[4] = {"FTFP_BERT", "FTFP_BERT_ATL", "QGSP_BERT", "QGSP_BIC"} ;
  for(int i=0 ; i<4 ; i++)
  { 
    if(canvasname.find(PhysicsLists[i])!=std::string::npos)
    {
      leg->SetX1(0.65) ;
      leg->SetX2(0.9) ;
      leg->SetY1(0.55) ;
      leg->SetY2(0.9) ;
    }
  }
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetBorderSize(0) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.04) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  leg->AddEntry(hist1, hist1->GetTitle(), "P") ;
  leg->AddEntry(hist2, hist2->GetTitle(), "P") ;
  leg->AddEntry(hist3, hist3->GetTitle(), "P") ;
  leg->AddEntry(hist4, hist4->GetTitle(), "P") ;
  leg->AddEntry(hist5, hist5->GetTitle(), "P") ;
  leg->Draw("Same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0 ;
}

int DrawTopFiveHistsAndBottomFourHistsOnCanvas(string canvasname, TH1F *top1, TH1F* top2, TH1F* top3, TH1F* top4, TH1F* top5, TH1F* bottom1, TH1F* bottom2, TH1F* bottom3, TH1F* bottom4, string topdrawopt1 = "PE",string topdrawopt2 = "PESAME", string topdrawopt3 = "PE",string topdrawopt4 = "PESAME", string topdrawopt5 = "PESAME", string bottomdrawopt1="PE", string bottomdrawopt2="PESame", string bottomdrawopt3="PESame", string bottomdrawopt4="PESame",bool logx=false, bool toplogy=true, bool bottomlogy=false, bool isrectangle=false, string header="", string label="")
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad1 = new TPad("intpad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *interpad2 = new TPad("intpad2","pad2",0,0,1,0.33) ;// For residuals histo
  //outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(interpad1, 0, 0, 0.15, 0.05, 0.00001, 0.05, 0, logx, toplogy) ;
  SetPadAttributes(interpad2, 0, 0, 0.15, 0.05, 0.3, 0.00001, 0, logx, bottomlogy) ;

  interpad1->Draw() ;
  interpad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  DrawLabels(0.65, 0.65, label, true) ;
  // Draw ATLAS Label
  DrawATLASLabels(0.2, 0.45, 1) ;
  DrawLabels(0.2, 0.4, "Tile Calorimeter", false) ;
 
  interpad1->cd() ;
  // Set histogram attributes
  SetHistAttributes(top1, 1, 2, 1, 0, 1, 20, 1, 1, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  SetHistAttributes(top2, 1, 2, 2, 0, 1, 21, 2, 1, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  SetHistAttributes(top3, 1, 2, 3, 0, 1, 22, 3, 1, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  SetHistAttributes(top4, 1, 2, 4, 0, 1, 23, 4, 1, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  SetHistAttributes(top5, 1, 2, 6, 0, 1, 24, 6, 1, 0.06, 1.2, 0.06, 0.06, 1.2, 0.06) ;
  // Draw Histograms
  top1->Draw(topdrawopt1.c_str()) ;
  top2->Draw(topdrawopt2.c_str()) ;
  top3->Draw(topdrawopt3.c_str()) ;
  top4->Draw(topdrawopt4.c_str()) ;
  top5->Draw(topdrawopt5.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.55,0.54,0.9,0.94) ;
  string PhysicsLists[4] = {"FTFP_BERT", "FTFP_BERT_ATL", "QGSP_BERT", "QGSP_BIC"} ;
  for(int i=0 ; i<4 ; i++)
  { 
    if(canvasname.find(PhysicsLists[i])!=std::string::npos)
    {
      leg->SetX1(0.6) ;
      leg->SetX2(0.9) ;
      leg->SetY1(0.45) ;
      leg->SetY2(0.9) ;
    }
  }
  leg->SetTextFont(42);
  leg->SetTextAlign(22);
  leg->SetBorderSize(0) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.05) ;
  leg->SetHeader(header.c_str()) ;
  TLegendEntry *theader = (TLegendEntry*)leg->GetListOfPrimitives()->First();
  theader->SetTextFont(22) ;
  leg->AddEntry(top1, top1->GetTitle(), "P") ;
  leg->AddEntry(top2, top2->GetTitle(), "P") ;
  leg->AddEntry(top3, top3->GetTitle(), "P") ;
  leg->AddEntry(top4, top4->GetTitle(), "P") ;
  leg->AddEntry(top5, top5->GetTitle(), "P") ;
  leg->Draw("Same") ;

  interpad2->cd() ;
  // set bottom histograms attributes
  SetHistAttributes(bottom1, 1, 2, 2, 0, 1, 21, 2, 1, 0.12, 1.4, 0.1, 0.12, 0.7, 0.1) ;
  SetHistAttributes(bottom2, 1, 2, 3, 0, 1, 22, 3, 1, 0.12, 1.4, 0.1, 0.12, 0.7, 0.1) ;
  SetHistAttributes(bottom3, 1, 2, 4, 0, 1, 23, 4, 1, 0.12, 1.4, 0.1, 0.12, 0.7, 0.1) ;
  SetHistAttributes(bottom4, 1, 2, 6, 0, 1, 24, 6, 1, 0.12, 1.4, 0.1, 0.12, 0.7, 0.1) ;
  // Draw bottom histograms
  bottom1->Draw(bottomdrawopt1.c_str()) ;
  bottom2->Draw(bottomdrawopt2.c_str()) ;
  bottom3->Draw(bottomdrawopt3.c_str()) ;
  bottom4->Draw(bottomdrawopt4.c_str()) ;
  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  c->Print((canvasname+".png").c_str()) ;
  return 0;
}

int DrawTwelveHistsOnCanvas(string canvasname, TH1F* hist1, TH1F* hist2, TH1F* hist3, TH1F* hist4, TH1F* hist5,TH1F* hist6, TH1F* hist7, TH1F* hist8, TH1F* hist9, TH1F* hist10,TH1F* hist11, TH1F* hist12, string drawopt1="PE",string drawopt2="PESame",string drawopt3="PESame",string drawopt4="PESame", string drawopt5="PESame",string drawopt6="PESame",string drawopt7="PESame",string drawopt8="PESame",string drawopt9="PESame", string drawopt10="PESame",string drawopt11="PESame",string drawopt12="PESame",bool logx=false, bool logy=true, bool isrectangle=true)
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  outpad->SetFillStyle(4000) ;//transparent
  TPad *interpad = new TPad("interpad","interpad",0,0,1,1) ;// For main histo
  SetPadAttributes(interpad, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;

  interpad->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  DrawATLASLabels(0.6, 0.85, 2) ;
  // draw center of mass energy and integrated luminosity
//  DrawCMEAndLumi(0.5, 0.7, "XX", "13 TeV") ;

  interpad->cd() ;
  // Set histogram attributes
  SetHistAttributes(hist1, 1, 2, 1, 0, 1, 20, 1, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist2, 1, 2, 2, 0, 1, 21, 2, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist3, 1, 2, 3, 0, 1, 22, 3, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist4, 1, 2, 4, 0, 1, 23, 4, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist5, 1, 2, 5, 0, 1, 24, 5, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist6, 1, 2, 6, 0, 1, 25, 6, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist7, 1, 2, 7, 0, 1, 26, 7, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist8, 1, 2, 8, 0, 1, 27, 8, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist9, 1, 2, 9, 0, 1, 28, 9, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist10, 1, 2, 15, 0, 1, 29, 15, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist11, 1, 2, 11, 0, 1, 30, 11, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  SetHistAttributes(hist12, 1, 2, 12, 0, 1, 31, 12, 1, 0.04, 1., 0.04, 0.04, 1.2, 0.04, false) ;
  // Draw Histograms
  hist1->Draw(drawopt1.c_str()) ;
  hist2->Draw(drawopt2.c_str()) ;
  hist3->Draw(drawopt3.c_str()) ;
  hist4->Draw(drawopt4.c_str()) ;
  hist5->Draw(drawopt5.c_str()) ;
  hist6->Draw(drawopt6.c_str()) ;
  hist7->Draw(drawopt7.c_str()) ;
  hist8->Draw(drawopt8.c_str()) ;
  hist9->Draw(drawopt9.c_str()) ;
  hist10->Draw(drawopt10.c_str()) ;
  hist11->Draw(drawopt11.c_str()) ;
  hist12->Draw(drawopt12.c_str()) ;
  // define legend
  TLegend *leg=new TLegend(0.6,0.4,0.89,0.8) ;
  leg->SetBorderSize(0) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.03) ;
  leg->AddEntry(hist1, hist1->GetTitle(), "f") ;
  leg->AddEntry(hist2, hist2->GetTitle(), "f") ;
  leg->AddEntry(hist3, hist3->GetTitle(), "f") ;
  leg->AddEntry(hist4, hist4->GetTitle(), "f") ;
  leg->AddEntry(hist5, hist5->GetTitle(), "f") ;
  leg->AddEntry(hist6, hist6->GetTitle(), "f") ;
  leg->AddEntry(hist7, hist7->GetTitle(), "f") ;
  leg->AddEntry(hist8, hist8->GetTitle(), "f") ;
  leg->AddEntry(hist9, hist9->GetTitle(), "f") ;
  leg->AddEntry(hist10, hist10->GetTitle(), "f") ;
  leg->AddEntry(hist11, hist11->GetTitle(), "f") ;
  leg->AddEntry(hist12, hist12->GetTitle(), "f") ;
  leg->Draw("Same") ;
  c->Print((canvasname+".eps").c_str()) ;
  c->Print((canvasname+".pdf").c_str()) ;
  return 0 ;
}

int DrawTopTwoHistsAndBottomOneHistOnCanvas(string canvasname, TH1F *top1, TH1F *top2, TH1F *bottom1, string topdrawopt1 = "HIST",string topdrawopt2 = "PESAME", string bottomdrawopt="PE",bool logx=false, bool toplogy=true, bool bottomlogy=false)
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  TCanvas *c = new TCanvas(canvasname.c_str(), "", 800, 800) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.33) ;// For residuals histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.00001, 0.1, 0, logx, toplogy) ;
  SetPadAttributes(pad2, 0, 0, 0.1, 0.1, 0.3, 0.00001, 0, logx, bottomlogy) ;
  pad1->Draw() ;
  pad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  DrawATLASLabels(0.6, 0.75, 2) ;
  // draw center of mass energy and integrated luminosity
  //DrawCMEAndLumi(0.6, 0.7, "XX", "13 TeV") ;
 
  pad1->cd() ;
  pad1->SetTickx() ;
  pad1->SetTicky() ;

  // set histogram attributes
  SetHistAttributes(top1, 1, 2, 1, 0, 1, 20, 1, 1, 0.05, 1., 0.05, 0.05, 1., 0.05, false) ;
  SetHistAttributes(top2, 1, 2, 2, 0, 1, 21, 2, 1, 0.05, 1., 0.05, 0.05, 1., 0.05, false) ;
  SetHistAttributes(bottom1, 1, 2, 1, 0, 1, 20, 1, 1, 0.1, 1.0, 0.1, 0.1, 0.5, 0.1, false) ;
  if(logx)
  {
    top1->GetXaxis()->SetMoreLogLabels() ;
    top2->GetXaxis()->SetMoreLogLabels() ;
    bottom1->GetXaxis()->SetMoreLogLabels() ;
  }
  top1->Draw(topdrawopt1.c_str()) ;
  top2->Draw(topdrawopt2.c_str()) ;
  // draw legend
  TLegend *leg=new TLegend(0.5,0.7,0.89,0.89) ;
  leg->SetFillStyle(0) ;
  leg->SetTextSize(0.03) ;
  leg->SetBorderSize(0) ;
  if(topdrawopt1.find("HIST")!=string::npos)
    leg->AddEntry(top1, top1->GetTitle(), "L") ;
  else
    leg->AddEntry(top1, top1->GetTitle(), "LPE") ;
  if(topdrawopt2.find("HIST")!=string::npos)
    leg->AddEntry(top2, top2->GetTitle(), "L") ;
  else
    leg->AddEntry(top2, top2->GetTitle(), "LPE") ;
  leg->Draw("same") ;

  pad2->cd() ;
  pad2->SetTicky() ;
  // Draw bottom histograms
  bottom1->Draw(bottomdrawopt.c_str()) ;
  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawTopTwoHistsAndBottomTwoHistsOnCanvas(string canvasname, TH1F *top1, TH1F* top2, 
    TH1F* bottom1, TH1F* bottom2, 
    bool TFirstDrawHistorNot=true, bool TSecondDrawHistorNot=false,
    bool DFirstDrawHistorNot=true, bool BSecondDrawHistorNot=false, 
    bool logx=false, bool toplogy=true, bool bottomlogy=false)
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  TCanvas *c = new TCanvas(canvasname.c_str(), "", 800, 800) ;

  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.33) ;// For residuals histo

  //outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 4000, 0, 0.1, 0.1, 0.00001, 0.1, 0, logx, toplogy) ;
  SetPadAttributes(pad2, 0, 0, 0.1, 0.1, 0.3, 0.00001, 0, logx, bottomlogy) ;
  pad1->Draw() ;
  pad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  DrawATLASLabels(0.5, 0.85, 4) ;
  // draw center of mass energy and integrated luminosity
  DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  pad1->SetTickx() ;
  pad1->SetTicky() ;
  // set top histograms attributes
  SetHistAttributes(top1, 1, 2, 1, 0, 1, 20, 1, 1, 0.04, 1., 0.04, 0.04, 1., 0.04, false) ;
  SetHistAttributes(top2, 1, 2, 2, 0, 1, 21, 2, 1, 0.04, 1., 0.04, 0.04, 1., 0.04, false) ;
  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  // sort the top hists by its maximum
  std::vector< std::pair< TH1F*, bool > > histvec ;
  histvec.push_back(std::make_pair(top1, TFirstDrawHistorNot)) ;
  histvec.push_back(std::make_pair(top2, TSecondDrawHistorNot)) ;
  std::sort(histvec.begin(), histvec.end(),sort_by_vechistmax) ;
  // draw histograms using option "HIST" or not
  SetHistDrawOption(histvec, leg) ;

  pad2->cd() ;
  pad2->SetTicky() ;
  // set bottom histograms attributes
  SetHistAttributes(bottom1, 1, 2, 1, 3004, 1, 20, 1, 1, 0.1, 0.5, 0.1, 0.1, 0.5, 0.1, false) ;
  SetHistAttributes(bottom2, 1, 2, 2, 3005, 1, 21, 2, 1, 0.1, 0.5, 0.1, 0.1, 0.5, 0.1, false) ;
  // Draw bottom histograms
  bottom1->Draw("EP0") ;
  bottom2->Draw("EP0same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;
  return 0;
}

int DrawTwoHistsinHStackOnCanvas(string canvasname, TH1F *top1, TH1F *top2, 
                  bool logx=false, bool logy=false, 
                  bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false, bool isrectangle=true)
{
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.65, 0.7, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.65, 0.65, "XX", "13 TeV") ;
 
  pad1->cd() ;
  pad1->SetTickx() ;
  pad1->SetTicky() ;
  // draw stack
  stack->Draw("HIST") ;
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  gPad->Modified() ;
  // draw legend
  TLegend *leg=new TLegend(0.65,0.75,0.89,0.89) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;
  return 0;
}

int DrawThreeHistsinHStackOnCanvas(string canvasname, TH1F *top1, TH1F *top2, TH1F* top3,
                  bool logx=false, bool logy=false, 
                  bool isrectangle=true, bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.65, 0.7, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.65, 0.65, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->Draw("HIST") ;
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  gPad->Modified() ;

  // draw legend
  TLegend *leg=new TLegend(0.65,0.75,0.89,0.89) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  leg->AddEntry(top3, top2->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawFourHistsinHStackOnCanvas(string canvasname, 
                  TH1F *top1, TH1F *top2, TH1F* top3, TH1F* top4,
                  bool logx=false, bool logy=false, 
                  bool isrectangle=true, bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4) ;
  SetHistAttributes(top4, 1, 2, 5, 1001, 5) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  stack->Add(top4) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.65, 0.65, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.65, 0.6, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->Draw("HIST") ;
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  gPad->Modified() ;

  // draw legend
  TLegend *leg=new TLegend(0.65,0.7,0.89,0.89) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  leg->AddEntry(top3, top3->GetTitle(), "F") ;
  leg->AddEntry(top4, top4->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawTwoHistsinStackAndOneHistOnCanvas(string canvasname, TH1F *top1, TH1F *top2, TH1F *hist,
                  bool logx=false, bool logy=false, 
                  bool isrectangle=true, bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1, 1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  //leg->SetTextSize(0.06) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  leg->AddEntry(hist, hist->GetTitle(), "APL") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawThreeHistsinHStackAndOneHistOnCanvas(string canvasname, TH1F *top1, TH1F *top2, TH1F* top3,                   TH1F *hist,
                  bool logx=false, bool logy=false, 
                  bool isrectangle=true, bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1, 1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, 0, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  leg->AddEntry(top3, top3->GetTitle(), "F") ;
  leg->AddEntry(hist, hist->GetTitle(), "APL") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawFourHistsinHStackAndOneHistOnCanvas(string canvasname, 
                  TH1F *top1, TH1F *top2, TH1F* top3, TH1F *top4, TH1F *hist,
                  bool logx=false, bool logy=false, 
                  bool isrectangle=true, bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  float height=600 ;
  float width ;
  if(isrectangle)
    width=800 ;
  else
    width=600 ;
  TCanvas *c = new TCanvas(canvasname.c_str(), "", width, height) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4) ;
  SetHistAttributes(top4, 1, 2, 5, 1001, 5) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1, 1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  stack->Add(top4) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0,1,1) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.1, 0.1, logx, logy) ;
  pad1->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  leg->AddEntry(top3, top3->GetTitle(), "F") ;
  leg->AddEntry(top4, top4->GetTitle(), "F") ;
  leg->AddEntry(hist, hist->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawTopTwoHistsinHStackAndOneHistBottomOneHistOnCanvas(string canvasname, 
                  TH1F *top1, TH1F *top2, TH1F *hist, TH1F*bottom,
                  bool logx=false, bool toplogy=false, bool bottomlogy=false,
                  bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  TCanvas *c = new TCanvas(canvasname.c_str(), "", 800, 800) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2, 21, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3, 21, 3) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1) ;
  SetHistAttributes(bottom, 1, 2, 1, 1001, 1, 20, 1, 1,  0.1, 0.5, 0.1, 0.1, 0.5, 0.1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.33) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0, 0, 0.1, 0.1, 0.00001, 0.1, 0, logx, toplogy) ;
  SetPadAttributes(pad2, 0, 0, 0.1, 0.1, 0.3, 0.00001, 0, logx, bottomlogy) ;
  pad1->Draw() ;
  pad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;
  // draw bottom pad
  pad2->cd() ;
  bottom->Draw("PE") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawTopThreeHistsinHStackAndOneHistBottomOneHistOnCanvas(string canvasname, 
                  TH1F *top1, TH1F *top2, TH1F* top3, TH1F *hist, TH1F *bottom,
                  bool logx=false, bool toplogy=false, bool bottomlogy=false,
                  bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  TCanvas *c = new TCanvas(canvasname.c_str(), "", 800, 800) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2, 21, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3, 21, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4, 21, 4) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1) ;
  SetHistAttributes(bottom, 1, 2, 1, 1001, 1, 20, 1, 1, 0.1, 0.5, 0.1, 0.1, 0.5, 0.1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.33) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0,0, 0.1, 0.1, 0.00001, 0.1, 0, logx, toplogy) ;
  SetPadAttributes(pad2, 0,0, 0.1, 0.1, 0.3, 0.00001, 0, logx, bottomlogy) ;
  pad1->Draw() ;
  pad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  SetHStackAttributes(stack) ;
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;
  // draw bottom pad
  pad2->cd() ;
  bottom->Draw("PE") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

int DrawTopFourHistsinHStacksAndOneHistBottomOneHistOnCanvas(string canvasname, 
                  TH1F *top1, TH1F *top2, TH1F* top3, TH1F* top4, TH1F *hist,TH1F* bottom,
                  bool logx=false, bool toplogy=false, bool bottomlogy=false,
                  bool drawlabel=true, string label="", 
                  bool drawcmeandlumi=false)
{
  // define Canvas
  TCanvas *c = new TCanvas(canvasname.c_str(), "", 800, 800) ;

  // set attributs of histograms
  SetHistAttributes(top1, 1, 2, 2, 1001, 2, 21, 2) ;
  SetHistAttributes(top2, 1, 2, 3, 1001, 3, 21, 3) ;
  SetHistAttributes(top3, 1, 2, 4, 1001, 4, 21, 4) ;
  SetHistAttributes(top4, 1, 2, 5, 1001, 5, 21, 5) ;
  SetHistAttributes(hist, 1, 2, 1, 1001, 1, 20, 1) ;
  SetHistAttributes(bottom, 1, 2, 1, 1001, 1, 20, 1, 1, 0.1, 0.5, 0.1, 0.1, 0.5, 0.1) ;

  // define stack 
  THStack *stack=new THStack("stack","") ;
  stack->Add(top1) ;
  stack->Add(top2) ;
  stack->Add(top3) ;
  stack->Add(top4) ;
  // Define pads
  TPad *outpad = new TPad("extpad","extpad",0,0,1,1) ;// For marking outermost dimensions
  TPad *pad1 = new TPad("pad1","pad1",0,0.33,1,1) ;// For main histo
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.33) ;// For main histo

  outpad->SetFillStyle(4000) ;//transparent
  SetPadAttributes(pad1, 0,0, 0.1, 0.1, 0.00001, 0.1, 0, logx, toplogy) ;
  SetPadAttributes(pad2, 0,0, 0.1, 0.1, 0.3, 0.00001, 0, logx, bottomlogy) ;
  pad1->Draw() ;
  pad2->Draw() ;
  outpad->Draw() ;

  outpad->cd() ;
  // Draw ATLAS Label
  if(drawlabel)
    DrawLabels(0.5, 0.85, label.c_str(), true) ;
  // draw center of mass energy and integrated luminosity
  if(drawcmeandlumi)
    DrawCMEAndLumi(0.5, 0.8, "XX", "13 TeV") ;
 
  pad1->cd() ;
  // draw stack
  stack->GetXaxis()->SetTitle(top1->GetXaxis()->GetTitle()) ;
  stack->GetYaxis()->SetTitle(top1->GetYaxis()->GetTitle()) ;
  stack->Draw("HIST") ;
  hist->Draw("PESAME") ;

  // draw legend
  TLegend *leg=new TLegend(0.5,0.55,0.89,0.65) ;
  leg->SetBorderSize(0) ;
  leg->AddEntry(top1, top1->GetTitle(), "F") ;
  leg->AddEntry(top2, top2->GetTitle(), "F") ;
  // draw legend
  leg->Draw("same") ;
  // draw bottom pad
  pad2->cd() ;
  bottom->Draw("PE") ;

  c->Update() ; 
  c->Print((canvasname+".eps").c_str()) ;

  return 0;
}

//-----------------------------------------------------
int DrawLabels(float xstart, float ystart, string label="", bool isbold=true)
{
  TLatex m_latex;
  m_latex.SetTextSize(0.04) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(42) ;
  if(isbold) m_latex.SetTextFont(22) ;
  m_latex.DrawLatex(xstart, ystart, label.c_str()) ;
  return 0 ;
}
#ifndef DrawATLASLabels_Func
#define DrawATLASLabels_Func

int DrawATLASLabels(float xstart, float ystart, int labeltype)
{
  // (xstart, ystart) the starting coordinate of the latex
  // define tlatex
  TLatex m_latex;
  //m_latex.SetFillStyle(0) ;
  m_latex.SetTextSize(0.04) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(72) ;
  // Draw ATLAS lebel
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
int DrawCMEAndLumi(float xstart, float ystart, string lumiInFb, string CME)
{
  // (xstart, ystart): coornidate of the leftbottom of the pavetext
  TPaveText *pavetext =new TPaveText() ;
  pavetext->SetOption("brNDC") ;
  pavetext->SetFillColor(0) ;
  pavetext->SetFillStyle(0) ;
  pavetext->SetBorderSize(0) ;
  pavetext->SetTextAlign(11) ;
  pavetext->SetTextFont(42) ;
  pavetext->SetTextSize(0.03) ;
  pavetext->SetTextColor(kBlack) ;
  pavetext->SetX1NDC(xstart-0.01) ;
  pavetext->SetY1NDC(ystart-0.01) ;
  pavetext->SetX2NDC(xstart+0.25) ;
  pavetext->SetY2NDC(ystart+0.05) ;

  pavetext->AddText(0.04, 1./8., ("#sqrt{s}="+CME+", #intLdt="+lumiInFb+"fb^{-1}").c_str()) ;
  pavetext->Draw() ;

  return  0;
}

// get the ratio histogram of two histrograms
TH1F *GetRatioHistOfTwoHists(string name, string title, string xaxistitle, string yaxistitle, TH1F* numeratorhist, TH1F* denominatorhist)
{
  //TH1F *ratiohist = new TH1F(name.c_str(), title.c_str(), numeratorhist->GetNbinsX(), numeratorhist->GetXaxis()->GetXbins()->GetArray()) ;
  TH1F *ratiohist = (TH1F*) numeratorhist->Clone() ;
  ratiohist->Sumw2() ;
  ratiohist->SetName(name.c_str()) ;
  ratiohist->SetTitle(title.c_str()) ;
  ratiohist->GetXaxis()->SetTitle(xaxistitle.c_str()) ;
  ratiohist->GetYaxis()->SetTitle(yaxistitle.c_str()) ;
  //ratiohist->Divide(numeratorhist, denominatorhist, 1., 1., "B") ;
  ratiohist->Divide(denominatorhist) ;
  return ratiohist ;
}

// get the ratio histogram of errorbar of two histrograms
TH1F *GetErrorBarRatioHistOfTwoHists(string name, string title, string xaxistitle, string yaxistitle, TH1F* numeratorhist, TH1F* denominatorhist)
{
  //TH1F *ratiohist = (TH1F*)denominatorhist->Clone() ;
  TH1F *ratiohist = new TH1F(name.c_str(), title.c_str(), numeratorhist->GetNbinsX(), numeratorhist->GetXaxis()->GetXbins()->GetArray()) ;
  ratiohist->GetYaxis()->SetTitle(yaxistitle.c_str()) ;
  ratiohist->GetXaxis()->SetTitle(xaxistitle.c_str()) ;
  //ratiohist->Divide(denominatorhist) ;
  for(int n=1 ; n<=ratiohist->GetNbinsX() ; n++)
  {
    if(denominatorhist->GetBinError(n)==0) 
      ratiohist->SetBinContent(n, 0.) ;
    else
    {
      float over = numeratorhist->GetBinError(n)/denominatorhist->GetBinError(n) ;
      ratiohist->SetBinContent(n, over) ;
    }
  }
  return ratiohist ;
}

bool sort_by_histmax(TH1F* hist1, TH1F* hist2)
{
  return hist1->GetMaximum()>hist2->GetMaximum() ;
}
bool sort_by_vechistmax(std::pair<TH1F*, bool> p1, std::pair<TH1F*, bool> p2)
{
  return p1.first->GetMaximum()>p2.first->GetMaximum() ; ;
}
