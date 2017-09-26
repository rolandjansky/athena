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

   const char* fullInpFilePath;
   if (!m_fname.empty()) fullInpFilePath = gSystem->ExpandPathName (m_fname.c_str());
   else fullInpFilePath = gSystem->ExpandPathName ("$ROOTCOREBIN/data/HIEventUtils/HIRun2PileUp_PbPb5p02_v2.root");
   if (gSystem->AccessPathName(fullInpFilePath)) throw std::invalid_argument(("File not found at \"" + (std::string)fullInpFilePath + "\"").c_str());
   
   ATH_MSG_INFO(("Read pileup cuts from " + (std::string)fullInpFilePath).c_str());
   TFile* fIn = new TFile(fullInpFilePath);

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

   return StatusCode::SUCCESS;
}

bool HIPileupTool::is_pileup(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const {

   bool kPileup = false;
   double FCal_Et = get_et(evShCont);
   double nNeutrons;
   nNeutrons = get_nNeutrons(ZdcCont);
   if (nNeutrons > m_hCut->GetBinContent(m_hCut->FindBin(FCal_Et))) kPileup = true;

   return kPileup;
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
   for (const auto *zdcModule : ZdcCont) {
      if (zdcModule->type()!=0) continue;

      if (!(zdcModule->isAvailable<float>("CalibEnergy"))) {isCalib = 0; continue;} 
      float modE = zdcModule->auxdecor<float>("CalibEnergy");
      ZdcE += modE;
   }
   if (!isCalib) throw std::invalid_argument("ZDC Module not Calibrated");

   return ZdcE/m_npeak;
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


