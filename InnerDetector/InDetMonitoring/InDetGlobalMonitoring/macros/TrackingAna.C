/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// tope level executable

#include "RealTrackTree.h"
#include "TApplication.h"
#include "TFile.h"
#include "TStyle.h"
#include <iostream>
#include "TString.h"
#include "TChain.h"

void SetAtlasStyle ();
TStyle* AtlasStyle();

int main(int argc, char **argv)
//int TrackingAna()
{
TApplication theApp("App", &argc, argv);

TString outFileName("outputFile.root");

// example input, it is better to copy locally for faster processing
TChain* chain = new TChain("InDetTrackTree");

// the first argument is -b such that at tier 0 it runs in batch (i.e. no grahics)
if (theApp.Argc()>1)
{
  outFileName = theApp.Argv()[theApp.Argc()-1];
  for (unsigned int nfiles = 2 ; nfiles < theApp.Argc()-1 ; nfiles++)
  {
    chain->Add(theApp.Argv()[nfiles]);
    //std::cout << "MACRO: " << theApp.Argv()[nfiles] << std::endl;
  }
} else


{
SetAtlasStyle();

/*
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00001.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00002.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00003.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00004.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00005.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00006.root");

chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00007.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00008.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00009.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00010.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00011.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00012.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00013.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00014.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00015.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00016.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00017.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00018.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00019.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.merge.NTUP_VTXMON.e517_s764_s767_r1316_r1260.V2.101126144938/user.AndreasWildauer.000105.D3PD._00020.root");
*/


chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00001.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00002.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00003.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00004.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00005.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00006.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00007.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00008.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00009.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00010.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00011.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00012.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00013.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00014.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00015.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00016.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00017.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00018.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00019.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00020.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00021.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00022.root");



/*

chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00011.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00012.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00013.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00014.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00015.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00016.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00017.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00018.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00019.root");
chain->Add("root://castoratlas//castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/user.AndreasWildauer.mc09_7TeV.105001.pythia_minbias.NTUP_VTXMON.e517_s764_s767_r1302_r1306.V2/user.AndreasWildauer.000111.D3PD._00020.root");

*/
}																															


																												
RealTrackTree t(chain, outFileName);																											
t.Loop();																														
																															
// arguments should only be given when run at tier 0																									
if (theApp.Argc() <= 1 ) { theApp.Run(); }																										
return 0;																														
}

// the rest is taken from the ATLAS style wiki, and adjusted that it actually compiles ...
TStyle* AtlasStyle() 
{
  TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.05);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.16);
  atlasStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.4);
  atlasStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");
  
  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(1.2);
  atlasStyle->SetHistLineWidth(2);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
//  atlasStyle->SetOptTitle(0);
  atlasStyle->SetOptStat(111111);
//  atlasStyle->SetOptStat(0);
  atlasStyle->SetOptFit(1111111);
//  atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  atlasStyle->SetPalette(1);
  
  atlasStyle->SetPadRightMargin(15);
  
  return atlasStyle;

}

void SetAtlasStyle ()
{
   std::cout << "\nApplying ATLAS style settings...\n" << std::endl ;
   TStyle* atlasStyle = AtlasStyle();
   gROOT->SetStyle("ATLAS");
   gROOT->ForceStyle();
}
