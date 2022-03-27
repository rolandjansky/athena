/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#define TPhotonMCShifterTool_cxx

#include "ElectronPhotonShowerShapeFudgeTool/TPhotonMCShifterTool.h"
#include "PathResolver/PathResolver.h"
#include "TMath.h"
#include "TFile.h"
#include <iostream>
#include <memory>
#include <stdexcept>

static const double GeV = 1.e+3;

using namespace IDVAR;

void TPhotonMCShifterTool::FudgeShowers( double  pt     ,
					 double  eta2   ,
					 double& rhad1  ,
					 double& rhad   ,
					 double& e277   ,
					 double& reta   ,
					 double& rphi   ,
					 double& weta2  ,
					 double& f1     ,
					 double& fside  ,
					 double& wtot   ,
					 double& w1     ,
					 double& deltae ,
					 double& eratio ,
					 int     isConv   ,
					 int     preselection)
{
  // only load new FF histograms if the tune changes
  if (preselection<0) preselection = m_preselection;
  if (preselection>0 && preselection!=m_preselection) {
    m_preselection = preselection;
    this->LoadFFs(preselection,m_corr_file);
  }

  //fudge showers
  rhad1  = Fudge_Rhad1(rhad1, pt, eta2, isConv);
  rhad   = Fudge_Rhad(rhad, pt, eta2, isConv);
  e277   = Fudge_E277(e277, pt, eta2, isConv);
  reta   = Fudge_Reta(reta, pt, eta2, isConv);
  rphi   = Fudge_Rphi(rphi, pt, eta2, isConv);
  weta2  = Fudge_Weta2(weta2, pt, eta2, isConv);
  f1     = Fudge_F1(f1, pt, eta2, isConv);
  deltae = Fudge_DE(deltae, pt, eta2, isConv);
  wtot   = Fudge_Wtot(wtot, pt, eta2, isConv);
  fside  = Fudge_Fside(fside, pt, eta2, isConv);
  w1     = Fudge_W1(w1, pt, eta2, isConv);
  eratio = Fudge_Eratio(eratio, pt, eta2, isConv);
}


void TPhotonMCShifterTool::FudgeShowers( float  pt     ,
					 float  eta2   ,
					 float& rhad1  ,
					 float& rhad   ,
					 float& e277   ,
					 float& reta   ,
					 float& rphi   ,
					 float& weta2  ,
					 float& f1     ,
					 float& fside  ,
					 float& wtot   ,
					 float& w1     ,
					 float& deltae ,
					 float& eratio ,
					 int     isConv   ,
					 int     preselection)
{
  // only load new FF histograms if the tune changes
  if (preselection<0) preselection = m_preselection;
  if (preselection>0 && preselection!=m_preselection) {
    m_preselection = preselection;
    this->LoadFFs(preselection,m_corr_file);
  }

  FudgeShowers (pt, eta2, rhad1, rhad, e277, reta, rphi,
                weta2, f1, fside, wtot, w1, deltae, eratio, isConv);
}


void TPhotonMCShifterTool::FudgeShowers( float  pt     ,
					 float  eta2   ,
					 float& rhad1  ,
					 float& rhad   ,
					 float& e277   ,
					 float& reta   ,
					 float& rphi   ,
					 float& weta2  ,
					 float& f1     ,
					 float& fside  ,
					 float& wtot   ,
					 float& w1     ,
					 float& deltae ,
					 float& eratio ,
					 int     isConv) const
{
  //fudge showers
  rhad1  = Fudge_Rhad1(rhad1, pt, eta2, isConv);
  rhad   = Fudge_Rhad(rhad, pt, eta2, isConv);
  e277   = Fudge_E277(e277, pt, eta2, isConv);
  reta   = Fudge_Reta(reta, pt, eta2, isConv);
  rphi   = Fudge_Rphi(rphi, pt, eta2, isConv);
  weta2  = Fudge_Weta2(weta2, pt, eta2, isConv);
  f1     = Fudge_F1(f1, pt, eta2, isConv);
  deltae = Fudge_DE(deltae, pt, eta2, isConv);
  wtot   = Fudge_Wtot(wtot, pt, eta2, isConv);
  fside  = Fudge_Fside(fside, pt, eta2, isConv);
  w1     = Fudge_W1(w1, pt, eta2, isConv);
  eratio = Fudge_Eratio(eratio, pt, eta2, isConv);
}

void TPhotonMCShifterTool::FudgeShowers(  std::vector<float> clE,
					  std::vector<float> eta2   ,
					  std::vector<float>& rhad1  ,
					  std::vector<float>& rhad   ,
					  std::vector<float>& e277   ,
					  std::vector<float>& reta   ,
					  std::vector<float>& rphi   ,
					  std::vector<float>& weta2  ,
					  std::vector<float>& f1     ,
					  std::vector<float>& fside  ,
					  std::vector<float>& wtot   ,
					  std::vector<float>& w1     ,
					  std::vector<float>& deltae ,
					  std::vector<float>& eratio ,
					  std::vector<int> isConv  ,
					  int    preselection)
{
  // Assume all vectors have the same size
  for (unsigned int i = 0; i < clE.size(); ++i)
    FudgeShowers(clE[i]/TMath::CosH(eta2[i]),
		 eta2  [i],
		 rhad1 [i],
		 rhad  [i],
		 e277  [i],
		 reta  [i],
		 rphi  [i],
		 weta2 [i],
		 f1    [i],
		 fside [i],
		 wtot  [i],
		 w1    [i],
		 deltae[i] ,
		 eratio[i] ,
		 isConv[i],
		 preselection);
}


void TPhotonMCShifterTool::LoadFFs(int preselection, const std::string& file)
{
  if (preselection == m_preselection) return;
  m_preselection = preselection;

  const std::string filename = PathResolverFindCalibFile( file );
  if (filename.empty()){
    throw std::runtime_error("FudgeMCTool  ERROR  Could NOT resolve file name "+ file);
  } else{
    Info("FudgeMCTool", "Path found = %s", filename.c_str());
  }
  m_corr_file = filename;
  std::unique_ptr< TFile > f( TFile::Open( m_corr_file.c_str(), "READ" ) );
  if( ( ! f.get() ) || f->IsZombie() ) {
     throw std::runtime_error( "Couldn't open file: " + m_corr_file );
  }
  if (!f->FindKey(Form("TUNE%d",preselection))) {
    throw std::runtime_error("Directory TUNE " + std::to_string(preselection) +
                             " does not exist in fudge factor file.");
  }

  h_u_rhad1 = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD1_UNCONV",preselection));
  h_u_rhad1->SetDirectory(nullptr);
 
  h_u_rhad = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD_UNCONV",preselection));
  h_u_rhad->SetDirectory(nullptr);
    
  h_u_e277 = (TH2D*) f->Get(Form("TUNE%d/FF_E277_UNCONV",preselection));
  h_u_e277->SetDirectory(nullptr);
    
  h_u_reta = (TH2D*) f->Get(Form("TUNE%d/FF_RETA_UNCONV",preselection));
  h_u_reta->SetDirectory(nullptr);
    
  h_u_rphi = (TH2D*) f->Get(Form("TUNE%d/FF_RPHI_UNCONV",preselection));
  h_u_rphi->SetDirectory(nullptr);
    
  h_u_weta2 =(TH2D*) f->Get(Form("TUNE%d/FF_WETA2_UNCONV",preselection));
  h_u_weta2->SetDirectory(nullptr);
    
  h_u_f1 = (TH2D*) f->Get(Form("TUNE%d/FF_F1_UNCONV",preselection));
  h_u_f1->SetDirectory(nullptr);
    
  h_u_fside = (TH2D*) f->Get(Form("TUNE%d/FF_FSIDE_UNCONV",preselection));
  h_u_fside->SetDirectory(nullptr);
    
  h_u_wtot = (TH2D*) f->Get(Form("TUNE%d/FF_WTOT_UNCONV",preselection));
  h_u_wtot->SetDirectory(nullptr);
    
  h_u_w1 = (TH2D*) f->Get(Form("TUNE%d/FF_W1_UNCONV",preselection));
  h_u_w1->SetDirectory(nullptr);
    
  h_u_de = (TH2D*) f->Get(Form("TUNE%d/FF_DE_UNCONV",preselection));
  h_u_de->SetDirectory(nullptr);
    
  h_u_eratio = (TH2D*) f->Get(Form("TUNE%d/FF_ERATIO_UNCONV",preselection));
  h_u_eratio->SetDirectory(nullptr);

  h_c_rhad1 = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD1_CONV",preselection));
  h_c_rhad1->SetDirectory(nullptr);
    
  h_c_rhad = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD_CONV",preselection));
  h_c_rhad->SetDirectory(nullptr);
    
  h_c_e277 = (TH2D*) f->Get(Form("TUNE%d/FF_E277_CONV",preselection));
  h_c_e277->SetDirectory(nullptr);
    
  h_c_reta = (TH2D*) f->Get(Form("TUNE%d/FF_RETA_CONV",preselection));
  h_c_reta->SetDirectory(nullptr);
    
  h_c_rphi = (TH2D*) f->Get(Form("TUNE%d/FF_RPHI_CONV",preselection));
  h_c_rphi->SetDirectory(nullptr);
    
  h_c_weta2 = (TH2D*) f->Get(Form("TUNE%d/FF_WETA2_CONV",preselection));
  h_c_weta2->SetDirectory(nullptr);
    
  h_c_f1 = (TH2D*) f->Get(Form("TUNE%d/FF_F1_CONV",preselection));
  h_c_f1->SetDirectory(nullptr);
    
  h_c_fside = (TH2D*) f->Get(Form("TUNE%d/FF_FSIDE_CONV",preselection));
  h_c_fside->SetDirectory(nullptr);
    
  h_c_wtot = (TH2D*) f->Get(Form("TUNE%d/FF_WTOT_CONV",preselection));
  h_c_wtot->SetDirectory(nullptr);
    
  h_c_w1 = (TH2D*) f->Get(Form("TUNE%d/FF_W1_CONV",preselection));
  h_c_w1->SetDirectory(nullptr);
    
  h_c_de = (TH2D*) f->Get(Form("TUNE%d/FF_DE_CONV",preselection));
  h_c_de->SetDirectory(nullptr);
    
  h_c_eratio = (TH2D*) f->Get(Form("TUNE%d/FF_ERATIO_CONV",preselection));
  h_c_eratio->SetDirectory(nullptr);

  f->Close();
}


//Get graph of FFs
TGraphErrors* TPhotonMCShifterTool::GetFFmap(int var, double eta, int isConv, int preselection ){
  if (preselection>=0 && preselection != m_preselection)
    this->LoadFFs(preselection,m_corr_file);

  if (h_u_rhad1==nullptr) return nullptr;
  
  const int ptbins = h_u_rhad1->GetNbinsX();

  // Strict C++ compiler in Athena does not allow variable widht arrays.
  // reserve enough space, use only first fraction to create TGraphErrors
  //double x[ptbins]; double xerr[ptbins];
  //double y[ptbins]; double yerr[ptbins];
  double x[100];	double xerr[100];
  double y[100];	double yerr[100];

  for (int i=0; i<ptbins; i++){

    x[i] = h_u_rhad1->GetXaxis()->GetBinCenter(i+1);
    xerr[i] = h_u_rhad1->GetXaxis()->GetBinWidth(i+1)/2.0;
    switch (var) {
    case IDVAR::RHAD1:
      y[i]    = GetFF_Rhad1( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rhad1( x[i], eta, isConv );
      break;
    case IDVAR::RHAD:
      y[i]    = GetFF_Rhad( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rhad( x[i], eta, isConv );
      break;
    case IDVAR::E277:
      y[i]    = GetFF_E277( x[i], eta, isConv );
      yerr[i] = GetFFerr_E277( x[i], eta, isConv );
      break;
    case IDVAR::RETA:
      y[i]    = GetFF_Reta( x[i], eta, isConv );
      yerr[i] = GetFFerr_Reta( x[i], eta, isConv );
      break;
    case IDVAR::RPHI:
      y[i]    = GetFF_Rphi( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rphi( x[i], eta, isConv );
      break;
    case IDVAR::WETA2:
      y[i]    = GetFF_Weta2( x[i], eta, isConv );
      yerr[i] = GetFFerr_Weta2( x[i], eta, isConv );
      break;
    case IDVAR::F1:
      y[i]    = GetFF_F1( x[i], eta, isConv );
      yerr[i] = GetFFerr_F1( x[i], eta, isConv );
      break;
    case IDVAR::FSIDE:
      y[i]    = GetFF_Fside( x[i], eta, isConv );
      yerr[i] = GetFFerr_Fside( x[i], eta, isConv );
      break;
    case IDVAR::WTOT:
      y[i]    = GetFF_Wtot( x[i], eta, isConv );
      yerr[i] = GetFFerr_Wtot( x[i], eta, isConv );
      break;
    case IDVAR::W1:
      y[i]    = GetFF_W1( x[i], eta, isConv );
      yerr[i] = GetFFerr_W1( x[i], eta, isConv );
      break;
    case IDVAR::DE:
      y[i]    = GetFF_DE( x[i], eta, isConv );
      yerr[i] = GetFFerr_DE( x[i], eta, isConv );
      break;
    case IDVAR::ERATIO:
      y[i]    = GetFF_Eratio( x[i], eta, isConv );
      yerr[i] = GetFFerr_Eratio( x[i], eta, isConv );
      break;
    default:
      break;
    }
  }
  
  //rescale to GeV
  for (int i=0; i<ptbins; i++){
    x[i] = x[i]/GeV;
    xerr[i] = xerr[i]/GeV;
  }
  
  TGraphErrors* graph = new TGraphErrors(ptbins,x,y,xerr,yerr);
  graph->GetXaxis()->SetTitle("p_{T} [GeV]");
  graph->GetYaxis()->SetTitle("FF");
  
  return graph;
}

TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rhad1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rhad(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_E277(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::E277 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Reta(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RETA , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rphi(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RPHI , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Weta2(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WETA2 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_F1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::F1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Fside(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::FSIDE , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Wtot(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WTOT , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_W1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::W1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_DE(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::DE , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Eratio(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::ERATIO , eta, isConv, preselection);
}

