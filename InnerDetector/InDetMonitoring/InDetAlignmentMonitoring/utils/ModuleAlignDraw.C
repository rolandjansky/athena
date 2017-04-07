/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// StaveDraw.C
//
#include "TCanvas.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TTree.h"
#include "TPad.h"
#include "TProfile.h"
#include "TString.h"
#include "TStyle.h"

#include "iostream"
enum {PIX=1, SCT=2, TRT=3};
#define Ndofs 6
TString dofName[Ndofs] = {"Tx", "Ty", "Tz", "Rx", "Ry", "Rz"};
TString unitsName[Ndofs] = {"[mm]", "[mm]", "[mm]", "[mrad]", "[mrad]", "[mrad]"};

float ComputeTz(int detec, int layer, double ring);
float ComputeEta(int detec, int ring);
//values
double tx, ty, tz;
double rx, ry, rz;
// errors
double etx, ety, etz;
double erx, ery, erz;


float UpperRing = 9;
float LowerRing = -10;
float NRings = 20;

void ModuleAlignDraw(char nname[80]=NULL,Int_t DetType= 1, Int_t Layer= 0, float UserRange =-1)
{
  int Sectors=0;
  float MaxT = -1;
  int PrintLevel = 3;
  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetPadColor(kWhite);
  gStyle->SetFrameFillColor(kWhite);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadRightMargin(0.15);


  char tname[80];
  enum {TX=0, TY=1, TZ=2, RX=3, RY=4, RZ=5};

  if (PrintLevel>=3) std::cout << " -- ModuleAlignDraw -- opening input file " << nname << std::endl;
  if (nname == NULL) sprintf(nname,"ComparePositions.root");

  TFile* f0 = new TFile(nname);
  if (PrintLevel>=3) std::cout << " -- ModuleAlignDraw -- input file opened " << nname << std::endl;

  Float_t TransMax = 0.30;
  if (MaxT>0) TransMax = MaxT;
  Float_t TransMin = -TransMax;

 
  Float_t TransScaleFactor = 1.;
  Float_t RotScaleFactor = 0.1;

  Float_t RotMax = TransMax/RotScaleFactor;
  Float_t RotMin = -RotMax;

  // sort out the detector type
  if (DetType<=0) {std::cout << " -- StaveDraw -- unknown detector type: " << DetType << std::endl; return;}

  // sort out the layer
  if (Layer<0) Layer = 0;
  if (DetType == PIX && Layer>3) Layer = 3;
  if (DetType == SCT && Layer>3) Layer = 3;
 
  // sort out the stave for pixels
  if (Sectors<0) Sectors = 0;
  if (DetType == PIX && Layer==0) Sectors = 13; // IBL
  if (DetType == PIX && Layer==1) Sectors = 21;
  if (DetType == PIX && Layer==2) Sectors = 37;
  if (DetType == PIX && Layer==3) Sectors = 51;
  if (DetType == PIX && Layer==0) {
    UpperRing = 9;
    LowerRing = -10;
    NRings = 20;
  }
  if (DetType == PIX && Layer>0) {
    UpperRing = 6;
    LowerRing = -6;
    NRings = 13;
  }

  // sort out the stave for the SCT
  if (DetType == SCT && Layer==0) Sectors = 31;
  if (DetType == SCT && Layer==1) Sectors = 39;
  if (DetType == SCT && Layer==2) Sectors = 47;
  if (DetType == SCT && Layer==3) Sectors = 51;

  if (DetType == SCT) {
    UpperRing = 5;
    LowerRing = -6;
    NRings = 12;
  }

  // TRT 
  if (DetType == TRT) {
    // as there is only one ring, the 3 layers are drawn in the same histogram
    Sectors = 32;
  }

  TString hName, hTitle;
  TString AxisTitle;

  TH2F* hAlignCorrectionsMap[Ndofs];
  TProfile* hAlignCorrStaveProfile[Ndofs];
  // now let's create the alignment corrections maps:
  for (int dof=0; dof < Ndofs; dof++) {
    hName.Clear();
    hName.Append("hAlignCorrMap");
    hName.Append(dofName[dof].Data());
    hTitle.Clear();
    if (DetType == PIX && Layer==0) hTitle.Append("IBL ");
    if (DetType == PIX && Layer >0) hTitle.Append("PIX Layer ");
    if (DetType == SCT) hTitle.Append("SCT Layer ");
    if (DetType == TRT) hTitle.Append("TRT Barrel ");
    if (!(DetType == PIX && Layer==0)) hTitle += Layer;
    hTitle.Append(": ");
    hTitle.Append(dofName[dof].Data());
    hTitle.Append(" corrections");

    if (DetType == PIX | DetType == SCT) 
      hAlignCorrectionsMap[dof] = new TH2F (hName.Data(), hTitle.Data(), NRings, LowerRing-0.5, UpperRing+0.5, Sectors, -0.5, Sectors-0.5);
    if (DetType == TRT) 
      hAlignCorrectionsMap[dof] = new TH2F (hName.Data(), hTitle.Data(), Sectors, -0.5, Sectors-0.5, 3, -0.5, 2.5); // sectors and layers

    hAlignCorrectionsMap[dof]->SetStats(false);
    hAlignCorrectionsMap[dof]->SetXTitle("#eta ring");
    hAlignCorrectionsMap[dof]->SetYTitle("#phi sector");
    if (DetType == SCT) { // set bin labels
      TString BinName;
      int thislabel = -6;
      for (int bin=1; bin<=  hAlignCorrectionsMap[dof]->GetNbinsX(); bin++){
	BinName.Clear(); 
	BinName += thislabel++;
	if (thislabel == 0) thislabel++;
	hAlignCorrectionsMap[dof]->GetXaxis()->SetBinLabel(bin, BinName.Data());
      }
    }
    if (DetType == TRT) {
      hAlignCorrectionsMap[dof]->SetXTitle("#phi sector");
      hAlignCorrectionsMap[dof]->SetYTitle("#layer");
    }      
    
    // create stave profiles
    hName.Append("_prof");
    hTitle.Append(" profile");
    if (DetType == PIX | DetType == SCT) 
      hAlignCorrStaveProfile[dof] = new TProfile(hName.Data(), hTitle.Data(), NRings, LowerRing-0.5, UpperRing+0.5);
    if (DetType == TRT) 
      hAlignCorrStaveProfile[dof] = new TProfile (hName.Data(), hTitle.Data(), Sectors, -0.5, Sectors-0.5); // sectors and layers
    hAlignCorrStaveProfile[dof]->SetMarkerStyle(20);
    hAlignCorrStaveProfile[dof]->SetStats(false);
    hAlignCorrStaveProfile[dof]->SetXTitle("#eta ring");
    AxisTitle.Clear();
    AxisTitle += dofName[dof];
    AxisTitle += " ";
    AxisTitle += unitsName[dof];
    hAlignCorrStaveProfile[dof]->SetYTitle(AxisTitle.Data());
  }

  TTree* Corrections = (TTree *) f0->Get("Corrections"); 
  double AlignCorrValues[6];
  double AlignCorrErrors[6];
  int type, bec, layer, sector, ring;
  Corrections->SetBranchAddress("type",  &type); 
  Corrections->SetBranchAddress("bec",   &bec); 
  Corrections->SetBranchAddress("layer", &layer); 
  Corrections->SetBranchAddress("sector",&sector); 
  Corrections->SetBranchAddress("ring",  &ring); 
  Corrections->SetBranchAddress("tx",    &AlignCorrValues[0]);
  Corrections->SetBranchAddress("ty",    &AlignCorrValues[1]);
  Corrections->SetBranchAddress("tz",    &AlignCorrValues[2]);
  Corrections->SetBranchAddress("rx",    &AlignCorrValues[3]);
  Corrections->SetBranchAddress("ry",    &AlignCorrValues[4]);
  Corrections->SetBranchAddress("rz",    &AlignCorrValues[5]);
  Corrections->SetBranchAddress("etx",   &AlignCorrErrors[0]);
  Corrections->SetBranchAddress("ety",   &AlignCorrErrors[1]);
  Corrections->SetBranchAddress("etz",   &AlignCorrErrors[2]);
  Corrections->SetBranchAddress("erx",   &AlignCorrErrors[3]);
  Corrections->SetBranchAddress("ery",   &AlignCorrErrors[4]);
  Corrections->SetBranchAddress("erz",   &AlignCorrErrors[5]);

  int NAlignedStruct = Corrections->GetEntries();
  if (PrintLevel >= 3) std::cout << " -- ModuleAlingDraw -- Number of aligned structures: " << NAlignedStruct << std::endl;

  for (int i=0; i < NAlignedStruct; i++) {
    Corrections->GetEntry(i);
    if (PrintLevel >= 5) std::cout << " -- ModuleAlingDraw -- looping on all structures: structure: " << i 
				   <<  "  detector: " << type 
				   <<  "  bec: " << bec 
				   <<  "  layer: " << layer 
				   << std::endl;
    if (type == DetType) { // is this entry for the detector we are asking ?
      if ( bec == 0) { // this is in the barrel 
	if (DetType == PIX | DetType == SCT) { // PIX and SCT are filled in one way, while TRT in another
	  if (layer == Layer) { // is for the same layer ?
	    float LogicEtaRing = ComputeEta(DetType, ring);
	    for (int dof = 0; dof < Ndofs; dof++) { // loop on all dofs
	      hAlignCorrectionsMap[dof]->Fill(LogicEtaRing, sector, AlignCorrValues[dof]);
	      // to set the error... it's a bit tricky
	      int thebin = hAlignCorrectionsMap[dof]->FindBin(LogicEtaRing, sector);
	      hAlignCorrectionsMap[dof]->SetBinError(thebin,AlignCorrErrors[dof]); 

	      if (PrintLevel >= 5) std::cout << "    -- ModuleAlingDraw -- " << dofName[dof].Data() << " = " << AlignCorrValues[dof]
					     << " thebin: " << thebin 
					     << " Error: " << AlignCorrErrors[dof]
					     << std::endl;
	      // fill the profile
	      hAlignCorrStaveProfile[dof]->Fill(LogicEtaRing, AlignCorrValues[dof]); 
	    }
	  }
	} // PIX and SCT
	if (DetType == TRT) { // filling TRT
	  for (int dof = 0; dof < Ndofs; dof++) { // loop on all dofs
	    hAlignCorrectionsMap[dof]->Fill(sector, layer, AlignCorrValues[dof]);
	  }
	}
      }
    }
  } 

  // Now histograms are filled. First thing, make sure range is symmetric
  // and set the Z axis title
  for (int dof = 0; dof < Ndofs; dof++) { // loop on all dofs
    // axis title
    AxisTitle.Clear();
    AxisTitle += dofName[dof];
    if (dof < 3) AxisTitle.Append(" [mm]");
    if (dof >=3) AxisTitle.Append(" [mrad]");
    hAlignCorrectionsMap[dof]->GetZaxis()->SetTitle(AxisTitle.Data()); 
    // range
    float maxValue = hAlignCorrectionsMap[dof]->GetMaximum();
    float minValue = hAlignCorrectionsMap[dof]->GetMinimum();

    if (fabs(maxValue)>fabs(minValue)) { maxValue = fabs(maxValue); minValue = -maxValue;}
    if (fabs(minValue)>fabs(maxValue)) { maxValue = fabs(minValue); minValue = -maxValue;}

    if (UserRange>0) {
      maxValue=  UserRange;
      minValue= -UserRange;
    }

    // check gap
    if (maxValue-minValue < 0.001) { maxValue = 0.001; minValue = -maxValue;}
    hAlignCorrectionsMap[dof]->SetMaximum(maxValue);
    hAlignCorrectionsMap[dof]->SetMinimum(minValue);
  }

  std::cout << " -- ModuleAlignDraw -- Drawing alignment corrections maps for type " << DetType << " layer: " << Layer << std::endl; 
  // Now let's print all the histograms and save it in a file
  TCanvas* cAlignCorrectionsMap[Ndofs];

  for (int dof=0; dof < Ndofs; dof++) {
    hName.Clear();
    hName.Append("cAlignCorrMap_");
    hName.Append(dofName[dof].Data());

    cAlignCorrectionsMap[dof] = new TCanvas (hName.Data(), hAlignCorrectionsMap[dof]->GetTitle(), 900, 650);
    hAlignCorrectionsMap[dof]->Draw("colz");

    // output file:
    hTitle.Clear();
    if (DetType == PIX && Layer==0) hTitle.Append("IBL_");
    if (DetType == PIX && Layer >0) hTitle.Append("PIX_Layer_");
    if (DetType == SCT) hTitle.Append("SCT_Layer_");
    if (!(DetType == PIX && Layer==0)) hTitle += Layer;
    if (DetType == TRT) {
      hTitle.Clear();
      hTitle.Append("TRT_Barrel_");
    }
    hTitle.Append(dofName[dof].Data());
    hTitle.Append("_map.png");
    
    cAlignCorrectionsMap[dof]->Print(hTitle.Data());
  }

  // test compute Tz and eta of the center of each IBL module
  std::vector<float> ringID, ringEta;
  for (int ring = -10; ring <=9; ring++) {
    float thisZ =  ComputeTz(PIX, 0, (double) ring);
    float thistheta = atan(33.45/thisZ); if (thistheta <0) thistheta += 3.14159265;
    float thiseta = -log(tan(thistheta/2));
    std::cout << " ring :" << ring << "  thisZ = " << thisZ << "  theta " << thistheta << "  eta " << thiseta << std::endl;
    ringID.push_back((float)ring);
    ringEta.push_back((float)thiseta);
  }
  
  //
  // draw the profiles 
  //
  TCanvas* cAlignCorrectionsProf[Ndofs];
  for (int dof=0; dof < Ndofs; dof++) {
    hName.Clear();
    hName.Append("cAlignCorrProf_");
    hName.Append(dofName[dof].Data());

    cAlignCorrectionsProf[dof] = new TCanvas (hName.Data(), hAlignCorrectionsMap[dof]->GetTitle(), 900, 650);
    hAlignCorrStaveProfile[dof]->Draw();

    hTitle.Clear();
    if (DetType == PIX && Layer==0) hTitle.Append("IBL_");
    if (DetType == PIX && Layer >0) hTitle.Append("PIX_Layer_");
    if (DetType == SCT) hTitle.Append("SCT_Layer_");
    if (!(DetType == PIX && Layer==0)) hTitle += Layer;
    if (DetType == TRT) {
      hTitle.Clear();
      hTitle.Append("TRT_Barrel_");
    }
    hTitle.Append(dofName[dof].Data());
    hTitle.Append("_prof.png");
    

    cAlignCorrectionsProf[dof]->Print(hTitle.Data());    
    
  }



  return;

}
/////////////////////////////////////////////////////
float ComputeTz(int detec, int Layer, double ring)
{
  bool debug = false;
  if (debug) std::cout << " -- ComputeTz -- for ring " << ring << " -- START -- " << std::endl; 
  float zpos = ring*20;
  if (detec == PIX) { 
    if (Layer == 0) { // IBL modules 
      // loop on ring till the desired one:
      zpos = -330; // far left edge
      float gap = 40; // 40 mm is the length of a planar module, 20 of a 3D one
      for (int i=-10; i <= ring; i++) {
	gap = 40; // refresh
	if ((-10 <= i) && (i <= -7)) gap = 20; // 3D modules 
	if ((6  <= i) && (i <=  9)) gap = 20; // 3D modules 
	zpos += gap;
	if (debug) std::cout << " ring " << i << " gap = " << gap << "  new zpos: " << zpos << std::endl;
      }
    }
    if (Layer > 0) { // Pixel modules 
      // loop on ring till the desired one:
      zpos = -441; // far left edge
      float gap = 63; // 63 mm is the length of a pixel module
      for (int i=-6; i <= ring; i++) {
	zpos += gap;
	if (debug) std::cout << " ring " << i << " gap = " << gap << "  new zpos: " << zpos << std::endl;
      }
    }
  }
  if (debug) std::cout << " -- ComputeTz -- ring: " << ring << "  Tz= " << zpos << std::endl;
 
  return zpos;
}
/////////////////////////////////////////////////////
float ComputeEta(int detec, int ring)
{
  // for pixel an dIBL, nothing to do:
  if (detec == PIX) return ring;

  // for SCT: negaive ring -> fine
  if (detec == SCT && ring < 0) return ring;

  // for SCT: positive ring -> subtract 1
  if (detec == SCT && ring > 0) return ring-1;

  return 0;
}
