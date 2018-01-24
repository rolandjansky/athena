/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TFile.h"
#include "G4UserActions/RadiationMapsMakerTool.h"

namespace G4UA{ 

  RadiationMapsMakerTool::RadiationMapsMakerTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
    : ActionToolBaseReport<RadiationMapsMaker>(type, name, parent)
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode RadiationMapsMakerTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    // first make sure the vectors are empty

    m_report.m_rz_tid .resize(0);
    m_report.m_rz_eion.resize(0);
    m_report.m_rz_niel.resize(0);
    m_report.m_rz_h20 .resize(0);
    
    m_report.m_full_rz_tid .resize(0);
    m_report.m_full_rz_eion.resize(0);
    m_report.m_full_rz_niel.resize(0);
    m_report.m_full_rz_h20 .resize(0);
    
    m_report.m_3d_tid .resize(0);
    m_report.m_3d_eion.resize(0);
    m_report.m_3d_niel.resize(0);
    m_report.m_3d_h20 .resize(0);

    // then resize to proper size and initialize with 0's 

    m_report.m_rz_tid .resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_rz_eion.resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_rz_niel.resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_rz_h20 .resize(m_report.nBinsz*m_report.nBinsr,0.0);
    
    m_report.m_full_rz_tid .resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_full_rz_eion.resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_full_rz_niel.resize(m_report.nBinsz*m_report.nBinsr,0.0);
    m_report.m_full_rz_h20 .resize(m_report.nBinsz*m_report.nBinsr,0.0);
    
    m_report.m_3d_tid .resize(m_report.nBinsz3d*m_report.nBinsr3d*m_report.nBinsphi3d,0.0);
    m_report.m_3d_eion.resize(m_report.nBinsz3d*m_report.nBinsr3d*m_report.nBinsphi3d,0.0);
    m_report.m_3d_niel.resize(m_report.nBinsz3d*m_report.nBinsr3d*m_report.nBinsphi3d,0.0);
    m_report.m_3d_h20 .resize(m_report.nBinsz3d*m_report.nBinsr3d*m_report.nBinsphi3d,0.0);

    // merge radiation map vectors from threads
    mergeReports();

    TFile * f = new TFile("RadMaps.root","RECREATE");

    TH2D * h_rz_tid  = new TH2D("rz_tid" ,"rz_tid" ,m_report.nBinsz,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr,m_report.rMinZoom,m_report.rMaxZoom);
    TH2D * h_rz_eion = new TH2D("rz_eion","rz_eion",m_report.nBinsz,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr,m_report.rMinZoom,m_report.rMaxZoom);
    TH2D * h_rz_niel = new TH2D("rz_niel","rz_niel",m_report.nBinsz,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr,m_report.rMinZoom,m_report.rMaxZoom);
    TH2D * h_rz_h20  = new TH2D("rz_h20" ,"rz_h20" ,m_report.nBinsz,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr,m_report.rMinZoom,m_report.rMaxZoom);

    h_rz_tid  ->SetXTitle("|z| [cm]");
    h_rz_eion ->SetXTitle("|z| [cm]");
    h_rz_niel ->SetXTitle("|z| [cm]");
    h_rz_h20  ->SetXTitle("|z| [cm]");

    h_rz_tid  ->SetYTitle("r [cm]");
    h_rz_eion ->SetYTitle("r [cm]");
    h_rz_niel ->SetYTitle("r [cm]");
    h_rz_h20  ->SetYTitle("r [cm]");

    h_rz_tid  ->SetZTitle("TID [Gy]");
    h_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    h_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    h_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");

    TH2D *h_full_rz_tid  = new TH2D("full_rz_tid" ,"full_rz_tid" ,m_report.nBinsz,m_report.zMinFull,m_report.zMaxFull,m_report.nBinsr,m_report.rMinFull,m_report.rMaxFull);
    TH2D *h_full_rz_eion = new TH2D("full_rz_eion","full_rz_eion",m_report.nBinsz,m_report.zMinFull,m_report.zMaxFull,m_report.nBinsr,m_report.rMinFull,m_report.rMaxFull);
    TH2D *h_full_rz_niel = new TH2D("full_rz_niel","full_rz_niel",m_report.nBinsz,m_report.zMinFull,m_report.zMaxFull,m_report.nBinsr,m_report.rMinFull,m_report.rMaxFull);
    TH2D *h_full_rz_h20  = new TH2D("full_rz_h20" ,"full_rz_h20" ,m_report.nBinsz,m_report.zMinFull,m_report.zMaxFull,m_report.nBinsr,m_report.rMinFull,m_report.rMaxFull);

    h_full_rz_tid  ->SetXTitle("|z| [cm]");
    h_full_rz_eion ->SetXTitle("|z| [cm]");
    h_full_rz_niel ->SetXTitle("|z| [cm]");
    h_full_rz_h20  ->SetXTitle("|z| [cm]");

    h_full_rz_tid  ->SetYTitle("r [cm]");
    h_full_rz_eion ->SetYTitle("r [cm]");
    h_full_rz_niel ->SetYTitle("r [cm]");
    h_full_rz_h20  ->SetYTitle("r [cm]");

    h_full_rz_tid  ->SetZTitle("TID [Gy]");
    h_full_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    h_full_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    h_full_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");

    TH3D * h_3d_tid  = new TH3D("h3d_tid" ,"h3d_tid" ,m_report.nBinsz3d,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr3d,m_report.rMinZoom,m_report.rMaxZoom,m_report.nBinsphi3d,m_report.phiMinZoom,m_report.phiMaxZoom);
    TH3D * h_3d_eion = new TH3D("h3d_eion","h3d_eion",m_report.nBinsz3d,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr3d,m_report.rMinZoom,m_report.rMaxZoom,m_report.nBinsphi3d,m_report.phiMinZoom,m_report.phiMaxZoom);
    TH3D * h_3d_niel = new TH3D("h3d_niel","h3d_niel",m_report.nBinsz3d,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr3d,m_report.rMinZoom,m_report.rMaxZoom,m_report.nBinsphi3d,m_report.phiMinZoom,m_report.phiMaxZoom);
    TH3D * h_3d_h20  = new TH3D("h3d_h20" ,"h3d_h20" ,m_report.nBinsz3d,m_report.zMinZoom,m_report.zMaxZoom,m_report.nBinsr3d,m_report.rMinZoom,m_report.rMaxZoom,m_report.nBinsphi3d,m_report.phiMinZoom,m_report.phiMaxZoom);

    h_3d_tid  ->SetXTitle("|z| [cm]");
    h_3d_eion ->SetXTitle("|z| [cm]");
    h_3d_niel ->SetXTitle("|z| [cm]");
    h_3d_h20  ->SetXTitle("|z| [cm]");

    h_3d_tid  ->SetYTitle("r [cm]");
    h_3d_eion ->SetYTitle("r [cm]");
    h_3d_niel ->SetYTitle("r [cm]");
    h_3d_h20  ->SetYTitle("r [cm]");

    h_3d_tid  ->SetZTitle("#phi [#circ]");
    h_3d_eion ->SetZTitle("#phi [#circ]");
    h_3d_niel ->SetZTitle("#phi [#circ]");
    h_3d_h20  ->SetZTitle("#phi [#circ]");

    h_3d_tid  ->SetTitle("TID [Gy]");
    h_3d_eion ->SetTitle("E_{ion}/V [MeV/cm^{3}]");
    h_3d_niel ->SetTitle("NIEL [n_{eq}/cm^{2}]");
    h_3d_h20  ->SetTitle("SEE [h_{>20 MeV}/cm^{2}]");


    // normalize to volume element per bin
    for(int i=0;i<h_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<h_rz_tid->GetNbinsY();j++) { 
	int iBin = h_rz_tid->GetBin(i+1,j+1); 
	int vBin = m_report.nBinsr*i+j;
	double r0=h_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =m_report.m_rz_tid[vBin];
	h_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =m_report.m_rz_eion[vBin];
	h_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =m_report.m_rz_niel[vBin];
	h_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =m_report.m_rz_h20[vBin];
	h_rz_h20->SetBinContent(iBin,val/vol);
      }
    }
    h_rz_tid->Write();
    h_rz_eion->Write();
    h_rz_niel->Write();
    h_rz_h20->Write();

    // normalize to volume element per bin
    for(int i=0;i<h_full_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<h_full_rz_tid->GetNbinsY();j++) { 
	int iBin = h_full_rz_tid->GetBin(i+1,j+1); 
	int vBin = m_report.nBinsr*i+j;
	double r0=h_full_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_full_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_full_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_full_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =m_report.m_full_rz_tid[vBin];
	h_full_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =m_report.m_full_rz_eion[vBin];
	h_full_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =m_report.m_full_rz_niel[vBin];
	h_full_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =m_report.m_full_rz_h20[vBin];
	h_full_rz_h20->SetBinContent(iBin,val/vol);
      }
    }
    h_full_rz_tid->Write();
    h_full_rz_eion->Write();
    h_full_rz_niel->Write();
    h_full_rz_h20->Write();

    // normalize to volume element per bin
    for(int i=0;i<h_3d_tid->GetNbinsX();i++) { /* |z| */
      for(int j=0;j<h_3d_tid->GetNbinsY();j++) { /* r */
	for(int k=0;k<h_3d_tid->GetNbinsZ();k++) { /* phi */
	  int vBin = m_report.nBinsr3d*m_report.nBinsphi3d*i+m_report.nBinsphi3d*j+k;
	  int iBin = h_3d_tid->GetBin(i+1,j+1,k+1); 
	  double phi0=h_3d_tid->GetZaxis()->GetBinLowEdge(k+1);
	  double phi1=h_3d_tid->GetZaxis()->GetBinUpEdge(k+1);
	  double r0=h_3d_tid->GetYaxis()->GetBinLowEdge(j+1);
	  double r1=h_3d_tid->GetYaxis()->GetBinUpEdge(j+1);
	  double z0=h_3d_tid->GetXaxis()->GetBinLowEdge(i+1);
	  double z1=h_3d_tid->GetXaxis()->GetBinUpEdge(i+1); 
	  double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0)*(phi1-phi0)/360.; 
	  double val;
	  // TID
	  val =m_report.m_3d_tid[vBin];
	  h_3d_tid->SetBinContent(iBin,val/vol);
	  // EION
	  val =m_report.m_3d_eion[vBin];
	  h_3d_eion->SetBinContent(iBin,val/vol);
	  // NIEL
	  val =m_report.m_3d_niel[vBin];
	  h_3d_niel->SetBinContent(iBin,val/vol);
	  // SEE
	  val =m_report.m_3d_h20[vBin];
	  h_3d_h20->SetBinContent(iBin,val/vol);
	}
      }
    }
    h_3d_tid->Write();
    h_3d_eion->Write();
    h_3d_niel->Write();
    h_3d_h20->Write();

    f->Close();

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<RadiationMapsMaker>  RadiationMapsMakerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<RadiationMapsMaker>();
  }

} // namespace G4UA 
