/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIPileupTool.cxx

#include "HIEventUtils/HIPileupTool.h"
#include "xAODForward/ZdcModule.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <PathResolver/PathResolver.h>

#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include <vector>
#include <exception>

namespace HI

{

HIPileupTool::HIPileupTool( const std::string& myname) : AsgTool(myname),
  							m_accept("HIPileup")
  {
  declareProperty("Message", m_msg);
  declareProperty("EtCutAndPurity", m_etCutAndPurity);
  declareProperty("inpFilePath", m_fname);
  declareProperty("Year", m_year);

  m_setup = 0;
}

HIPileupTool::~HIPileupTool() {

   if (m_setup) {
      delete m_hEvents; delete m_hPileUp;
      delete m_hCut; delete m_hEff; delete m_hPurity;
   }
}

StatusCode HIPileupTool::initialize() {

   ATH_MSG_VERBOSE( "Initialising tool " << name() );
  
   m_accept.addCut("pileupVeto","for pileup rejection");

// in-time pileup configuration 2015
if(m_year=="2015"){
   TFile* fIn = TFile::Open(PathResolverFindCalibFile(m_fname).c_str(), "READ");
   if( !fIn ){
      ANA_MSG_ERROR( "Could not find input Pileup calibration file "<< m_fname << ", exiting");
    return StatusCode::FAILURE;
   }

   ATH_MSG_INFO( "Read pileup cuts from "<< m_fname );

   std::string hname;
   hname = "hEvents"; if (!(fIn->GetListOfKeys()->Contains(hname.c_str()))) {
      fIn->Close(); throw std::invalid_argument(("Expected histogram \"" + hname + "\" not found in \"" + m_fname + "\"").c_str());}
   hname = "hPileUp"; if (!(fIn->GetListOfKeys()->Contains(hname.c_str()))) {
      fIn->Close(); throw std::invalid_argument(("Expected histogram \"" + hname + "\" not found in \"" + m_fname + "\"").c_str());}
   hname = "hCut";    if (!(fIn->GetListOfKeys()->Contains(hname.c_str()))) {
      fIn->Close(); throw std::invalid_argument(("Expected histogram \"" + hname + "\" not found in \"" + m_fname + "\"").c_str());}
   hname = "hEff";    if (!(fIn->GetListOfKeys()->Contains(hname.c_str()))) {
      fIn->Close(); throw std::invalid_argument(("Expected histogram \"" + hname + "\" not found in \"" + m_fname + "\"").c_str());}
   hname = "hPurity"; if (!(fIn->GetListOfKeys()->Contains(hname.c_str()))) {
      fIn->Close(); throw std::invalid_argument(("Expected histogram \"" + hname + "\" not found in \"" + m_fname + "\"").c_str());}
   
   m_hEvents = (TH2D*)((TH2D*)fIn->Get("hEvents"))->Clone("hEvents_HIPileupTool"); m_hEvents->SetDirectory(0);
   m_hPileUp = (TH2D*)((TH2D*)fIn->Get("hPileUp"))->Clone("hPileUp_HIPileupTool"); m_hPileUp->SetDirectory(0);
   m_hCut    = (TH1D*)((TH1D*)fIn->Get("hCut"))->Clone("hCut_HIPileupTool");       m_hCut->SetDirectory(0);
   m_hEff    = (TH1D*)((TH1D*)fIn->Get("hEff"))->Clone("hEff_HIPileupTool");       m_hEff->SetDirectory(0);
   m_hPurity = (TH1D*)((TH1D*)fIn->Get("hPurity"))->Clone("hPurity_HIPileupTool"); m_hPurity->SetDirectory(0);
   m_setup = 1;

   fIn->Close();

   m_npeak = 2510;

   m_etThreshold = 6.0;
   m_purityCut   = -1.0;
   if (!m_etCutAndPurity.empty()) {
      if (m_etCutAndPurity.size()<2) ATH_MSG_WARNING("Cuts vector should have two elements. Will use default settings");
      else {
         if (m_etCutAndPurity.at(1)<0 || m_etCutAndPurity.at(1)>1) ATH_MSG_WARNING("Purity cut should be between 0 and 1. Will use default settings");
         else {
            m_etThreshold = m_etCutAndPurity.at(0);
            m_purityCut   = m_etCutAndPurity.at(1);
            if (fabs(m_purityCut-1)<1e-6) m_purityCut -= 1e-3; 
         }
      }
   }

   if (m_purityCut<0) {
       ATH_MSG_INFO("ET threshold for purity cut = NONE");
       ATH_MSG_INFO("Purity cut = DEFAULT");
   }
   else {
       ATH_MSG_INFO("ET threshold for purity cut = " << m_etThreshold << " TeV");
       ATH_MSG_INFO("Purity cut = " << m_purityCut );
   }

   for (int ibx=0;ibx<m_hPileUp->GetNbinsX();ibx++) {
       if (m_hPileUp->GetXaxis()->GetBinCenter(ibx+1) < m_etThreshold) continue;
       int NY = m_hPileUp->GetNbinsY();
       if (m_hPileUp->Integral(ibx+1,ibx+1,1,NY) == 0) {m_hCut->SetBinContent(ibx+1,0); continue;}
       if (m_hPileUp->GetXaxis()->GetBinCenter(ibx+1)>5.0 && fabs(m_purityCut-1)<1e-6) {m_hCut->SetBinContent(ibx+1,0); continue;}
       for (int iby=0;iby<NY;iby++) {
           if (m_hPileUp->Integral(ibx+1,ibx+1,NY-iby,NY)/m_hPileUp->Integral(ibx+1,ibx+1,1,NY) < m_purityCut) continue;
           m_hCut->SetBinContent(ibx+1,m_hPileUp->GetYaxis()->GetBinCenter(NY-iby)); break;
       }
   }

   for (int ibx=0;ibx<m_hEvents->GetNbinsX();ibx++) {
      if (m_hEvents->GetXaxis()->GetBinCenter(ibx+1) < m_etThreshold) {m_hEff->SetBinContent(ibx+1,1-m_hEff->GetBinContent(ibx+1)); continue;}
      double cut = m_hEvents->Integral(ibx+1,ibx+1,m_hEvents->GetYaxis()->FindBin(m_hCut->GetBinContent(ibx+1)), m_hEvents->GetNbinsY());
      double all = m_hEvents->Integral(ibx+1,ibx+1,1,m_hEvents->GetYaxis()->GetNbins()) - m_hPileUp->Integral(ibx+1,ibx+1,1,m_hPileUp->GetYaxis()->GetNbins());
      double pu_cut = m_hPileUp->Integral(ibx+1,ibx+1,m_hPileUp->GetYaxis()->FindBin(m_hCut->GetBinContent(ibx+1)), m_hPileUp->GetNbinsY());
      double pu_all = m_hPileUp->Integral(ibx+1,ibx+1,1,m_hPileUp->GetYaxis()->GetNbins());
      double eff = 1 - (cut - pu_cut)/all; if ((cut - pu_cut) < 0) eff=1;
      double pty = pu_cut/pu_all;
      if (all!=0) m_hEff->SetBinContent(ibx+1,eff);
      if (pu_all!=0) m_hPurity->SetBinContent(ibx+1,pty);
   }
}// 2015 in-time pileup configuration

 // Configuration in-time pileup 2018
if(m_year=="2018"){
   std::unique_ptr< TFile > f_itp_In(TFile::Open( PathResolverFindCalibFile(m_itp_fname).c_str(), "READ" ) );

	if( !f_itp_In ){
		ANA_MSG_ERROR( "Could not find input In-time Pileup calibration file "<< m_itp_fname << ", exiting");
		 return StatusCode::FAILURE;
	}

   ATH_MSG_INFO( "Read In-time pileup cuts from "<< m_itp_fname );
   
   m_hCut = (TH1D*)((TH1D*)f_itp_In->Get("h_cuts"))->Clone("hCut_HIPileupTool"); m_hCut->SetDirectory(0);

/// Configuration Out-of-time pileup 2018
   TFile* f_oop_In = TFile::Open(PathResolverFindCalibFile(m_oop_fname).c_str(), "READ");
   if( !f_oop_In ){
   ANA_MSG_ERROR( "Could not find input Out-of-time Pileup calibration file "<< m_oop_fname << ", exiting");
   	return StatusCode::FAILURE;
   }

   ATH_MSG_INFO( "Read Out-of-time pileup cuts from "<< m_oop_fname );

   m_oop_hMean = (TH1D*)((TH1D*)f_oop_In->Get("hMeanTotal"))->Clone("hMeanTotal_HIPileTool"); m_oop_hMean->SetDirectory(0);
   m_oop_hSigma = (TH1D*)((TH1D*)f_oop_In->Get("hSigmaTotal"))->Clone("hSigmaTotal_HIPileTool"); m_oop_hSigma->SetDirectory(0);
   f_oop_In->Close();
}
////
   return StatusCode::SUCCESS;
}

bool HIPileupTool::is_pileup(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const {

   bool kPileup = false;
   double FCal_Et = get_et(evShCont);
   if(m_year=="2015"){
   	double nNeutrons;
   	nNeutrons = get_nNeutrons(ZdcCont);
   	if (nNeutrons > m_hCut->GetBinContent(m_hCut->FindBin(FCal_Et))) kPileup = true; // 2015 cut based in Number of neutrons
   }
   else if(m_year=="2018"){
   	double ZDC_energy = get_ZDC_E(ZdcCont); 
	if (ZDC_energy > m_hCut->GetBinContent(m_hCut->FindBin(FCal_Et))) kPileup = true; // 2018 cut based in ZDC (A+C) energy 
   }

   else{
	ATH_MSG_INFO("Warning:In-time pileup rejection not calibrated for year: " << m_year);
	kPileup = false;
   }

   return kPileup;
}

bool HIPileupTool::is_Outpileup(const xAOD::HIEventShapeContainer& evShCont, const int nTrack) const {
   if(m_year=="2018"){
   	if (nTrack > 3000) // The selection is only for [0, 3000]
        	return 0;

   	float Fcal_Et = 0.0;
   	float Tot_Et = 0.0;
   	float oop_Et = 0.0;
   	Fcal_Et = evShCont.at(5)->et()*1e-6;
   	Tot_Et = evShCont.at(0)->et()*1e-6;
   	oop_Et = Tot_Et - Fcal_Et;// Barrel + Endcap calo

   	int nBin{m_oop_hMean->GetXaxis()->FindFixBin(nTrack)};
   	double mean{m_oop_hMean->GetBinContent(nBin)};
   	double sigma{m_oop_hSigma->GetBinContent(nBin)};

   	switch (m_nside){

   		case 1: if (oop_Et - mean > -4 * sigma) return 0;
        		break;
    		case 2: if (abs(oop_Et - mean) < 4 * sigma) return 0;
        		break;
    		default: if (oop_Et - mean > -4 * sigma) return 0;
	}
   }
   else{
        ATH_MSG_INFO("Warning:Out-of-time pileup rejection not calibrated for year: " << m_year);
        return 0;
   }  
      
   return 1;
}

/*bool HIPileupTool::is_pileup2(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const {

    bool kPileup = false;
    double FCal_Et = get_et(evShCont);
    double nNeutrons;
    nNeutrons = get_nNeutrons(ZdcE);
    if (nNeutrons > m_hCut->GetBinContent(m_hCut->FindBin(FCal_Et))) kPileup = true;

    return kPileup;
} */

double HIPileupTool::get_efficiency(const xAOD::HIEventShapeContainer& evShCont, double& FCal_Et) {

   FCal_Et = get_et(evShCont);
   return m_hEff->GetBinContent(m_hEff->FindBin(FCal_Et));
}

double HIPileupTool::get_purity(const xAOD::HIEventShapeContainer& evShCont, double& FCal_Et) {

   FCal_Et = get_et(evShCont);
   return m_hPurity->GetBinContent(m_hPurity->FindBin(FCal_Et));
}

/*
double HIPileupTool::get_et(const xAOD::HIEventShapeContainer& evShCont) const{

   double Fcal_Et=0;
   for(const auto *ptrEvtShp : evShCont) {
      if(fabs((ptrEvtShp->etaMin()+ptrEvtShp->etaMax())/2.0)<3.2) continue;
      Fcal_Et += ptrEvtShp->et()/1000000;
   }
   return Fcal_Et;
}
*/

double HIPileupTool::get_et(const xAOD::HIEventShapeContainer& evShCont) const{
   double Fcal_Et=0;
   Fcal_Et = evShCont.at(5)->et()*1e-6;
   return Fcal_Et;
}

double HIPileupTool::get_nNeutrons(const xAOD::ZdcModuleContainer& ZdcCont) const{ 

   bool isCalib=1;
    double ZdcE=0;
   for (const xAOD::ZdcModule* zdcModule : ZdcCont) {
      if (zdcModule->type()!=0) continue;

      if (!(zdcModule->isAvailable<float>("CalibEnergy"))) {isCalib = 0; continue;} 
      float modE = zdcModule->auxdecor<float>("CalibEnergy");
      ZdcE += modE;
   }
   if (!isCalib) throw std::invalid_argument("ZDC Module not Calibrated");

   return ZdcE/m_npeak;
}

double HIPileupTool::get_ZDC_E(const xAOD::ZdcModuleContainer& ZdcCont) const{

    double ZdcE=0;
    bool isCalib=1;
   for (const xAOD::ZdcModule* zdcModule : ZdcCont) {
      if (!(zdcModule->isAvailable<float>("CalibEnergy"))) {isCalib = 0; continue;}
      float modE = zdcModule->auxdataConst<float>("CalibEnergy")*1e-3;
      ZdcE += modE;
   }
   if (!isCalib) throw std::invalid_argument("ZDC Module not Calibrated");
   return ZdcE;
}

/*double HIPileupTool::get_nNeutrons(double ZdcE) { 

    return ZdcE/m_npeak;
}*/


void HIPileupTool::print() const {
    AsgTool::print();
    if (m_purityCut<0) {
        ATH_MSG_INFO("ET threshold for purity cut = NONE");
        ATH_MSG_INFO("Purity cut = DEFAULT");
    }
   else {
      ATH_MSG_INFO("ET threshold for purity cut = " << m_etThreshold << " TeV");
      ATH_MSG_INFO("Purity cut = " << m_purityCut );
   }
}

void HIPileupTool::write(TFile* fOut) const {

    fOut->cd(0);
    m_hEvents->Write();
    m_hPileUp->Write();
    m_hCut->Write();
    m_hEff->Write();
    m_hPurity->Write();
}

const Root::TAccept& HIPileupTool::getTAccept(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const
{
m_accept.clear();
bool pileup_dec = !is_pileup(evShCont, ZdcCont);
m_accept.setCutResult("pileupVeto", pileup_dec);
return m_accept;
}

} // HI


