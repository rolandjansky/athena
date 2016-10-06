/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonShowerShapeFudgeTool/FudgeMCTool.h"
#include "TFile.h"
#include "TH2D.h"
#include "TString.h"

int main() {

  FudgeMCTool fftool(0,0,0,0);
  TFile* f = new TFile("ElectronPhotonShowerShapeFudgeTool/data/PhotonFudgeFactors.root","UPDATE");

  for (int preselection = 0; preselection < 19; preselection++) {
    if (preselection == 9) continue;

    TString directory = Form("TUNE%d",preselection);
    bool retvalue = f->Cd(directory);
    if (retvalue) {
      std::cout << "WARNING: directory " << directory << " already exists. Skipping..." << std::endl;
      continue;
    }
    f->mkdir(directory);
    f->Cd(directory);
    
    TString name;
    TH2D* h_u_rhad1 = fftool.GetFFTH2D(IDVAR::RHAD1,0,preselection);
    name = "FF_RHAD1_UNCONV"; h_u_rhad1->SetNameTitle(name,name);
    
    TH2D* h_u_rhad = fftool.GetFFTH2D(IDVAR::RHAD,0,preselection);
    name = "FF_RHAD_UNCONV"; h_u_rhad->SetNameTitle(name,name);
    
    TH2D* h_u_e277 = fftool.GetFFTH2D(IDVAR::E277,0,preselection);
    name = "FF_E277_UNCONV"; h_u_e277->SetNameTitle(name,name);
    
    TH2D* h_u_reta = fftool.GetFFTH2D(IDVAR::RETA,0,preselection);
    name = "FF_RETA_UNCONV"; h_u_reta->SetNameTitle(name,name);
    
    TH2D* h_u_rphi = fftool.GetFFTH2D(IDVAR::RPHI,0,preselection);
    name = "FF_RPHI_UNCONV"; h_u_rphi->SetNameTitle(name,name);
    
    TH2D* h_u_weta2 = fftool.GetFFTH2D(IDVAR::WETA2,0,preselection);
    name = "FF_WETA2_UNCONV"; h_u_weta2->SetNameTitle(name,name);
    
    TH2D* h_u_f1 = fftool.GetFFTH2D(IDVAR::F1,0,preselection);
    name = "FF_F1_UNCONV"; h_u_f1->SetNameTitle(name,name);
    
    TH2D* h_u_fside = fftool.GetFFTH2D(IDVAR::FSIDE,0,preselection);
    name = "FF_FSIDE_UNCONV"; h_u_fside->SetNameTitle(name,name);
    
    TH2D* h_u_wtot = fftool.GetFFTH2D(IDVAR::WTOT,0,preselection);
    name = "FF_WTOT_UNCONV"; h_u_wtot->SetNameTitle(name,name);
    
    TH2D* h_u_w1 = fftool.GetFFTH2D(IDVAR::W1,0,preselection);
    name = "FF_W1_UNCONV"; h_u_w1->SetNameTitle(name,name);
    
    TH2D* h_u_de = fftool.GetFFTH2D(IDVAR::DE,0,preselection);
    name = "FF_DE_UNCONV"; h_u_de->SetNameTitle(name,name);
    
    TH2D* h_u_eratio = fftool.GetFFTH2D(IDVAR::ERATIO,0,preselection);
    name = "FF_ERATIO_UNCONV"; h_u_eratio->SetNameTitle(name,name);

    TH2D* h_c_rhad1 = fftool.GetFFTH2D(IDVAR::RHAD1,1,preselection);
    name = "FF_RHAD1_CONV"; h_c_rhad1->SetNameTitle(name,name);
    
    TH2D* h_c_rhad = fftool.GetFFTH2D(IDVAR::RHAD,1,preselection);
    name = "FF_RHAD_CONV"; h_c_rhad->SetNameTitle(name,name);
    
    TH2D* h_c_e277 = fftool.GetFFTH2D(IDVAR::E277,1,preselection);
    name = "FF_E277_CONV"; h_c_e277->SetNameTitle(name,name);
    
    TH2D* h_c_reta = fftool.GetFFTH2D(IDVAR::RETA,1,preselection);
    name = "FF_RETA_CONV"; h_c_reta->SetNameTitle(name,name);
    
    TH2D* h_c_rphi = fftool.GetFFTH2D(IDVAR::RPHI,1,preselection);
    name = "FF_RPHI_CONV"; h_c_rphi->SetNameTitle(name,name);
    
    TH2D* h_c_weta2 = fftool.GetFFTH2D(IDVAR::WETA2,1,preselection);
    name = "FF_WETA2_CONV"; h_c_weta2->SetNameTitle(name,name);
    
    TH2D* h_c_f1 = fftool.GetFFTH2D(IDVAR::F1,1,preselection);
    name = "FF_F1_CONV"; h_c_f1->SetNameTitle(name,name);
    
    TH2D* h_c_fside = fftool.GetFFTH2D(IDVAR::FSIDE,1,preselection);
    name = "FF_FSIDE_CONV"; h_c_fside->SetNameTitle(name,name);
    
    TH2D* h_c_wtot = fftool.GetFFTH2D(IDVAR::WTOT,1,preselection);
    name = "FF_WTOT_CONV"; h_c_wtot->SetNameTitle(name,name);
    
    TH2D* h_c_w1 = fftool.GetFFTH2D(IDVAR::W1,1,preselection);
    name = "FF_W1_CONV"; h_c_w1->SetNameTitle(name,name);
    
    TH2D* h_c_de = fftool.GetFFTH2D(IDVAR::DE,1,preselection);
    name = "FF_DE_CONV"; h_c_de->SetNameTitle(name,name);
    
    TH2D* h_c_eratio = fftool.GetFFTH2D(IDVAR::ERATIO,1,preselection);
    name = "FF_ERATIO_CONV"; h_c_eratio->SetNameTitle(name,name);

    f->Cd("..");
  }

  f->Write();
  f->Close();
  return 0;
}
