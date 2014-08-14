/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/ChargeDepCbMSIDAlignPlots.h"

using namespace MuonTrkPhysMonUtils;

ChargeDepCbMSIDAlignPlots::ChargeDepCbMSIDAlignPlots(PlotBase *pParent, const std::string& sChargeInfo):PlotBase(pParent),m_sChargeInfo(sChargeInfo){}

void ChargeDepCbMSIDAlignPlots::initializePlots(){

  std::string m_sSign;
  if      (m_sChargeInfo == "pos") m_sSign = "q > 0";
  else if (m_sChargeInfo == "neg") m_sSign = "q < 0";

  // <q/pT_MSEXT - q/pT_ID> vs Eta Region
  Book1D(m_QoPt_ms_id_sum, "m_QoPt_ms_id_sum_" + m_sChargeInfo, m_sTrackCollectionName+": (q_{MExt}/p_{T})_{MExt} - (q_{ID}/p_{T})_{ID} [GeV]^{-1}: " + m_sSign + ";[GeV]^{-1}", 100,-.05,.05); //Going to have to find the best binning for this
  Book1D(m_QoPt_ms_id_sum_weights, "m_QoPt_ms_id_sum_weights_" + m_sChargeInfo, m_sTrackCollectionName+": (q_{MExt}/p_{T})_{MExt} - (q_{ID}/p_{T})_{ID} [GeV]^{-1}: " + m_sSign + " : Weights;;[GeV]^{-1}", 8,0.,8.);
  Book1D(m_QoPt_ms_id_sum_entries, "m_QoPt_ms_id_sum_entries_" + m_sChargeInfo, m_sTrackCollectionName+": (q_{MExt}/p_{T})_{MExt} - (q_{ID}/p_{T})_{ID} [GeV]^{-1}: " + m_sSign + " : Entries;;N_{#mu}", 8,0.,8.);
  Book1D(m_QoPt_ms_id_sum_mean, "m_QoPt_ms_id_sum_mean_" + m_sChargeInfo, m_sTrackCollectionName+": <(q_{MExt}/p_{T})_{MExt} - (q_{ID}/p_{T})_{ID}> [GeV]^{-1}: " + m_sSign + ";;[GeV]^{-1}", 8,0.,8.);

  // <1/pT>{q>1} - <1/pT>{q<1} vs eta
  Book1D(m_oPt_sum, "m_oPt_sum_" + m_sChargeInfo, m_sTrackCollectionName+": 1/pT [GeV]^{-1} : " + m_sSign + ";[GeV]^{-1}", 100,.0,.07);
  Book1D(m_oPt_sum_weights, "m_oPt_sum_weights_" + m_sChargeInfo, m_sTrackCollectionName+": 1/p_{T} : " + m_sSign + " [GeV]^{-1}: Weights;;[GeV]^{-1}", 8,0.,8.);
  Book1D(m_oPt_sum_entries, "m_oPt_sum_entries_" + m_sChargeInfo, m_sTrackCollectionName+": 1/p_{T} : " + m_sSign + " : Entries;;N_{#mu}", 8,0.,8.);
  Book1D(m_oPt_sum_mean, "m_oPt_sum_mean_" + m_sChargeInfo, m_sTrackCollectionName+": <1/p_{T}> [GeV]^{-1}: " + m_sSign + " ;;[GeV]^{-1}", 8,0.,8.);

  // q(p_ID - p_MV)/p_ID vs EtaRegion Phi Region
  Book1D(m_rel_p_ms_id_sum, "m_rel_p_ms_id_sum_" + m_sChargeInfo, m_sTrackCollectionName+": (p_{ID} - p_{MExt})/p_{ID} : Z Sample : " + m_sSign, 100,-.50,.50); //Going to have to find the best binning for this
  Book2D(m_rel_p_ms_id_sum_weights, "m_rel_p_ms_id_sum_weights_" + m_sChargeInfo, m_sTrackCollectionName+": (p_{ID} - p_{MExt})/p_{ID} : Z Sample : " + m_sSign + " : Weights", 8,0.,8.,16,1.,17.);
  Book2D(m_rel_p_ms_id_sum_entries, "m_rel_p_ms_id_sum_entries_" + m_sChargeInfo, m_sTrackCollectionName+": (p_{ID} - p_{MExt})/p_{ID} : Z Sample : " + m_sSign + " : Entries;;N_{#mu}", 8,0.,8.,16,1.,17.);
  Book2D(m_rel_p_ms_id_sum_mean, "m_rel_p_ms_id_sum_mean_" + m_sChargeInfo, m_sTrackCollectionName+": <(p_{ID} - p_{MExt})/p_{ID}> : Z Sample : " + m_sSign, 8,0.,8.,16,1.,17.);


}

void ChargeDepCbMSIDAlignPlots::fill(const xAOD::Muon* Muon, const Trk::Perigee* measPerigeeID, const Trk::Perigee* measPerigeeME){
                              
  float pID = fabs(measPerigeeID->parameters()[Trk::qOverP]);
  float qID = measPerigeeID->charge(); //parameters()[Trk::qOverP] < 0) ? -1 : 1;

  float pME = fabs(measPerigeeME->parameters()[Trk::qOverP]);
  float qME = measPerigeeME->charge(); //parameters()[Trk::qOverP] < 0) ? -1 : 1;
                   
  // <q/pT_MSEXT - q/pT_ID> vs Eta Region
  m_QoPt_ms_id_sum_weights->Fill(EtaRegion(Muon->eta()),qME*1000./measPerigeeME->pT()-qID*1000./measPerigeeID->pT());
  m_QoPt_ms_id_sum_entries->Fill(EtaRegion(Muon->eta()));
  m_QoPt_ms_id_sum->Fill(qME*1000./measPerigeeME->pT()-qID*1000./measPerigeeID->pT()); //<-- this will provide us with a sigma(ip_Delta_qop0) for the error calc.
                
  // <1/pT>{q>1} - <1/pT>{q<1} vs eta
  m_oPt_sum_weights->Fill(EtaRegion(Muon->eta()),1000./Muon->pt());
  m_oPt_sum_entries->Fill(EtaRegion(Muon->eta()));
  m_oPt_sum->Fill(1000./Muon->pt()); //<-- this will provide us with a sigma(1000./Muon->pt()) for the error calc.
                
  // (p_ID - p_ME)/p_ID vs EtaRegion v Phi Region
  double ip_Delta_phi0  = measPerigeeME->parameters()[Trk::phi0]  - measPerigeeID->parameters()[Trk::phi0];
  m_rel_p_ms_id_sum_weights->Fill(EtaRegion(Muon->eta()),PhiSector(ip_Delta_phi0/2.+measPerigeeID->parameters()[Trk::phi0]), (pID - pME)/pID );
  m_rel_p_ms_id_sum_entries->Fill(EtaRegion(Muon->eta()),PhiSector(ip_Delta_phi0/2.+measPerigeeID->parameters()[Trk::phi0]));
  m_rel_p_ms_id_sum->Fill(  (pID - pME)/pID  ); //<-- this will provide us with a sigma(ip_Delta_mom0*ip_ID_qop0*qID) for the error calc.
                        
}

void ChargeDepCbMSIDAlignPlots::finalizePlots(){

    LabelLargeEtaRegions(m_QoPt_ms_id_sum_mean->GetXaxis());
    LabelLargeEtaRegions(m_QoPt_ms_id_sum_weights->GetXaxis());
    LabelLargeEtaRegions(m_QoPt_ms_id_sum_entries->GetXaxis());        
        
    m_QoPt_ms_id_sum_mean->Divide(m_QoPt_ms_id_sum_weights,m_QoPt_ms_id_sum_entries);                
    for (int j = 1; j < m_QoPt_ms_id_sum_mean->GetNbinsX()+1; j++) {
      m_QoPt_ms_id_sum_mean->SetBinError(j,m_QoPt_ms_id_sum_entries->GetBinContent(j) ? m_QoPt_ms_id_sum->GetRMS(1)/sqrt(m_QoPt_ms_id_sum_entries->GetBinContent(j)) : 0.0);
    }
    
    LabelLargeEtaRegions(m_rel_p_ms_id_sum_mean->GetXaxis());
    LabelLargeEtaRegions(m_rel_p_ms_id_sum_weights->GetXaxis());
    LabelLargeEtaRegions(m_rel_p_ms_id_sum_entries->GetXaxis());
                
    LabelSectorAxis(m_rel_p_ms_id_sum_mean->GetYaxis());
    LabelSectorAxis(m_rel_p_ms_id_sum_weights->GetYaxis());
    LabelSectorAxis(m_rel_p_ms_id_sum_entries->GetYaxis());
                
    m_rel_p_ms_id_sum_mean->Divide(m_rel_p_ms_id_sum_weights,m_rel_p_ms_id_sum_entries);
    m_rel_p_ms_id_sum_mean->SetBit(TH1::kIsAverage);
        
    for (int j = 1; j < m_rel_p_ms_id_sum_mean->GetNbinsX()+1; j++) {
      for (int k = 1; k < m_rel_p_ms_id_sum_mean->GetNbinsY()+1; k++) {
      	m_rel_p_ms_id_sum_mean->SetBinError(j,k,m_rel_p_ms_id_sum_entries->GetBinContent(j,k) ? m_rel_p_ms_id_sum->GetRMS(1)/sqrt(m_rel_p_ms_id_sum_entries->GetBinContent(j,k)) : 0.0);
      }
    }
    
    LabelLargeEtaRegions(m_oPt_sum_mean->GetXaxis());
    LabelLargeEtaRegions(m_oPt_sum_weights->GetXaxis());
    LabelLargeEtaRegions(m_oPt_sum_entries->GetXaxis());
    m_oPt_sum_mean->Divide(m_oPt_sum_weights,m_oPt_sum_entries);
    for (int j = 1; j < m_oPt_sum_mean->GetNbinsX()+1; j++) {          
      m_oPt_sum_mean->SetBinError(j,m_oPt_sum_entries->GetBinContent(j) ? m_oPt_sum->GetRMS(1)/sqrt(m_oPt_sum_entries->GetBinContent(j)) : 0.0);            
    }
}

