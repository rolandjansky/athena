/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


  
#include "include/AtlasUtils.h"
#include "include/AtlasStyle.C"
#include "include/AtlasLabels.C"
#include "include/Labels.h"
#include <TCanvas.h>
#include <TFile.h>
#include "TLegend.h"
#include <TString.h>
#include <TH2D.h>
#include <map>
#include <vector>

void BowingMagEvolForStaveAll2016()
{
//=========Macro generated from canvas: CanBowingMagEvolForStaveAll/IBL stave All 
//=========  (Sat Oct 10 02:48:25 2015) by ROOT version6.02/12
   TCanvas *CanBowingMagEvolForStaveAll = new TCanvas("CanBowingMagEvolForStaveAll", "IBL stave All ",0,0,900,600);
   CanBowingMagEvolForStaveAll->Range(-4.5,-31.25,35.5,31.25);
   CanBowingMagEvolForStaveAll->SetFillColor(0);
   CanBowingMagEvolForStaveAll->SetBorderMode(0);
   CanBowingMagEvolForStaveAll->SetBorderSize(2);
   CanBowingMagEvolForStaveAll->SetTickx(1);
   CanBowingMagEvolForStaveAll->SetTicky(1);
   CanBowingMagEvolForStaveAll->SetFrameBorderMode(0);
   CanBowingMagEvolForStaveAll->SetFrameBorderMode(0);
   CanBowingMagEvolForStaveAll->SetBottomMargin(0.22);
   CanBowingMagEvolForStaveAll->SetRightMargin(0.05);
   CanBowingMagEvolForStaveAll->SetTopMargin(0.05);
  
  
   //TH1F *h_bowMagEvolIBL1 = new TH1F("h_bowMagEvolIBL1","",47,-0.5,46.5);
   int nruns_2015 = 47;
   int nruns_2016 = 58;
   int nruns = nruns_2015 + nruns_2016 + 1;

   // vector with the dates:
   //std::vector<TDatime> me_runDateTime;
   

   // set 2015 after corrections
  TH1F *h_bowMagEvolIBL1 = new TH1F("h_bowMagEvolIBL1","", nruns,-0.5, nruns-0.5);
  h_bowMagEvolIBL1->SetBinContent(1,1.896648e10);
  h_bowMagEvolIBL1->SetBinContent(2,2.135196e10);
  h_bowMagEvolIBL1->SetBinContent(3,1.101701e10);
  h_bowMagEvolIBL1->SetBinContent(4,1.313775e10);
  h_bowMagEvolIBL1->SetBinContent(5,1.28636e10);
  h_bowMagEvolIBL1->SetBinContent(6,1.430065e10);
  h_bowMagEvolIBL1->SetBinContent(7,1.382757e10);
  h_bowMagEvolIBL1->SetBinContent(8,-0.6470556e10);
  h_bowMagEvolIBL1->SetBinContent(9,2.134798e10);
  h_bowMagEvolIBL1->SetBinContent(10,0.8949475e10);
  h_bowMagEvolIBL1->SetBinContent(11,-0.4941857e10);
  h_bowMagEvolIBL1->SetBinContent(12,-1.017381e10);
  
  h_bowMagEvolIBL1->SetBinContent(13,1.803407);
  h_bowMagEvolIBL1->SetBinContent(14,1.306839);
  h_bowMagEvolIBL1->SetBinContent(15,0.07100698);
  h_bowMagEvolIBL1->SetBinContent(16,-0.2115438);
  h_bowMagEvolIBL1->SetBinContent(17,0.836698);
  h_bowMagEvolIBL1->SetBinContent(18,-0.182584);
  h_bowMagEvolIBL1->SetBinContent(19,0.3558431);
  h_bowMagEvolIBL1->SetBinContent(20,1.392249);
  h_bowMagEvolIBL1->SetBinContent(21,0.2004839);
  h_bowMagEvolIBL1->SetBinContent(22,2.141745);
  h_bowMagEvolIBL1->SetBinContent(23,2.141745);
  h_bowMagEvolIBL1->SetBinContent(24,2.141745);
  h_bowMagEvolIBL1->SetBinContent(25,0.6531002);
  h_bowMagEvolIBL1->SetBinContent(26,1.13957);
  h_bowMagEvolIBL1->SetBinContent(27,1.261269);
  h_bowMagEvolIBL1->SetBinContent(28,1.603248);
  h_bowMagEvolIBL1->SetBinContent(29,1.718212);
  h_bowMagEvolIBL1->SetBinContent(30,2.02957);
  h_bowMagEvolIBL1->SetBinContent(31,2.820183);
  h_bowMagEvolIBL1->SetBinContent(32,0.2423804);
  h_bowMagEvolIBL1->SetBinContent(33,2.116418);
  h_bowMagEvolIBL1->SetBinContent(34,2.073665);
  h_bowMagEvolIBL1->SetBinContent(35,1.970335);
  h_bowMagEvolIBL1->SetBinContent(36,2.560627);
  h_bowMagEvolIBL1->SetBinContent(37,2.358727);
  h_bowMagEvolIBL1->SetBinContent(38,2.634164);
  h_bowMagEvolIBL1->SetBinContent(39,2.276105);
  h_bowMagEvolIBL1->SetBinContent(40,2.131346);
  h_bowMagEvolIBL1->SetBinContent(41,2.073411);
  h_bowMagEvolIBL1->SetBinContent(42,2.441031);
  h_bowMagEvolIBL1->SetBinContent(43,3.6162);
  h_bowMagEvolIBL1->SetBinContent(44,2.446549);
  h_bowMagEvolIBL1->SetBinContent(45,2.629768);
  h_bowMagEvolIBL1->SetBinContent(46,1.866875);
  h_bowMagEvolIBL1->SetBinContent(47,2.685501);





  h_bowMagEvolIBL1->SetBinError(1,0.5413845);
  h_bowMagEvolIBL1->SetBinError(2,0.5347788);
  h_bowMagEvolIBL1->SetBinError(3,0.8878446);
  h_bowMagEvolIBL1->SetBinError(4,0.587181);
  h_bowMagEvolIBL1->SetBinError(5,0.6501646);
  h_bowMagEvolIBL1->SetBinError(6,0.6052314);
  h_bowMagEvolIBL1->SetBinError(7,0.5286695);
  h_bowMagEvolIBL1->SetBinError(8,0.4673619);
  h_bowMagEvolIBL1->SetBinError(9,0.5130117);
  h_bowMagEvolIBL1->SetBinError(10,0.8947007);
  h_bowMagEvolIBL1->SetBinError(11,0.5348972);
  h_bowMagEvolIBL1->SetBinError(12,0.5789596);
  h_bowMagEvolIBL1->SetBinError(13,0.8094525);
  h_bowMagEvolIBL1->SetBinError(14,0.512046);
  h_bowMagEvolIBL1->SetBinError(15,0.5325303);
  h_bowMagEvolIBL1->SetBinError(16,0.4179377);
  h_bowMagEvolIBL1->SetBinError(17,0.5184069);
  h_bowMagEvolIBL1->SetBinError(18,0.6530609);
  h_bowMagEvolIBL1->SetBinError(19,0.4197632);
  h_bowMagEvolIBL1->SetBinError(20,0.4220131);
  h_bowMagEvolIBL1->SetBinError(21,0.5363397);
  h_bowMagEvolIBL1->SetBinError(22,0.6552895);
  h_bowMagEvolIBL1->SetBinError(23,0.533067);
  h_bowMagEvolIBL1->SetBinError(24,0.8041184);
  h_bowMagEvolIBL1->SetBinError(25,0.4130648);
  h_bowMagEvolIBL1->SetBinError(26,0.3907885);
  h_bowMagEvolIBL1->SetBinError(27,0.3689131);
  h_bowMagEvolIBL1->SetBinError(28,0.3656114);
  h_bowMagEvolIBL1->SetBinError(29,0.5307977);
  h_bowMagEvolIBL1->SetBinError(30,0.5064833);
  h_bowMagEvolIBL1->SetBinError(31,0.4247717);
  h_bowMagEvolIBL1->SetBinError(32,0.6705536);
  h_bowMagEvolIBL1->SetBinError(33,0.416724);
  h_bowMagEvolIBL1->SetBinError(34,0.450326);
  h_bowMagEvolIBL1->SetBinError(35,0.500049);
  h_bowMagEvolIBL1->SetBinError(36,0.587815);
  h_bowMagEvolIBL1->SetBinError(37,0.3923438);
  h_bowMagEvolIBL1->SetBinError(38,0.5066393);
  h_bowMagEvolIBL1->SetBinError(39,0.4092376);
  h_bowMagEvolIBL1->SetBinError(40,0.5000976);
  h_bowMagEvolIBL1->SetBinError(41,0.525107);
  h_bowMagEvolIBL1->SetBinError(42,0.4034376);
  h_bowMagEvolIBL1->SetBinError(43,0.3984655);
  h_bowMagEvolIBL1->SetBinError(44,0.4786002);
  h_bowMagEvolIBL1->SetBinError(45,0.5275378);
  h_bowMagEvolIBL1->SetBinError(46,0.4945553);
  h_bowMagEvolIBL1->SetBinError(47,0.4359028);
  
  //
  h_bowMagEvolIBL1->SetMinimum(-30);
  h_bowMagEvolIBL1->SetMaximum(30);
  h_bowMagEvolIBL1->SetEntries(47);
  h_bowMagEvolIBL1->SetStats(0);
  
  //   Int_t ci;      // for color index setting
  //   TColor *color; // for color definition with alpha
  //   ci = TColor::GetColor("#0033cc");
  h_bowMagEvolIBL1->SetLineColor(kRed);
  h_bowMagEvolIBL1->SetLineWidth(2);
  
  //   ci = TColor::GetColor("#0033cc");
  h_bowMagEvolIBL1->SetMarkerColor(kRed);
  h_bowMagEvolIBL1->SetMarkerStyle(20);

//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(1,"279598");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(2,"279685");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(3,"279764");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(4,"279813");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(5,"279867");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(6,"279932");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(7,"279984");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(8,"280231");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(9,"280319");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(10,"280368");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(11,"280423");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(12,"280464");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(13,"280500");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(14,"280520");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(15,"280614");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(16,"280673");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(17,"280753");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(18,"280853");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(19,"280862");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(20,"280950");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(21,"280977");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(22,"281070");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(23,"281074");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(24,"281075");
//  //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(25,"281130");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(25,"281317");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(26,"281381");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(27,"281385");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(28,"281411");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(29,"282625");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(30,"282631");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(31,"282712");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(32,"282784");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(33,"282992");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(34,"283074");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(35,"283155");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(36,"283270");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(37,"283429");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(38,"283608");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(39,"283780");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(40,"284006");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(41,"284154");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(42,"284213");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(43,"284285");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(44,"284420");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(45,"284427");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(46,"284473");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(47,"284484");
  h_bowMagEvolIBL1->Draw("");
  TLine *line = new TLine(h_bowMagEvolIBL1->GetXaxis()->GetXmin(), 0. , h_bowMagEvolIBL1->GetXaxis()->GetXmax(), 0.);
  
 

//   Int_t ci;      // for color index setting
//   TColor *color; // for color definition with alpha
//   ci = TColor::GetColor("#0033cc");
   h_bowMagEvolIBL1->SetLineColor(kRed);
   h_bowMagEvolIBL1->SetLineWidth(2);

//   ci = TColor::GetColor("#0033cc");
   h_bowMagEvolIBL1->SetMarkerColor(kRed);
   h_bowMagEvolIBL1->SetMarkerStyle(20);
  /**
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(1,"278748");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(2,"278880");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(3,"278912");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(4,"278968");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(5,"279169");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(6,"279259");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(7,"279279");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(8,"279284");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(9,"279345");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(10,"279598");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(11,"279685");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(12,"279764");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(13,"279813");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(14,"279867");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(15,"279928");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(16,"279932");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(17,"279984");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(18,"280231");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(19,"280273");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(20,"280319");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(21,"280368");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(22,"280464");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(23,"280500");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(24,"280520");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(25,"280614");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(26,"280673");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(27,"280753");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(28,"280853");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(29,"280862");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(30,"280950");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(31,"280977");
   h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(32,"281070");
  **/
  
  
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(1,"07/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(2,"08/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(3,"09/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(4,"09/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(5,"11/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(6,"12/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(7,"13/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(8,"14/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(9,"14/09");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(1,"16/09/15");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(11,"17/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(12,"18/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(13,"19/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(14,"19/09");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(6,"20/09/15");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(16,"20/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(17,"21/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(18,"24/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(19,"24/09");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(11,"26/09/15");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(21,"26/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(22,"27/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(23,"28/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(24,"28/09");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(16,"29/09/15");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(26,"29/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(27,"30/09");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(28,"02/10");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(29,"02/10");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(21,"03/10/15");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(26,"09/10/15");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(31,"20/10/15");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(36,"24/10/15");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(41,"29/10/15");
  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(46,"02/11/15");
//  
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(1,"07-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(2,"08-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(3,"09-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(4,"09-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(5,"11-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(6,"12-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(7,"13-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(8,"14-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(9,"14-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(10,"16-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(11,"17-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(12,"18-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(13,"19-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(14,"19-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(15,"20-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(16,"20-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(17,"21-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(18,"24-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(19,"24-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(20,"25-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(21,"26-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(22,"27-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(23,"28-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(24,"28-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(25,"29-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(26,"29-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(27,"30-09-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(28,"02-10-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(29,"02-10-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(30,"03-10-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(31,"04-10-2015");
//  h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(32,"05-10-2015");
  
  
   h_bowMagEvolIBL1->GetXaxis()->SetLabelFont(42);
   h_bowMagEvolIBL1->GetXaxis()->SetLabelSize(0.06);
   h_bowMagEvolIBL1->GetXaxis()->SetTitleSize(0.045);
   h_bowMagEvolIBL1->GetXaxis()->SetTitleFont(42);
   h_bowMagEvolIBL1->GetYaxis()->SetTitle("Average IBL distortion magnitude [#mum]");
   h_bowMagEvolIBL1->GetXaxis()->SetTitle("Starting date of LHC Fill");
   h_bowMagEvolIBL1->GetYaxis()->SetLabelFont(42);
   h_bowMagEvolIBL1->GetYaxis()->SetLabelSize(0.04);
   h_bowMagEvolIBL1->GetYaxis()->SetTitleSize(0.045);
   h_bowMagEvolIBL1->GetYaxis()->SetTitleFont(42);
   h_bowMagEvolIBL1->GetZaxis()->SetLabelFont(42);
   h_bowMagEvolIBL1->GetZaxis()->SetLabelSize(0.04);
   h_bowMagEvolIBL1->GetZaxis()->SetTitleSize(0.04);
   h_bowMagEvolIBL1->GetZaxis()->SetTitleFont(42);
   h_bowMagEvolIBL1->GetXaxis()->SetTitleOffset(2.6);
   h_bowMagEvolIBL1->GetXaxis()->LabelsOption("v");
   h_bowMagEvolIBL1->GetXaxis()->SetNdivisions(210,false);

   h_bowMagEvolIBL1->Draw("");
  // TLine *line = new TLine(-0.5,0,31.5,0);

  
  TH1F *h_bowMagEvolIBL2 = new TH1F("h_bowMagEvolIBL2","IBL all staves bowing magnitude evolution per run",
				    h_bowMagEvolIBL1->GetNbinsX(), h_bowMagEvolIBL1->GetXaxis()->GetXmin(), h_bowMagEvolIBL1->GetXaxis()->GetXmax()); 
  h_bowMagEvolIBL2->SetBinContent(1,1.896648);
  h_bowMagEvolIBL2->SetBinContent(2,2.135196);
  h_bowMagEvolIBL2->SetBinContent(3,1.101701);
  h_bowMagEvolIBL2->SetBinContent(4,1.313775);
  h_bowMagEvolIBL2->SetBinContent(5,1.28636);
  h_bowMagEvolIBL2->SetBinContent(6,1.430065);
  h_bowMagEvolIBL2->SetBinContent(7,1.382757);
  h_bowMagEvolIBL2->SetBinContent(8,-0.6470556);
  h_bowMagEvolIBL2->SetBinContent(9,2.134798);
  h_bowMagEvolIBL2->SetBinContent(10,0.5633476);
  h_bowMagEvolIBL2->SetBinContent(11,-1.317766);
  h_bowMagEvolIBL2->SetBinContent(12,-1.724783);
  h_bowMagEvolIBL2->SetBinContent(13,-3.008788);
  h_bowMagEvolIBL2->SetBinContent(14,-3.153189);
  h_bowMagEvolIBL2->SetBinContent(15,-4.060569);
  h_bowMagEvolIBL2->SetBinContent(16,-4.245149);
  h_bowMagEvolIBL2->SetBinContent(17,-7.168593);
  h_bowMagEvolIBL2->SetBinContent(18,-5.22915);
  h_bowMagEvolIBL2->SetBinContent(19,-7.700996);
  h_bowMagEvolIBL2->SetBinContent(20,-11.08023);
  h_bowMagEvolIBL2->SetBinContent(21,-18.76935);
  h_bowMagEvolIBL2->SetBinContent(22,-12.99612);
  h_bowMagEvolIBL2->SetBinContent(23,-13.48292);
  h_bowMagEvolIBL2->SetBinContent(24,-14.18536);
  h_bowMagEvolIBL2->SetBinContent(25,-7.057932);
  h_bowMagEvolIBL2->SetBinContent(26,-5.950931);
  h_bowMagEvolIBL2->SetBinContent(27,-6.858956);
  h_bowMagEvolIBL2->SetBinContent(28,-7.652114);
  h_bowMagEvolIBL2->SetBinContent(29,-4.604797);
  h_bowMagEvolIBL2->SetBinContent(30,-5.33258);
  h_bowMagEvolIBL2->SetBinContent(31,-6.198364);
  h_bowMagEvolIBL2->SetBinContent(32,-7.662108);
  h_bowMagEvolIBL2->SetBinContent(33,-6.886216);
  h_bowMagEvolIBL2->SetBinContent(34,-7.485165);
  h_bowMagEvolIBL2->SetBinContent(35,-8.084642);
  h_bowMagEvolIBL2->SetBinContent(36,-7.21821);
  h_bowMagEvolIBL2->SetBinContent(37,-10.05523);
  h_bowMagEvolIBL2->SetBinContent(38,-14.47217);
  h_bowMagEvolIBL2->SetBinContent(39,-15.19572);
  h_bowMagEvolIBL2->SetBinContent(40,-15.00539);
  h_bowMagEvolIBL2->SetBinContent(41,-11.90118);
  h_bowMagEvolIBL2->SetBinContent(42,-15.73882);
  h_bowMagEvolIBL2->SetBinContent(43,-19.79849);
  h_bowMagEvolIBL2->SetBinContent(44,-23.14169);
  h_bowMagEvolIBL2->SetBinContent(45,-22.45906);
  h_bowMagEvolIBL2->SetBinContent(46,-21.847);
  h_bowMagEvolIBL2->SetBinContent(47,-26.28523);
  h_bowMagEvolIBL2->SetBinError(1,0.5413845);
  h_bowMagEvolIBL2->SetBinError(2,0.5347788);
  h_bowMagEvolIBL2->SetBinError(3,0.8878446);
  h_bowMagEvolIBL2->SetBinError(4,0.587181);
  h_bowMagEvolIBL2->SetBinError(5,0.6501646);
  h_bowMagEvolIBL2->SetBinError(6,0.6052314);
  h_bowMagEvolIBL2->SetBinError(7,0.5286695);
  h_bowMagEvolIBL2->SetBinError(8,0.4673619);
  h_bowMagEvolIBL2->SetBinError(9,0.5130117);
  h_bowMagEvolIBL2->SetBinError(10,0.8972625);
  h_bowMagEvolIBL2->SetBinError(11,0.5339997);
  h_bowMagEvolIBL2->SetBinError(12,0.5787392);
  h_bowMagEvolIBL2->SetBinError(13,0.807655);
  h_bowMagEvolIBL2->SetBinError(14,0.6379336);
  h_bowMagEvolIBL2->SetBinError(15,0.6833411);
  h_bowMagEvolIBL2->SetBinError(16,0.4103184);
  h_bowMagEvolIBL2->SetBinError(17,0.5238094);
  h_bowMagEvolIBL2->SetBinError(18,0.6558808);
  h_bowMagEvolIBL2->SetBinError(19,0.4219276);
  h_bowMagEvolIBL2->SetBinError(20,0.4259876);
  h_bowMagEvolIBL2->SetBinError(21,0.5522355);
  h_bowMagEvolIBL2->SetBinError(22,0.6670758);
  h_bowMagEvolIBL2->SetBinError(23,0.5409073);
  h_bowMagEvolIBL2->SetBinError(24,0.8137216);
  h_bowMagEvolIBL2->SetBinError(25,0.418953);
  h_bowMagEvolIBL2->SetBinError(26,0.3954922);
  h_bowMagEvolIBL2->SetBinError(27,0.3720939);
  h_bowMagEvolIBL2->SetBinError(28,0.3689138);
  h_bowMagEvolIBL2->SetBinError(29,0.5305057);
  h_bowMagEvolIBL2->SetBinError(30,0.508383);
  h_bowMagEvolIBL2->SetBinError(31,0.4256861);
  h_bowMagEvolIBL2->SetBinError(32,0.67439);
  h_bowMagEvolIBL2->SetBinError(33,0.4187743);
  h_bowMagEvolIBL2->SetBinError(34,0.452897);
  h_bowMagEvolIBL2->SetBinError(35,0.5033457);
  h_bowMagEvolIBL2->SetBinError(36,0.5918881);
  h_bowMagEvolIBL2->SetBinError(37,0.3907636);
  h_bowMagEvolIBL2->SetBinError(38,0.5134978);
  h_bowMagEvolIBL2->SetBinError(39,0.4137497);
  h_bowMagEvolIBL2->SetBinError(40,0.5084443);
  h_bowMagEvolIBL2->SetBinError(41,0.5290402);
  h_bowMagEvolIBL2->SetBinError(42,0.4078279);
  h_bowMagEvolIBL2->SetBinError(43,0.4048347);
  h_bowMagEvolIBL2->SetBinError(44,0.4930222);
  h_bowMagEvolIBL2->SetBinError(45,0.5447291);
  h_bowMagEvolIBL2->SetBinError(46,0.5092064);
  h_bowMagEvolIBL2->SetBinError(47,0.4511144);

  // 2016 Initial bowing
  int noffset = nruns_2015 + 1; 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 1, -1.09319);  h_bowMagEvolIBL2->SetBinError(noffset+ 1, 0.726296); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 2,  0.858415); h_bowMagEvolIBL2->SetBinError(noffset+ 2, 1.13415); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 3,  1.12282);  h_bowMagEvolIBL2->SetBinError(noffset+ 3, 1.20037); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 4,  2.61717);  h_bowMagEvolIBL2->SetBinError(noffset+ 4, 0.630301); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 5,  3.04281);  h_bowMagEvolIBL2->SetBinError(noffset+ 5, 1.68062); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 6,  4.53271);  h_bowMagEvolIBL2->SetBinError(noffset+ 6, 1.31229); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 7,  5.18743);  h_bowMagEvolIBL2->SetBinError(noffset+ 7, 0.893422); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 8,  6.40669);  h_bowMagEvolIBL2->SetBinError(noffset+ 8, 0.645387); 
  h_bowMagEvolIBL2->SetBinContent(noffset+ 9,  6.46948);  h_bowMagEvolIBL2->SetBinError(noffset+ 9, 0.921436); 
  h_bowMagEvolIBL2->SetBinContent(noffset+10,  6.96103);  h_bowMagEvolIBL2->SetBinError(noffset+10, 0.967207); 

  h_bowMagEvolIBL2->SetBinContent(noffset+11,  9.61658);  h_bowMagEvolIBL2->SetBinError(noffset+11, 0.63359); 
  h_bowMagEvolIBL2->SetBinContent(noffset+12,  7.74762);  h_bowMagEvolIBL2->SetBinError(noffset+12, 0.534215); 
  h_bowMagEvolIBL2->SetBinContent(noffset+13,  7.53439);  h_bowMagEvolIBL2->SetBinError(noffset+13, 0.536437); 
  h_bowMagEvolIBL2->SetBinContent(noffset+14,  8.53997);  h_bowMagEvolIBL2->SetBinError(noffset+14, 0.615471); 
  h_bowMagEvolIBL2->SetBinContent(noffset+15,  7.09937);  h_bowMagEvolIBL2->SetBinError(noffset+15, 0.623519); 
  h_bowMagEvolIBL2->SetBinContent(noffset+16,  9.36805);  h_bowMagEvolIBL2->SetBinError(noffset+16, 0.484097); 
  h_bowMagEvolIBL2->SetBinContent(noffset+17,  9.46302);  h_bowMagEvolIBL2->SetBinError(noffset+17, 0.852412); 
  h_bowMagEvolIBL2->SetBinContent(noffset+18,  9.72661);  h_bowMagEvolIBL2->SetBinError(noffset+18, 0.930376); 
  h_bowMagEvolIBL2->SetBinContent(noffset+19,  9.25499);  h_bowMagEvolIBL2->SetBinError(noffset+19, 0.600801); 
  h_bowMagEvolIBL2->SetBinContent(noffset+20,  7.61856);  h_bowMagEvolIBL2->SetBinError(noffset+20, 1.28417); 

  h_bowMagEvolIBL2->SetBinContent(noffset+21,  9.13656);  h_bowMagEvolIBL2->SetBinError(noffset+21, 1.01832); 
  h_bowMagEvolIBL2->SetBinContent(noffset+22,  8.72418);  h_bowMagEvolIBL2->SetBinError(noffset+22, 0.420864); 
  h_bowMagEvolIBL2->SetBinContent(noffset+23,  6.04773);  h_bowMagEvolIBL2->SetBinError(noffset+23, 3.20476); 
  h_bowMagEvolIBL2->SetBinContent(noffset+24,  9.13688);  h_bowMagEvolIBL2->SetBinError(noffset+24, 0.510128); 
  h_bowMagEvolIBL2->SetBinContent(noffset+25,  7.97921);  h_bowMagEvolIBL2->SetBinError(noffset+25, 0.494575); 
  h_bowMagEvolIBL2->SetBinContent(noffset+26,  7.01375);  h_bowMagEvolIBL2->SetBinError(noffset+26, 0.469563); 
  h_bowMagEvolIBL2->SetBinContent(noffset+27, 10.2817);   h_bowMagEvolIBL2->SetBinError(noffset+27, 0.496946); 
  h_bowMagEvolIBL2->SetBinContent(noffset+28,  7.59964);  h_bowMagEvolIBL2->SetBinError(noffset+28, 0.703863); 
  h_bowMagEvolIBL2->SetBinContent(noffset+29,  6.78474);  h_bowMagEvolIBL2->SetBinError(noffset+29, 0.468381); 
  h_bowMagEvolIBL2->SetBinContent(noffset+30,  7.85488);  h_bowMagEvolIBL2->SetBinError(noffset+30, 0.439373); 

  h_bowMagEvolIBL2->SetBinContent(noffset+31,  4.57062);  h_bowMagEvolIBL2->SetBinError(noffset+31, 0.455782); 
  h_bowMagEvolIBL2->SetBinContent(noffset+32,  3.33413);  h_bowMagEvolIBL2->SetBinError(noffset+32, 0.441117); 
  h_bowMagEvolIBL2->SetBinContent(noffset+33,  0.602291); h_bowMagEvolIBL2->SetBinError(noffset+33, 0.469204); 
  h_bowMagEvolIBL2->SetBinContent(noffset+34, -0.258273); h_bowMagEvolIBL2->SetBinError(noffset+34, 0.399322); 
  h_bowMagEvolIBL2->SetBinContent(noffset+35, -1.42844);  h_bowMagEvolIBL2->SetBinError(noffset+35, 0.389006); 
  h_bowMagEvolIBL2->SetBinContent(noffset+36, -1.90697);  h_bowMagEvolIBL2->SetBinError(noffset+36, 0.405537); 
  h_bowMagEvolIBL2->SetBinContent(noffset+37, -2.45898);  h_bowMagEvolIBL2->SetBinError(noffset+37, 0.439876); 
  h_bowMagEvolIBL2->SetBinContent(noffset+38, -3.78498);  h_bowMagEvolIBL2->SetBinError(noffset+38, 0.493806); 
  h_bowMagEvolIBL2->SetBinContent(noffset+39, -3.44989);  h_bowMagEvolIBL2->SetBinError(noffset+39, 0.549432); 
  h_bowMagEvolIBL2->SetBinContent(noffset+40, -3.18449);  h_bowMagEvolIBL2->SetBinError(noffset+40, 0.39276); 

  h_bowMagEvolIBL2->SetBinContent(noffset+41, -3.85065);  h_bowMagEvolIBL2->SetBinError(noffset+41, 0.440223); 
  h_bowMagEvolIBL2->SetBinContent(noffset+42, -3.46377);  h_bowMagEvolIBL2->SetBinError(noffset+42, 0.504711); 
  h_bowMagEvolIBL2->SetBinContent(noffset+43, -3.45319);  h_bowMagEvolIBL2->SetBinError(noffset+43, 0.480298); 
  h_bowMagEvolIBL2->SetBinContent(noffset+44, -3.58678);  h_bowMagEvolIBL2->SetBinError(noffset+44, 0.387737); 
  h_bowMagEvolIBL2->SetBinContent(noffset+45, -3.11562);  h_bowMagEvolIBL2->SetBinError(noffset+45, 0.546831); 
  h_bowMagEvolIBL2->SetBinContent(noffset+46, -3.37522);  h_bowMagEvolIBL2->SetBinError(noffset+46, 0.392157); 
  h_bowMagEvolIBL2->SetBinContent(noffset+47, -4.22588);  h_bowMagEvolIBL2->SetBinError(noffset+47, 0.676302); 
  h_bowMagEvolIBL2->SetBinContent(noffset+48, -4.24833);  h_bowMagEvolIBL2->SetBinError(noffset+48, 0.381939); 
  h_bowMagEvolIBL2->SetBinContent(noffset+49, -3.57547);  h_bowMagEvolIBL2->SetBinError(noffset+49, 0.575213); 
  h_bowMagEvolIBL2->SetBinContent(noffset+50, -4.10227);  h_bowMagEvolIBL2->SetBinError(noffset+50, 0.470863); 

  h_bowMagEvolIBL2->SetBinContent(noffset+51, -4.41851);  h_bowMagEvolIBL2->SetBinError(noffset+51, 0.411748); 
  h_bowMagEvolIBL2->SetBinContent(noffset+52, -3.75934);  h_bowMagEvolIBL2->SetBinError(noffset+52, 0.544677); 
  h_bowMagEvolIBL2->SetBinContent(noffset+53, -3.60712);  h_bowMagEvolIBL2->SetBinError(noffset+53, 0.522931); 
  h_bowMagEvolIBL2->SetBinContent(noffset+54, -3.35196);  h_bowMagEvolIBL2->SetBinError(noffset+54, 0.542296); 
  h_bowMagEvolIBL2->SetBinContent(noffset+55, -4.31438);  h_bowMagEvolIBL2->SetBinError(noffset+55, 0.39307); 
  h_bowMagEvolIBL2->SetBinContent(noffset+56, -4.65765);  h_bowMagEvolIBL2->SetBinError(noffset+56, 0.915422); 
  h_bowMagEvolIBL2->SetBinContent(noffset+57, -4.24684);  h_bowMagEvolIBL2->SetBinError(noffset+57, 0.424396); 
  h_bowMagEvolIBL2->SetBinContent(noffset+58, -4.91104);  h_bowMagEvolIBL2->SetBinError(noffset+58, 0.46234); 

  // 2016 bowing after alignment
  h_bowMagEvolIBL1->SetBinContent(noffset+ 1, -1.96399);  h_bowMagEvolIBL1->SetBinError(noffset+ 1, 0.724774);  //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+1,"24/04/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 2, -2.72711);  h_bowMagEvolIBL1->SetBinError(noffset+ 2, 1.1182);  //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+2,"25/04/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 3,  0.260247); h_bowMagEvolIBL1->SetBinError(noffset+ 3, 1.202574); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+3,"27/04/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 4,  1.49857);  h_bowMagEvolIBL1->SetBinError(noffset+ 4, 0.627577); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+4,"29/04/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 5, -0.86344);  h_bowMagEvolIBL1->SetBinError(noffset+ 5, 0.971603); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+5,"07/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 6, -0.59015);  h_bowMagEvolIBL1->SetBinError(noffset+ 6, 0.769932); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+6,"08/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 7, -0.219211); h_bowMagEvolIBL1->SetBinError(noffset+ 7, 0.577161); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+7,"09/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 8, -0.551176); h_bowMagEvolIBL1->SetBinError(noffset+ 8, 0.480291); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+8,"10/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+ 9, -0.277831); h_bowMagEvolIBL1->SetBinError(noffset+ 9, 0.724774); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+9,"10/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+10, -0.526703); h_bowMagEvolIBL1->SetBinError(noffset+10, 0.638936); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+10,"11/05/16");

  h_bowMagEvolIBL1->SetBinContent(noffset+11, -0.150444); h_bowMagEvolIBL1->SetBinError(noffset+11, 0.470343);  //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+11,"12/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+12, -0.0634404);  h_bowMagEvolIBL1->SetBinError(noffset+12, 0.427838); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+12,"13/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+13, -0.253839); h_bowMagEvolIBL1->SetBinError(noffset+13, 0.428988); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+13,"14/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+14,  0.121999);  h_bowMagEvolIBL1->SetBinError(noffset+14, 0.461212); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+14,"14/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+15,  0.332271);  h_bowMagEvolIBL1->SetBinError(noffset+15, 0.471041); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+15,"15/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+16,  0.600473);  h_bowMagEvolIBL1->SetBinError(noffset+16, 0.40916); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+16,"16/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+17,  0.468158); h_bowMagEvolIBL1->SetBinError(noffset+17, 0.579008); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+17,"16/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+18,  1.16536); h_bowMagEvolIBL1->SetBinError(noffset+18, 0.615315); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+18,"17/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+19,  0.560783); h_bowMagEvolIBL1->SetBinError(noffset+19, 0.460122); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+19,"18/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+20,  0.795769); h_bowMagEvolIBL1->SetBinError(noffset+20, 0.812581); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+20,"18/05/16");

  h_bowMagEvolIBL1->SetBinContent(noffset+21, 1.27305);  h_bowMagEvolIBL1->SetBinError(noffset+21, 0.670996); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+21,"21/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+22, 0.547907); h_bowMagEvolIBL1->SetBinError(noffset+22, 0.377334); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+22,"27/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+23, 1.42712);  h_bowMagEvolIBL1->SetBinError(noffset+23, 1.91749); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+23,"28/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+24, 1.31194);  h_bowMagEvolIBL1->SetBinError(noffset+24, 0.42303); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+24,"30/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+25, 1.62115);  h_bowMagEvolIBL1->SetBinError(noffset+25, 0.416707); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+25,"31/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+26, 1.3652);  h_bowMagEvolIBL1->SetBinError(noffset+26, 0.404798); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+26,"31/05/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+27,  2.18016);  h_bowMagEvolIBL1->SetBinError(noffset+27, 0.428575); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+27,"01/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+28,  2.25136);   h_bowMagEvolIBL1->SetBinError(noffset+28, 0.531716); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+28,"02/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+29,  2.65414);  h_bowMagEvolIBL1->SetBinError(noffset+29, 0.408073); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+29,"03/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+30, 2.5564);  h_bowMagEvolIBL1->SetBinError(noffset+30, 0.390734); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+30,"04/06/16");

  h_bowMagEvolIBL1->SetBinContent(noffset+31, 1.28252);  h_bowMagEvolIBL1->SetBinError(noffset+31,0.399183); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+31,"05/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+32, 0.59300); h_bowMagEvolIBL1->SetBinError(noffset+32, 0.391888); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+32,"06/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+33, 1.41979);  h_bowMagEvolIBL1->SetBinError(noffset+33, 0.4087); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+33,"15/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+34, -5.83847);  h_bowMagEvolIBL1->SetBinError(noffset+34,0.397381); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+34,"15/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+35, -6.26797);  h_bowMagEvolIBL1->SetBinError(noffset+35, 0.387407); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+35,"15/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+36, -5.41707);   h_bowMagEvolIBL1->SetBinError(noffset+36, 0.402859); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+36,"16/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+37, -5.40687); h_bowMagEvolIBL1->SetBinError(noffset+37, 0.435224); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+37,"17/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+38, -4.13609); h_bowMagEvolIBL1->SetBinError(noffset+38, 0.488578); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+38,"17/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+39, -4.39038); h_bowMagEvolIBL1->SetBinError(noffset+39,0.542634);// h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+39,"18/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+40, -5.04054);   h_bowMagEvolIBL1->SetBinError(noffset+40, 0.39175); // h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+40,"19/06/16");

  h_bowMagEvolIBL1->SetBinContent(noffset+41, -0.640258);  h_bowMagEvolIBL1->SetBinError(noffset+41, 0.437891); // h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+41,"19/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+42, -0.503099); h_bowMagEvolIBL1->SetBinError(noffset+42, 0.498335); // h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+42,"20/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+43, -0.444113);  h_bowMagEvolIBL1->SetBinError(noffset+43, 0.525269); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+43,"21/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+44, -0.159562);  h_bowMagEvolIBL1->SetBinError(noffset+44, 0.387441); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+44,"24/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+45, -0.368219);  h_bowMagEvolIBL1->SetBinError(noffset+45, 0.54017); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+45,"26/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+46, -0.452835);   h_bowMagEvolIBL1->SetBinError(noffset+46, 0.391741); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+46,"26/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+47, -0.663758); h_bowMagEvolIBL1->SetBinError(noffset+47, 0.6772); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+47,"28/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+48, -0.41907); h_bowMagEvolIBL1->SetBinError(noffset+48, 0.381287); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+48,"28/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+49, -0.458063); h_bowMagEvolIBL1->SetBinError(noffset+49, 0.565279); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+49,"28/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+50, -0.636045);   h_bowMagEvolIBL1->SetBinError(noffset+50, 0.467322); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+50,"29/06/16");

  h_bowMagEvolIBL1->SetBinContent(noffset+51, -0.715439);  h_bowMagEvolIBL1->SetBinError(noffset+51, 0.410293); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+51,"29/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+52, -0.57544); h_bowMagEvolIBL1->SetBinError(noffset+52, 0.537664); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+51,"29/06/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+53, -0.404361);  h_bowMagEvolIBL1->SetBinError(noffset+53,0.51515); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+53,"01/07/16"); 
  h_bowMagEvolIBL1->SetBinContent(noffset+54, -0.462195);  h_bowMagEvolIBL1->SetBinError(noffset+54, 0.531714); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+54,"02/07/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+55, -0.48065);  h_bowMagEvolIBL1->SetBinError(noffset+55, 0.391687); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+55,"03/07/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+56, -0.380608);   h_bowMagEvolIBL1->SetBinError(noffset+56, 0.896067); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+56,"04/07/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+57, -0.214873); h_bowMagEvolIBL1->SetBinError(noffset+57, 0.422862); //h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+57,"04/07/16");
  h_bowMagEvolIBL1->SetBinContent(noffset+58, -0.712058); h_bowMagEvolIBL1->SetBinError(noffset+58, 0.459442); h_bowMagEvolIBL1->GetXaxis()->SetBinLabel(noffset+58,"04/07/16");


  h_bowMagEvolIBL2->SetMinimum(-30);
  h_bowMagEvolIBL2->SetMaximum(11);
  //h_bowMagEvolIBL2->SetEntries(47);
  h_bowMagEvolIBL2->SetStats(0);
  h_bowMagEvolIBL2->Draw("same");
  Int_t ci;      // for color index setting
  TColor *color; // for color definition with alpha
  ci = TColor::GetColor("#0033cc");
  h_bowMagEvolIBL2->SetLineColor(ci);
  h_bowMagEvolIBL2->SetLineWidth(2);
  
  ci = TColor::GetColor("#0033cc");
  h_bowMagEvolIBL2->SetMarkerColor(ci);
  h_bowMagEvolIBL2->SetMarkerStyle(24);
  h_bowMagEvolIBL2->Draw("same");
  
  
//  
//  TH1F *h_bowMagEvolIBL3 = new TH1F("h_bowMagEvolIBL3","IBL all staves bowing magnitude evolution per run",47,-0.5,46.5);
//  h_bowMagEvolIBL3->SetBinContent(28,-0.4441209);
//  h_bowMagEvolIBL3->SetBinContent(29,0.7010919);
//  h_bowMagEvolIBL3->SetBinContent(30,-0.988483);
//  h_bowMagEvolIBL3->SetBinContent(31,-1.473469);
//  h_bowMagEvolIBL3->SetBinContent(32,-1.764734);
//  h_bowMagEvolIBL3->SetBinContent(33,-0.418724);
//  h_bowMagEvolIBL3->SetBinContent(34,-2.540311);
//  h_bowMagEvolIBL3->SetBinContent(35,-2.674458);
//  h_bowMagEvolIBL3->SetBinContent(36,-1.636667);
//  h_bowMagEvolIBL3->SetBinContent(37,-0.6487529);
//  h_bowMagEvolIBL3->SetBinContent(38,-1.197403);
//  h_bowMagEvolIBL3->SetBinContent(39,-1.625576);
//  h_bowMagEvolIBL3->SetBinContent(40,-0.999604);
//  h_bowMagEvolIBL3->SetBinContent(41,-1.068903);
//  h_bowMagEvolIBL3->SetBinContent(42,-0.922247);
//  h_bowMagEvolIBL3->SetBinContent(43,-1.081638);
//  h_bowMagEvolIBL3->SetBinContent(44,-0.8787865);
//  h_bowMagEvolIBL3->SetBinContent(45,-1.286169);
//  h_bowMagEvolIBL3->SetBinContent(46,-1.784337);
//  h_bowMagEvolIBL3->SetBinContent(47,-0.763969);
//  h_bowMagEvolIBL3->SetBinError(28,0.8135155);
//  h_bowMagEvolIBL3->SetBinError(29,0.7993051);
//  h_bowMagEvolIBL3->SetBinError(30,1.668258);
//  h_bowMagEvolIBL3->SetBinError(31,1.255279);
//  h_bowMagEvolIBL3->SetBinError(32,0.8164883);
//  h_bowMagEvolIBL3->SetBinError(33,1.440603);
//  h_bowMagEvolIBL3->SetBinError(34,0.8101156);
//  h_bowMagEvolIBL3->SetBinError(35,0.8463334);
//  h_bowMagEvolIBL3->SetBinError(36,1.11358);
//  h_bowMagEvolIBL3->SetBinError(37,1.46284);
//  h_bowMagEvolIBL3->SetBinError(38,0.3996555);
//  h_bowMagEvolIBL3->SetBinError(39,0.5118914);
//  h_bowMagEvolIBL3->SetBinError(40,0.4131203);
//  h_bowMagEvolIBL3->SetBinError(41,0.5056021);
//  h_bowMagEvolIBL3->SetBinError(42,0.5434087);
//  h_bowMagEvolIBL3->SetBinError(43,0.4497851);
//  h_bowMagEvolIBL3->SetBinError(44,0.4227611);
//  h_bowMagEvolIBL3->SetBinError(45,0.5309765);
//  h_bowMagEvolIBL3->SetBinError(46,0.6609266);
//  h_bowMagEvolIBL3->SetBinError(47,0.4843258);
//  h_bowMagEvolIBL3->SetMinimum(-36);
//  h_bowMagEvolIBL3->SetMaximum(12);
//  h_bowMagEvolIBL3->SetEntries(20);
//  h_bowMagEvolIBL3->SetStats(0);
//  h_bowMagEvolIBL3->SetLineColor(kGray+3);
//  h_bowMagEvolIBL3->SetLineWidth(2);
//  
//  //   ci = TColor::GetColor("#0033cc");
//  h_bowMagEvolIBL3->SetMarkerColor(kGray+3);
//  h_bowMagEvolIBL3->SetMarkerStyle(20);
//  h_bowMagEvolIBL3->Draw("same");
//  
  
  
   ci = TColor::GetColor("#666666");
   line->SetLineColor(ci);
   line->SetLineStyle(2);
   line->Draw();
  
   line->DrawLine(nruns_2015, h_bowMagEvolIBL1->GetMinimum(), nruns_2015, h_bowMagEvolIBL1->GetMaximum());


//   TPaveText *pt = new TPaveText(0.15,0.9339831,0.85,0.995,"blNDC");
//   pt->SetName("title");
//   pt->SetBorderSize(0);
//   pt->SetFillColor(0);
//   pt->SetFillStyle(0);
//   pt->SetTextFont(42);
//   TText *AText = pt->AddText("IBL all staves bowing magnitude evolution per run");
//   pt->Draw();
//   CanBowingMagEvolForStaveAll->Modified();
//   CanBowingMagEvolForStaveAll->cd();
//   CanBowingMagEvolForStaveAll->SetSelected(CanBowingMagEvolForStaveAll);
  

  
  ATLASLabel(0.15,0.85,"Internal",1,0.05);
  TLatex l;
  l.SetTextSize(0.05);
  l.SetNDC();
  l.SetTextColor(1);
  // l.DrawLatex(0.4,0.95,"Preliminary");
  l.SetTextSize(0.04);
  //l.DrawLatex(0.275,0.85,"#sqrt{s}=13 TeV");
 // l.DrawLatex(0.275,0.85,"Preliminary");
  
  
  TLegend* leg = new TLegend(0.15,0.675,0.3,0.825);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetTextSize(0.04);
  leg->SetShadowColor(0);
  leg->SetHeader("Data 2015 / 2016");
  leg->AddEntry(h_bowMagEvolIBL2,"Default alignment","ep");
  leg->AddEntry(h_bowMagEvolIBL1,"Run averaged alignment","ep");
  leg->Draw("same");
  
  CanBowingMagEvolForStaveAll->Print("BowingMagEvolForStaveAll.pdf");
  CanBowingMagEvolForStaveAll->Print("BowingMagEvolForStaveAll.png");
  
}













