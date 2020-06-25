/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile MuonTruthMon.cxx
 *   
 *    authors: Laurynas Mince (laurynas.mince@cern.ch)
 *       date: 25.02.2020
 */

#include "TROOT.h"
#include "TH1F.h"
#include "TMath.h"

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

StatusCode HLTMuonMonTool::initMuonTruthHists()
{
    ATH_CHECK(m_muonTruthParticlesKey.initialize());
    return StatusCode::SUCCESS;
}//initMuonTruthHists

StatusCode HLTMuonMonTool::bookMuonTruthHists()
{
    // Truth muon histograms, i.e. the denominator in efficiency
    addHistogram( new TH1F("EFMuon_effi_toTruth_pt_denom", "Truth muon p_{T}; p_{T}[GeV/c]; Entries", 30, 0.0, 200.0), m_histdirtruthmuon );

    //single chain monitoring
    std::vector<std::string> triggerlist;
    triggerlist.insert(triggerlist.end(), m_chainsGeneric.begin(), m_chainsGeneric.end());
    triggerlist.insert(triggerlist.end(), m_chainsEFiso.begin(), m_chainsEFiso.end());
    triggerlist.insert(triggerlist.end(), m_chainsMSonly.begin(), m_chainsMSonly.end());
    triggerlist.insert(triggerlist.end(), m_chainsLowpt.begin(), m_chainsLowpt.end());
    for (std::string trig : triggerlist){
      // Truth muon eta and phi histograms with pT cuts per trigger chain
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_eta_denom_%s",trig.c_str()), Form("Truth muon %s #eta; #eta; Entries",trig.c_str()), 50, -2.7, 2.7), m_histdirtruthmuon );
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_phi_denom_%s",trig.c_str()), Form("Truth muon %s #phi; #phi; Entries",trig.c_str()), 50, -3.14, 3.14), m_histdirtruthmuon );
    
      // truth-matched trigger muon histograms, i.e. the numerator in efficiency
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_pt_numer_%s", trig.c_str()),    Form("Truth-matched EF Muon %s p_{T}; p_{T}[GeV/c]; Entries", trig.c_str()),    30, 0.0, 200.0), m_histdirtruthmuon );
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_eta_numer_%s",trig.c_str()),    Form("Truth-matched EF Muon %s #eta; #eta; Entries", trig.c_str()),    50, -2.7, 2.7), m_histdirtruthmuon );
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_phi_numer_%s",trig.c_str()),    Form("Truth-matched EF Muon %s #phi; #phi [rad]; Entries", trig.c_str()),   50, -3.14,3.14), m_histdirtruthmuon );

      // Truth-based efficiency histograms
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_pt_%s", trig.c_str()), Form("Truth-based EF Muon %s Efficiency pt (GeV/c); p_{T}[GeV/c]; Efficiency", trig.c_str()), 30, 0.0, 200.0 ), m_histdirtruthmuon );
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_eta_%s", trig.c_str()), Form("Truth-based EF Muon %s Efficiency #eta; #eta; Efficiency", trig.c_str()), 50, -2.7, 2.7 ), m_histdirtruthmuon );
      addHistogram( new TH1F(Form("EFMuon_effi_toTruth_phi_%s", trig.c_str()), Form("Truth-based EF Muon %s Efficiency #phi (rad); #phi[rad]; Efficiency", trig.c_str()), 50, -3.14, 3.14 ), m_histdirtruthmuon );
    }

    return StatusCode::SUCCESS;
}//bookMuonTruthHists

StatusCode HLTMuonMonTool::fillMuonTruthHists()
{
  const double DR_MATCHED = 0.1;

  //single chain monitoring
  std::vector<std::string> triggerlist;
  triggerlist.insert(triggerlist.end(), m_chainsGeneric.begin(), m_chainsGeneric.end());
  triggerlist.insert(triggerlist.end(), m_chainsEFiso.begin(), m_chainsEFiso.end());
  triggerlist.insert(triggerlist.end(), m_chainsMSonly.begin(), m_chainsMSonly.end());
  triggerlist.insert(triggerlist.end(), m_chainsLowpt.begin(), m_chainsLowpt.end());

  SG::ReadHandle<xAOD::TruthParticleContainer> truthMuons(m_muonTruthParticlesKey);
  if (truthMuons.isValid()) {
    for (const auto truthMu : *truthMuons)  {
      double pt_true = truthMu->pt()/1000.0;
      double eta_true = truthMu->eta();
      double phi_true = truthMu->phi();
      // Fill the truth histograms
      hist("EFMuon_effi_toTruth_pt_denom", m_histdirtruthmuon)->Fill(pt_true);
      // Match a trigger muon with the truth muon for each trigger chain
      for (std::string trig : triggerlist){
        ATH_MSG_DEBUG("Retrieving feature container for " << trig);
        // Names of muon trigger chains indicate the pT cuts like so "_mu50" for a 50 GeV cut
        // Find the index of "_mu" in the string of a muon trigger chain name and move the index by
        // 3 places to the right to reach a pT value
        std::size_t index_i = trig.find("_mu")+3;
        std::size_t index_f = trig.find("_", index_i+1);
        double pT_cut = 0.0;
        if (index_f < trig.length()) {
          pT_cut = std::stod(trig.substr(index_i, index_f - index_i));
        }
        else {
          pT_cut = std::stod(trig.substr(index_i, trig.length()-1));
        }
        // pT cut for eta/ phi distributions 1 GeV above the trigger pT cut
        if (pT_cut + 1.0 > pt_true)
          continue;

        // Fill the truth muon eta and phi histograms
        hist(Form( "EFMuon_effi_toTruth_eta_denom_%s",trig.c_str()), m_histdirtruthmuon)->Fill(eta_true);
        hist(Form( "EFMuon_effi_toTruth_phi_denom_%s",trig.c_str()), m_histdirtruthmuon)->Fill(phi_true);

        double dR = getTruthToEFdRmin(trig, eta_true, phi_true);
        if ( dR <= DR_MATCHED) {
          hist(Form( "EFMuon_effi_toTruth_pt_numer_%s",trig.c_str()),  m_histdirtruthmuon)->Fill( pt_true );
          hist(Form( "EFMuon_effi_toTruth_eta_numer_%s",trig.c_str()), m_histdirtruthmuon)->Fill( eta_true );
          hist(Form( "EFMuon_effi_toTruth_phi_numer_%s",trig.c_str()), m_histdirtruthmuon)->Fill( phi_true );
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}//fillMuonTruthHists

double HLTMuonMonTool::getTruthToEFdRmin(const std::string trig, double eta_true, double phi_true)
{
  double dR_min = 1000.0;
  if (getTDT()->getNavigationFormat() == "TriggerElement") { // run 2 access
    ATH_MSG_DEBUG("Run 2 access to EF " << trig.c_str());
    const Trig::FeatureContainer fc = getTDT()->features( trig, TrigDefs::alsoDeactivateTEs);
    const std::vector< Trig::Feature<xAOD::MuonContainer> > fEFs = fc.get<xAOD::MuonContainer>();
    for(const Trig::Feature<xAOD::MuonContainer> &fEF : fEFs){
      const xAOD::MuonContainer *cont = fEF.cptr();
      for( const xAOD::Muon* ef : *cont ){
        const HLT::TriggerElement *efTE = fEF.te();
        if(efTE->getActiveState()){//pass
          double dR = calc_dR(ef->eta(), ef->phi(), eta_true, phi_true);
          if (dR < dR_min)
            dR_min = dR;
        }
      }
    }
  }//run 2 access
    else { // run 3 access
      ATH_MSG_DEBUG("Run 3 access to EF " << trig.c_str() << " = " << getTDT()->isPassed(trig));
      const std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > fc = getTDT()->features<xAOD::MuonContainer>(trig);
      ATH_MSG_DEBUG("N(EF muon LinkInfo) for chain " << trig << " = " << fc.size() << " passed = " << getTDT()->isPassed(trig));
      for(auto muonLinkInfo : fc) {
        ATH_CHECK( muonLinkInfo.isValid(), -1 );
        ElementLink<xAOD::MuonContainer> muonLink = muonLinkInfo.link;
        ATH_CHECK( muonLink.isValid(), 1 );
        double dR = calc_dR((*muonLink)->eta(), (*muonLink)->phi(), eta_true, phi_true);
        if (dR < dR_min)
          dR_min = dR;
      }
    }// run 3 access

  return dR_min;
}//getTruthToTruthToEFdRmin

StatusCode HLTMuonMonTool::procMuonTruthHists()
{
  if( endOfRunFlag() ){
    ATH_MSG_DEBUG("procMuonTruthHists");

    //single chain monitoring
    std::vector<std::string> triggerlist;
    triggerlist.insert(triggerlist.end(), m_chainsGeneric.begin(), m_chainsGeneric.end());
    triggerlist.insert(triggerlist.end(), m_chainsEFiso.begin(), m_chainsEFiso.end());
    triggerlist.insert(triggerlist.end(), m_chainsMSonly.begin(), m_chainsMSonly.end());
    triggerlist.insert(triggerlist.end(), m_chainsLowpt.begin(), m_chainsLowpt.end());

    // Fill the truth-based efficiency histograms
    for(std::string trig : triggerlist){
      hist(Form("EFMuon_effi_toTruth_pt_%s", trig.c_str()), m_histdirtruthmuon)->Sumw2();
      hist(Form("EFMuon_effi_toTruth_pt_%s", trig.c_str()), m_histdirtruthmuon)->Divide(hist(Form("EFMuon_effi_toTruth_pt_numer_%s", trig.c_str()), m_histdirtruthmuon), hist("EFMuon_effi_toTruth_pt_denom", m_histdirtruthmuon), 1, 1, "B");

      hist(Form("EFMuon_effi_toTruth_eta_%s", trig.c_str()), m_histdirtruthmuon)->Sumw2();
      hist(Form("EFMuon_effi_toTruth_eta_%s", trig.c_str()), m_histdirtruthmuon)->Divide(hist(Form("EFMuon_effi_toTruth_eta_numer_%s", trig.c_str()), m_histdirtruthmuon), hist(Form("EFMuon_effi_toTruth_eta_denom_%s",trig.c_str()), m_histdirtruthmuon), 1, 1, "B");

      hist(Form("EFMuon_effi_toTruth_phi_%s", trig.c_str()), m_histdirtruthmuon)->Sumw2();
      hist(Form("EFMuon_effi_toTruth_phi_%s", trig.c_str()), m_histdirtruthmuon)->Divide(hist(Form("EFMuon_effi_toTruth_phi_numer_%s", trig.c_str()), m_histdirtruthmuon), hist(Form("EFMuon_effi_toTruth_phi_denom_%s",trig.c_str()), m_histdirtruthmuon), 1, 1, "B");
    }
  }
  return StatusCode::SUCCESS;
}//procMuonTruthHists