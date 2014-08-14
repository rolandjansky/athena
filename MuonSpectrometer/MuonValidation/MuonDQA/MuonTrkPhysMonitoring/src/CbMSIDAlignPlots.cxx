/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/CbMSIDAlignPlots.h"
#include "TMath.h"

CbMSIDAlignPlots::CbMSIDAlignPlots(PlotBase *pParent):PlotBase(pParent),
m_oPosCbMSIDAlignPlots(this, "pos"),
m_oNegCbMSIDAlignPlots(this, "neg")
{;}

using namespace MuonTrkPhysMonUtils;

void CbMSIDAlignPlots::initializePlots(){
  //======================================================================
  // <1/pT>{q>1} - <1/pT>{q<1} vs eta
  Book1D(m_oPt_Diff, "m_oPt_Diff", m_sTrackCollectionName+": (<1/p_{T}> : q>0) - (<1/p_{T}> : q<0)[GeV]^{-1};;[GeV]^{-1}", 8,0.,8.);

  // <1/pT_MSEXT - 1/pT_ID> vs eta
  Book1D(m_oPt_ms_id_sum, "m_oPt_ms_id_sum", m_sTrackCollectionName+": (1/p_{T})_{MExt} - (1/p_{T})_{ID} [GeV]^{-1};[GeV]^{-1}", 100,-0.04,0.04);
  Book1D(m_oPt_ms_id_sum_weights, "m_oPt_ms_id_sum_weights", m_sTrackCollectionName+": (1/p_{T})_{MExt} - (1/p_{T})_{ID} [GeV]^{-1} : Weights;;[GeV]^{-1}", 8,0.,8.);
  Book1D(m_oPt_ms_id_sum_entries, "m_oPt_ms_id_sum_entries", m_sTrackCollectionName+": (1/p_{T})_{MExt} - (1/p_{T})_{ID} [GeV]^{-1} : Entries;;N_{#mu}", 8,0.,8.);
  Book1D(m_oPt_ms_id_sum_mean, "m_oPt_ms_id_sum_mean", m_sTrackCollectionName+": <(1/p_{T})_{MExt} - (1/p_{T})_{ID}> [GeV]^{-1};;[GeV]^{-1}", 8,0.,8.);

  // <chi2_cb trk/ndof> vs muon eta
  Book1D(m_chi2ndof_CB, "m_chi2ndof_CB", m_sTrackCollectionName+": #Chi^{2} Prob.", 100,0.0,1.0);
  Book1D(m_chi2ndof_CB_weights, "m_chi2ndof_CB_weights", m_sTrackCollectionName+": #Chi^{2} Prob.", 8,0.,8.);
  Book1D(m_chi2ndof_CB_entries, "m_chi2ndof_CB_entries", m_sTrackCollectionName+": Entries", 8,0.,8.);
  Book1D(m_chi2ndof_CB_mean, "m_chi2ndof_CB_mean", m_sTrackCollectionName+": <#Chi^{2} Prob>", 8,0.,8.);
    
  // <DeltaZ0> vs muon eta region
  Book1D(m_deltaZ0_CB_Eta_AllSectors, "m_deltaZ0_CB_Eta_AllSectors", m_sTrackCollectionName+": <#Deltaz_{0}>^{(ME-ID)};#eta", 30,-3.0,3.0);
  Book2D(m_deltaZ0_CB_Eta_Phi, "m_deltaZ0_CB_Eta_Phi", m_sTrackCollectionName+": <#Deltaz_{0}>^{(ME-ID)};#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_deltaZ0_CB_Eta_Phi_Width, "m_deltaZ0_CB_Eta_Phi_Width", m_sTrackCollectionName+": #sigma(#Deltaz_{0}^{(ME-ID)});#eta", 30,-3.0,3.0,16,1.,17.);
    
  Book2D(m_deltaZ0_CB, "m_deltaZ0_CB", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)};[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S1, "m_deltaZ0_CB_S01", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 01;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S2, "m_deltaZ0_CB_S02", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 02;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S3, "m_deltaZ0_CB_S03", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 03;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S4, "m_deltaZ0_CB_S04", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 04;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S5, "m_deltaZ0_CB_S05", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 05;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S6, "m_deltaZ0_CB_S06", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 06;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S7, "m_deltaZ0_CB_S07", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 07;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S8, "m_deltaZ0_CB_S08", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 08;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S9, "m_deltaZ0_CB_S09", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 09;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S10, "m_deltaZ0_CB_S10", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 10;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S11, "m_deltaZ0_CB_S11", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 11;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S12, "m_deltaZ0_CB_S12", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 12;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S13, "m_deltaZ0_CB_S13", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 13;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S14, "m_deltaZ0_CB_S14", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 14;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S15, "m_deltaZ0_CB_S15", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 15;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);
  Book2D(m_deltaZ0_CB_S16, "m_deltaZ0_CB_S16", m_sTrackCollectionName+": #Deltaz_{0}^{(ME-ID)} : Sector 16;[mm];#eta", 200, -100.0, 100.0, 30, -3.0, 3.0);

}

void CbMSIDAlignPlots::fill(const xAOD::Muon* Muon, const Trk::Perigee* measPerigeeID, const Trk::Perigee* measPerigeeME){

  // <1/pT_MSEXT - 1/pT_ID> vs eta
  m_oPt_ms_id_sum_weights->Fill(EtaRegion(Muon->eta()),1000./measPerigeeME->pT()-1000./measPerigeeID->pT()); //Convert to GeV
  m_oPt_ms_id_sum_entries->Fill(EtaRegion(Muon->eta()));
  m_oPt_ms_id_sum->Fill(1000./measPerigeeME->pT()-1000./measPerigeeID->pT()); //<-- this will provide us with a sigma(1./measPerigeeID->pT()-1./measPerigeeME->pT())) for the error calc.
        
  float qID = measPerigeeID->charge();
  if(qID > 0) m_oPosCbMSIDAlignPlots.fill(Muon, measPerigeeID,measPerigeeME);
  else if(qID < 0) m_oNegCbMSIDAlignPlots.fill(Muon, measPerigeeID,measPerigeeME);
                  
  double ip_Delta_z0    = measPerigeeME->parameters()[Trk::z0]    - measPerigeeID->parameters()[Trk::z0];
  float phiSector = PhiSector(measPerigeeME->parameters()[Trk::phi0]); 
  if(phiSector  < 17.) m_deltaZ0_CB->Fill(ip_Delta_z0,Muon->eta());
  if(phiSector  < 2. ) m_deltaZ0_CB_S1->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 3. ) m_deltaZ0_CB_S2->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 4. ) m_deltaZ0_CB_S3->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 5. ) m_deltaZ0_CB_S4->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 6. ) m_deltaZ0_CB_S5->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 7. ) m_deltaZ0_CB_S6->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 8. ) m_deltaZ0_CB_S7->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 9. ) m_deltaZ0_CB_S8->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 10.) m_deltaZ0_CB_S9->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 11.) m_deltaZ0_CB_S10->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 12.) m_deltaZ0_CB_S11->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 13.) m_deltaZ0_CB_S12->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 14.) m_deltaZ0_CB_S13->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 15.) m_deltaZ0_CB_S14->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 16.) m_deltaZ0_CB_S15->Fill(ip_Delta_z0,Muon->eta());
  else if(phiSector < 17.) m_deltaZ0_CB_S16->Fill(ip_Delta_z0,Muon->eta());
  
  float msInnerMatchChi2 = 0;
  float msInnerMatchDOF  = 0;
  if (Muon->parameter(msInnerMatchChi2, xAOD::Muon::msInnerMatchChi2) &&  Muon->parameter(msInnerMatchDOF, xAOD::Muon::msInnerMatchDOF)){
    double chiSqProb = TMath::Prob( msInnerMatchChi2, msInnerMatchDOF );
    if (chiSqProb > 0) {
      m_chi2ndof_CB->Fill(chiSqProb);
      m_chi2ndof_CB_weights->Fill(EtaRegion(Muon->eta()),chiSqProb);
      m_chi2ndof_CB_entries->Fill(EtaRegion(Muon->eta()));
    }
  }
}

void CbMSIDAlignPlots::finalizePlots(){
  
  LabelLargeEtaRegions(m_oPt_ms_id_sum_entries->GetXaxis());
  LabelLargeEtaRegions(m_oPt_ms_id_sum_weights->GetXaxis());
  LabelLargeEtaRegions(m_oPt_ms_id_sum_mean->GetXaxis());

  m_oPt_ms_id_sum_mean->Divide(m_oPt_ms_id_sum_weights,m_oPt_ms_id_sum_entries);
  for (int j = 1; j < m_oPt_ms_id_sum_mean->GetNbinsX()+1; ++j) {
    m_oPt_ms_id_sum_mean->SetBinError(j,m_oPt_ms_id_sum_entries->GetBinContent(j) ? m_oPt_ms_id_sum->GetRMS(1)/sqrt(m_oPt_ms_id_sum_entries->GetBinContent(j)) : 0.0);
  }

  LabelSectorAxis(m_deltaZ0_CB_Eta_Phi->GetYaxis());
  LabelSectorAxis(m_deltaZ0_CB_Eta_Phi_Width->GetYaxis());
            
  LabelLargeEtaRegions(m_chi2ndof_CB_weights->GetXaxis());
  LabelLargeEtaRegions(m_chi2ndof_CB_entries->GetXaxis());
  LabelLargeEtaRegions(m_chi2ndof_CB_mean->GetXaxis());
        
  m_chi2ndof_CB_mean->Divide(m_chi2ndof_CB_weights,m_chi2ndof_CB_entries);
        
  LabelLargeEtaRegions(m_oPt_Diff->GetXaxis());
         
  for (int j = 1; j < m_oPt_Diff->GetNbinsX()+1; ++j) {          
          
    //Compute Diff
    m_oPt_Diff->SetBinContent(j, m_oPosCbMSIDAlignPlots.m_oPt_sum_mean->GetBinContent(j) - m_oNegCbMSIDAlignPlots.m_oPt_sum_mean->GetBinContent(j));
          
    //Quadrature Error Addition
    m_oPt_Diff->SetBinError(j,
			  sqrt(pow(m_oPosCbMSIDAlignPlots.m_oPt_sum_mean->GetBinError(j),2)+
			       pow(m_oNegCbMSIDAlignPlots.m_oPt_sum_mean->GetBinError(j),2)));
  }
}

