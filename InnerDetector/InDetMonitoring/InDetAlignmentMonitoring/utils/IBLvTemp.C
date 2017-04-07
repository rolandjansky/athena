/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 
// ROOT macro to draw the IBL vs the temperature
//
#include <iostream>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"


TCanvas *CanSagittavTemp, *CanIBLNHitsvTemp, *CanIBLFHitsvTemp;
struct RangeOfHist {
  float UpperValue;
  float LowerValue;
};

RangeOfHist getRange(std::vector<float>, float gapfactor = 0.05);
//
//-------------------------------------------------
//
void IBLvTemp()
{
  // load values
  std::vector<float> IBLTemp, IBLSagitta, IBLSagittaError, NIDCosmic, NIBLHits, FractionIBLHits;

  // +20 C modules (+15C set point)
  IBLTemp.push_back(15);
  IBLSagitta.push_back(-2.4); IBLSagittaError.push_back(11.1);
  NIDCosmic.push_back(308881);
  NIBLHits.push_back(4801);

  //+12 C modules (+7C set point)
  IBLTemp.push_back(7);
  IBLSagitta.push_back(45.0); IBLSagittaError.push_back(10.3);
  NIDCosmic.push_back(255922);
  NIBLHits.push_back(5506);

  //  0 C
  IBLTemp.push_back(0);
  IBLSagitta.push_back(101.9); IBLSagittaError.push_back(10.8);
  NIDCosmic.push_back(250644);
  NIBLHits.push_back(5427);

  // -12C
  IBLTemp.push_back(-10);
  IBLSagitta.push_back(148.0); IBLSagittaError.push_back(8.9);
  NIDCosmic.push_back(366804);
  NIBLHits.push_back(7889);

  // -15C
  IBLTemp.push_back(-15);
  IBLSagitta.push_back(208.5); IBLSagittaError.push_back(7.7);
  NIDCosmic.push_back(457588);
  NIBLHits.push_back(9187);
  
  // -20C
  IBLTemp.push_back(-20);
  IBLSagitta.push_back(238.9); IBLSagittaError.push_back(5.6);
  NIDCosmic.push_back(977508);
  NIBLHits.push_back(19540);
  
  // work out IBL hits per trigger
  for (int i=0; i< (int) IBLTemp.size(); i++) {
    FractionIBLHits.push_back(100 * NIBLHits.at(i)/NIDCosmic.at(i));
  }

  // find the ranges:
  RangeOfHist myRange = getRange(IBLTemp);
  float UpperTemp = myRange.UpperValue;
  float LowerTemp = myRange.LowerValue;

  myRange = getRange(IBLSagitta);
  float UpperSagitta = myRange.UpperValue;
  float LowerSagitta = myRange.LowerValue;

  myRange = getRange(NIBLHits);
  float UpperNHits = myRange.UpperValue;
  float LowerNHits = myRange.LowerValue;

  myRange = getRange(FractionIBLHits, 1.10);
  float UpperFHits = myRange.UpperValue;
  float LowerFHits = 0;

  ///////////////////////////////////////
  // Create the frames
  ///////////////////////////////////////
  TH1F* FrameSagitta = new TH1F("hSagitta","M9 data: IBL stave sagitta vs temperature", 100, LowerTemp, UpperTemp);  
  FrameSagitta->SetXTitle("Average module temperature [C]");
  FrameSagitta->SetYTitle("Sagitta [#mum]");
  FrameSagitta->SetMaximum(UpperSagitta);
  FrameSagitta->SetMinimum(LowerSagitta);
  FrameSagitta->SetStats(kFALSE);
  // fake histogram to be like empty
  for (int bin=1; bin <= FrameSagitta->GetNbinsX(); bin++){
    FrameSagitta->SetBinContent(bin, LowerSagitta -1);
  } 

  TH1F* FrameNhits = new TH1F("hNhits","Collected IBL hits per temperature point", 100, LowerTemp, UpperTemp);  
  FrameNhits->SetXTitle("Set point temperature [C]");
  FrameNhits->SetYTitle("Number of hits");
  FrameNhits->SetMaximum(UpperNHits);
  FrameNhits->SetMinimum(LowerNHits);
  FrameNhits->SetStats(kFALSE);
  // fake histogram to be like empty
  for (int bin=1; bin <= FrameNhits->GetNbinsX(); bin++){
    FrameNhits->SetBinContent(bin, LowerNHits -1);
  } 

  TH1F* FrameFHits = new TH1F("hFHits","Fraction of IBL hits per IDCosmics and temperature point", 100, LowerTemp, UpperTemp);  
  FrameFHits->SetXTitle("Average module temperature [C]");
  FrameFHits->SetYTitle("Fraction of IBL hits [%]");
  FrameFHits->SetMaximum(UpperFHits);
  FrameFHits->SetMinimum(LowerFHits);
  FrameFHits->SetStats(kFALSE);
  // fake histogram to be like empty
  for (int bin=1; bin <= FrameNhits->GetNbinsX(); bin++){
    FrameFHits->SetBinContent(bin, LowerNHits -1);
  } 

  ///////////////////////////////////////
  // create the TgraphErrors
  ///////////////////////////////////////
  TGraphErrors *Graph_IBLSagitta = new TGraphErrors();
  Graph_IBLSagitta->SetMarkerStyle(20);
  Graph_IBLSagitta->SetMarkerSize(1.5);
  Graph_IBLSagitta->SetMarkerColor(kBlue);
  Graph_IBLSagitta->SetLineColor(Graph_IBLSagitta->GetMarkerColor());
  Graph_IBLSagitta->SetLineWidth(2);
  Graph_IBLSagitta->SetTitle(FrameSagitta->GetTitle());
  for (int i=0; i < (int) IBLTemp.size(); i++) {
    Graph_IBLSagitta->SetPoint(i, IBLTemp.at(i), IBLSagitta.at(i));
    Graph_IBLSagitta->SetPointError(i, 0.1, IBLSagittaError.at(i));
  }

  TGraphErrors* Graph_IBLNhits = new TGraphErrors();
  Graph_IBLNhits->SetMarkerStyle(20);
  Graph_IBLNhits->SetMarkerSize(1.5);
  Graph_IBLNhits->SetMarkerColor(kBlue);
  Graph_IBLNhits->SetLineColor(Graph_IBLNhits->GetMarkerColor());
  Graph_IBLNhits->SetLineWidth(2);
  Graph_IBLNhits->SetTitle(FrameSagitta->GetTitle());
  for (int i=0; i < (int) IBLTemp.size(); i++) {
    Graph_IBLNhits->SetPoint(i, IBLTemp.at(i), NIBLHits.at(i));
  }

  TGraphErrors* Graph_IBLFHits = new TGraphErrors();
  Graph_IBLFHits->SetMarkerStyle(20);
  Graph_IBLFHits->SetMarkerSize(1.5);
  Graph_IBLFHits->SetMarkerColor(kGreen+2);
  Graph_IBLFHits->SetLineColor(Graph_IBLFHits->GetMarkerColor());
  Graph_IBLFHits->SetLineWidth(2);
  Graph_IBLFHits->SetTitle(FrameSagitta->GetTitle());
  for (int i=0; i < (int) IBLTemp.size(); i++) {
    Graph_IBLFHits->SetPoint(i, IBLTemp.at(i), FractionIBLHits.at(i));
  }

  ///////////////////////////////////////
  // create canvas and draw
  ///////////////////////////////////////
  CanSagittavTemp = new TCanvas("cansagitavtemp","IBL sagitta vs temperature",900,600);
  FrameSagitta->Draw();
  Graph_IBLSagitta->Draw("LP");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  Graph_IBLSagitta->Fit("pol1");
  CanSagittavTemp->Print("IBLSagittaVsTemp.png");
  Graph_IBLSagitta->Print();

  // create canvas and draw
  CanIBLNHitsvTemp = new TCanvas("canIBLNhitsvtemp","Collected IBL hits per temperature point",900,600);
  FrameNhits->Draw();
  Graph_IBLNhits->Draw("LP");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  CanIBLNHitsvTemp->Print("IBLNHitsVsTemp.png");

  // fraction of hits
  CanIBLFHitsvTemp = new TCanvas("canIBLFHitsvtemp","Fraction of IBL hits per IDCosmic and temperature point",900,600);
  FrameFHits->Draw();
  Graph_IBLFHits->Draw("LP");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  CanIBLFHitsvTemp->Print("IBLFHitsVsTemp.png");

  return;
}
//
//-------------------------------------------------
//
RangeOfHist getRange(std::vector<float> ListOfValues, float gapfactor)
{
  RangeOfHist myRange;
  
  myRange.LowerValue = ListOfValues.at(0);
  myRange.UpperValue = ListOfValues.at(0);

  for (int i=1; i< (int) ListOfValues.size(); i++) {
    if ( ListOfValues.at(i) < myRange.LowerValue) myRange.LowerValue =  ListOfValues.at(i); 
    if ( ListOfValues.at(i) > myRange.UpperValue) myRange.UpperValue =  ListOfValues.at(i); 
  }

  float delta = gapfactor * (myRange.UpperValue -  myRange.LowerValue);

  myRange.LowerValue -= delta;
  myRange.UpperValue += delta;

  std::cout <<  myRange.LowerValue << " -> " << myRange.UpperValue << std::endl;
  
  return myRange;
}

