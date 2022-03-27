/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "JetCalibTools/JetCalibUtils.h"
#include "PathResolver/PathResolver.h"

InsituDataCorrection::InsituDataCorrection()
  : JetCalibrationStep::JetCalibrationStep(),
    m_config(nullptr),
    m_jetAlgo(""),
    m_calibAreaTag(""),
    m_dev(false),
    m_insituCorr(nullptr),
    m_insituCorr_JMS(nullptr),
    m_insituCorr_ResidualMCbased(nullptr),
    m_firstRun(0),
    m_lastRun(0)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, bool dev, unsigned int firstRun, unsigned int lastRun)
  : JetCalibrationStep::JetCalibrationStep(name.c_str()),
    m_config(config),
    m_jetAlgo(jetAlgo),
    m_calibAreaTag(calibAreaTag),
    m_dev(dev),
    m_insituCorr(nullptr),
    m_insituCorr_JMS(nullptr),
    m_insituCorr_ResidualMCbased(nullptr),
    m_firstRun(firstRun),
    m_lastRun(lastRun)
{ }

InsituDataCorrection::~InsituDataCorrection() {

}

StatusCode InsituDataCorrection::initialize() {

  ATH_MSG_INFO("Initializing In Situ correction.");

  if(!m_config){
    ATH_MSG_ERROR("In Situ data correction tool received a null config pointer.");
    return StatusCode::FAILURE;
  }

  m_jetStartScale = m_config->GetValue("InsituStartingScale","JetGSCScaleMomentum");

  //Retrieve the name of root file containing the in-situ calibration histograms from the config
  TString insitu_filename = m_config->GetValue("InsituCalibrationFile","None");
  //Should be the Relative and Abosolute Insitu correction applied?
  m_applyRelativeandAbsoluteInsitu = m_config->GetValue("ApplyRelativeandAbsoluteInsitu", true);
  //Should be a Eta restriction in the Relative and Absolute Insitu correction?
  m_applyEtaRestrictionRelativeandAbsolute = m_config->GetValue("ApplyEtaRestrictionRelativeandAbsolute", false);
  //Should be applied the Residual MC based Insitu correction?
  m_applyResidualMCbasedInsitu = m_config->GetValue("ApplyResidualMCbasedInsitu", false);
  //Should be a Eta restriction in the Residual MC based Insitu correction?
  m_applyEtaRestrictionResidualMCbased = m_config->GetValue("ApplyEtaRestrictionResidualMCbased", false);
  //Retrieve the name of the histogram for the relative in-situ calibration
  TString rel_histoname = m_config->GetValue("RelativeInsituCalibrationHistogram","");
  //Retrieve the name of the histogram for the absolute in-situ calibration 
  TString abs_histoname = m_config->GetValue("AbsoluteInsituCalibrationHistogram","");
  //Retrieve the name of the histogram for the absolute in-situ calibration 
  TString residualmcbased_histoname = m_config->GetValue("ResidualMCbasedInsituCalibrationHistogram","");
  //Retrieve the description/name of the in-situ calibration
  TString insitu_desc = m_config->GetValue("InsituCalibrationDescription","");
  //Retrieve the Eta restriction on the Residual MC based insitu calibration
  double insitu_etarestriction_residualmcbased = m_config->GetValue("InsituEtaRestrictionResidualMCbased",0.8);
  //Retrieve the Eta restriction on the Relative and Absolute insitu calibration
  double insitu_etarestriction_relativeandabsolute = m_config->GetValue("InsituEtaRestrictionRelativeandAbsolute",0.8);
  // Apply Insitu only to calo and TA mass calibrated jets (only for large jets)
  m_applyInsituCaloTAjets = m_config->GetValue("ApplyInsituCaloTAJets", false);
  // Apply in situ JMS:
  m_applyInsituJMS = m_config->GetValue("ApplyInsituJMS", false);

  //Find the absolute path to the insitu root file
  if ( !insitu_filename.EqualTo("None") ){
    if(m_dev){
      insitu_filename.Remove(0,32);
      insitu_filename.Insert(0,"JetCalibTools/");
    }
    else{insitu_filename.Insert(14,m_calibAreaTag);}
    insitu_filename=PathResolverFindCalibFile(insitu_filename.Data());
  }

  std::unique_ptr<TFile> insitu_file(TFile::Open(insitu_filename));
  if ( !insitu_file ) { ATH_MSG_FATAL( "Cannot open InsituCalibrationFile: " << insitu_filename ); return StatusCode::FAILURE; }

  ATH_MSG_INFO("Reading In-situ correction factors from: " << insitu_filename);

  rel_histoname.ReplaceAll("JETALGO",m_jetAlgo); abs_histoname.ReplaceAll("JETALGO",m_jetAlgo);
  if(m_applyRelativeandAbsoluteInsitu){
    std::unique_ptr<const TH2> rel_histo(JetCalibUtils::GetHisto2(*insitu_file,rel_histoname));
    std::unique_ptr<const TH1> abs_histo(JetCalibUtils::GetHisto(*insitu_file,abs_histoname));
    if ( !rel_histo || !abs_histo ) {
      ATH_MSG_FATAL( "\n  Tool configured for data, but no residual in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
    gROOT->cd();
    // save pTmax of the relative and absolute in situ calibrations
    m_relhistoPtMax = rel_histo->GetXaxis()->GetBinLowEdge(rel_histo->GetNbinsX()+1);
    m_abshistoPtMax = abs_histo->GetBinLowEdge(abs_histo->GetNbinsX()+1);
    // combine in situ calibrations
    m_insituCorr = combineCalibration(rel_histo.get(),abs_histo.get());
    m_insituEtaMax = m_insituCorr->GetYaxis()->GetBinLowEdge(m_insituCorr->GetNbinsY()+1);
    m_insituPtMin = m_insituCorr->GetXaxis()->GetBinLowEdge(1);
    m_insituPtMax = m_insituCorr->GetXaxis()->GetBinLowEdge(m_insituCorr->GetNbinsX()+1);
    if(m_applyEtaRestrictionRelativeandAbsolute) m_insituEtaMax = insitu_etarestriction_relativeandabsolute;
  }
  if(m_applyResidualMCbasedInsitu){
    m_insituCorr_ResidualMCbased = JetCalibUtils::GetHisto2(*insitu_file,residualmcbased_histoname);
    if ( !m_insituCorr_ResidualMCbased ) {
      ATH_MSG_FATAL( "\n  Tool configured for the Residual MC based correction, but no residualmcbased in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
    else{
      gROOT->cd();
      m_insituEtaMax_ResidualMCbased = m_insituCorr_ResidualMCbased->GetYaxis()->GetBinLowEdge(m_insituCorr_ResidualMCbased->GetNbinsY()+1);
      m_insituPtMin_ResidualMCbased = m_insituCorr_ResidualMCbased->GetXaxis()->GetBinLowEdge(1);
      m_insituPtMax_ResidualMCbased = m_insituCorr_ResidualMCbased->GetXaxis()->GetBinLowEdge(m_insituCorr_ResidualMCbased->GetNbinsX()+1);
    }
    if(m_applyEtaRestrictionResidualMCbased) m_insituEtaMax_ResidualMCbased = insitu_etarestriction_residualmcbased;
  }
  if(!m_applyRelativeandAbsoluteInsitu && !m_applyResidualMCbasedInsitu){
    ATH_MSG_FATAL( "\n  Tool configured for Insitu correction, but no in-situ histograms could be retrieved. Aborting..." );
    return StatusCode::FAILURE;
  }

  //Large-R in situ JMS calibration  
  if(m_applyInsituJMS){
    //Retrieve the name of root files containing the in-situ calibration histograms from the config
    TString insituJMS_filename = m_config->GetValue("InsituCalibrationFile_JMS","None");
    //Retrieve the name of the histogram for the absolute in-situ calibration
    TString abs_histoname_JMS = m_config->GetValue("AbsoluteInsituCalibrationHistogram_JMS","");
    TString abs_histoname_JMS_TA = m_config->GetValue("AbsoluteInsituCalibrationHistogram_JMS_TA","");
    //Retrieve the eta range for the in-situ JMS calibration
    double insitu_etarestriction_JMS = m_config->GetValue("InsituEtaRestriction_JMS",2.0);

    //Find the absolute path to the insitu root file Low and High Mass
    if ( !insituJMS_filename.EqualTo("None")){
      if(m_dev){
        insituJMS_filename.Remove(0,32);
        insituJMS_filename.Insert(0,"JetCalibTools/");
      }
      else{
        insituJMS_filename.Insert(14,m_calibAreaTag);
      }
      insituJMS_filename=PathResolverFindCalibFile(insituJMS_filename.Data());
    }

    std::unique_ptr<TFile> insituJMS_file(TFile::Open(insituJMS_filename));
    if ( !insituJMS_file ) { ATH_MSG_FATAL( "Cannot open InsituJMSCalibrationFile: " << insituJMS_filename ); return StatusCode::FAILURE; }

    ATH_MSG_INFO("Reading In-situ JMS correction factors from: " << insituJMS_filename);

    abs_histoname_JMS.ReplaceAll("JETALGO",m_jetAlgo);
    if(m_applyInsituCaloTAjets){
      abs_histoname_JMS_TA.ReplaceAll("JETALGO",m_jetAlgo);
    }

    if(m_applyRelativeandAbsoluteInsitu){
      std::unique_ptr<const TH2> abs_histo_JMS(JetCalibUtils::GetHisto2(*insituJMS_file,abs_histoname_JMS));
      if ( !abs_histo_JMS ) {
        ATH_MSG_FATAL( "\n  Tool configured for data, but no in-situ JMS histogram could be retrieved. Aborting..." );
        return StatusCode::FAILURE;
      }
      else {
        gROOT->cd();
        // combine in situ calibrations
        m_insituCorr_JMS    = invertHistogram(abs_histo_JMS.get());
        m_insituEtaMax_JMS  = insitu_etarestriction_JMS;
        m_insituPtMin_JMS   = m_insituCorr_JMS->GetXaxis()->GetBinLowEdge(1);
        m_insituPtMax_JMS   = m_insituCorr_JMS->GetXaxis()->GetBinLowEdge(m_insituCorr_JMS->GetNbinsX()+1);
        m_insituMassMin_JMS = m_insituCorr_JMS->GetYaxis()->GetBinLowEdge(1);
        m_insituMassMax_JMS = m_insituCorr_JMS->GetYaxis()->GetBinLowEdge((m_insituCorr_JMS->GetNbinsY()+1));

        if(m_applyInsituCaloTAjets){

          std::unique_ptr<const TH2> abs_histo_JMS_TA(JetCalibUtils::GetHisto2(*insituJMS_file,abs_histoname_JMS_TA));

          if ( !abs_histo_JMS_TA ){
            ATH_MSG_FATAL( "\n  Tool configured for data, but no in-situ JMS histogram for TA mass could be retrieved. Aborting..." );
            return StatusCode::FAILURE;
          }

          gROOT->cd();
          m_insituCorr_JMS_TA = invertHistogram(abs_histo_JMS_TA.get());
        }
      }
    }
    if(!m_applyRelativeandAbsoluteInsitu){
      ATH_MSG_FATAL( "\n  Tool configured for Insitu correction, but no in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
  }

  ATH_MSG_INFO("Tool configured to calibrate data");
  ATH_MSG_INFO("In-situ correction to be applied: " << insitu_desc);
  return StatusCode::SUCCESS;

}

StatusCode InsituDataCorrection::calibrate(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {

  //If this is a time-dependent calibration and we're not on a relevant run, don't calibrate.
  unsigned int runNumber = static_cast<unsigned int>(jetEventInfo.runNumber()+0.5);
  if(m_lastRun > 0 && (runNumber < m_firstRun || runNumber > m_lastRun))
    return StatusCode::SUCCESS;

  float detectorEta = jet.getAttribute<float>("DetectorEta");

  // For small R jets or large-R jets without calo-TA combination
  if(!m_applyInsituCaloTAjets){
    xAOD::JetFourMom_t jetStartP4;
    ATH_CHECK( setStartP4(jet) );
    jetStartP4 = jet.jetP4();

    xAOD::JetFourMom_t calibP4=jetStartP4;
  
    if(m_applyResidualMCbasedInsitu) calibP4=calibP4*getInsituCorr( calibP4.pt(), fabs(detectorEta), "ResidualMCbased" );
    
    if(m_dev){
      float insituFactor = getInsituCorr( jetStartP4.pt(), detectorEta, "RelativeAbs" );
      jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor",insituFactor);
    }

    if(m_applyRelativeandAbsoluteInsitu) calibP4=calibP4*getInsituCorr( jetStartP4.pt(), detectorEta, "RelativeAbs" );

    // Only for large R jets with insitu JMS but no combination
    if(m_applyInsituJMS){
      xAOD::JetFourMom_t calibP4_JMS;
      calibP4_JMS = calibP4;

      calibP4_JMS=calibP4*getInsituCorr_JMS( calibP4.pt(), calibP4.M(), detectorEta, "RelativeAbs", false );

      // pT doesn't change while applying in situ JMS
      TLorentzVector TLVjet;
      TLVjet.SetPtEtaPhiM( calibP4.pt(), calibP4.eta(), calibP4.phi(), calibP4_JMS.M() );
      calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );
    }

    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );
  }

  // For large-R jets: insitu needs to be applied to calo mass and TA mass (by default it's only applied to combined mass)
  if(m_applyInsituCaloTAjets){

    // calo mass calibrated jets
    xAOD::JetFourMom_t jetStartP4_calo;
    xAOD::JetFourMom_t calibP4_calo;
    if(jet.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jetStartP4_calo)){
      calibP4_calo=jetStartP4_calo;
    }else{
      ATH_MSG_FATAL( "Cannot retrieve JetJMSScaleMomentumCalo jets" );
      return StatusCode::FAILURE;
    }

    if(m_applyResidualMCbasedInsitu) calibP4_calo=calibP4_calo*getInsituCorr( calibP4_calo.pt(), fabs(detectorEta), "ResidualMCbased" );

    if(m_dev){
      float insituFactor_calo = getInsituCorr( jetStartP4_calo.pt(), detectorEta, "RelativeAbs" );
      jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor_calo",insituFactor_calo);
    }

    if(m_applyRelativeandAbsoluteInsitu) calibP4_calo=calibP4_calo*getInsituCorr( jetStartP4_calo.pt(), detectorEta, "RelativeAbs" );

    if(m_applyInsituJMS){
      xAOD::JetFourMom_t calibP4_calo_JMS;
      calibP4_calo_JMS = calibP4_calo;

      calibP4_calo_JMS=calibP4_calo*getInsituCorr_JMS( calibP4_calo.pt(), calibP4_calo.M(), detectorEta, "RelativeAbs", false );

      // pT doesn't change while applying in situ JMS
      TLorentzVector TLVjet_calo;
      TLVjet_calo.SetPtEtaPhiM( calibP4_calo.pt(), calibP4_calo.eta(), calibP4_calo.phi(), calibP4_calo_JMS.M() );
      calibP4_calo.SetPxPyPzE( TLVjet_calo.Px(), TLVjet_calo.Py(), TLVjet_calo.Pz(), TLVjet_calo.E() );
    }

    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumCalo",calibP4_calo);
    jet.setJetP4( calibP4_calo );

    // TA mass calibrated jets
    xAOD::JetFourMom_t jetStartP4_ta;
    xAOD::JetFourMom_t calibP4_ta;
    if(jet.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA", jetStartP4_ta)){
      calibP4_ta=jetStartP4_ta;
    }else{
      ATH_MSG_FATAL( "Cannot retrieve JetJMSScaleMomentumTA jets" );
      return StatusCode::FAILURE;
    }

    if(m_applyResidualMCbasedInsitu) calibP4_ta=calibP4_ta*getInsituCorr( calibP4_ta.pt(), fabs(detectorEta), "ResidualMCbased" );

    if(m_dev){
      float insituFactor_ta = getInsituCorr( jetStartP4_ta.pt(), detectorEta, "RelativeAbs" );
      jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor_ta",insituFactor_ta);
    }

    if(m_applyRelativeandAbsoluteInsitu) calibP4_ta=calibP4_ta*getInsituCorr( jetStartP4_ta.pt(), detectorEta, "RelativeAbs" );

    if(m_applyInsituJMS){
      xAOD::JetFourMom_t calibP4_ta_JMS;
      calibP4_ta_JMS = calibP4_ta;

      calibP4_ta_JMS=calibP4_ta*getInsituCorr_JMS( calibP4_ta.pt(), calibP4_ta.M(), detectorEta, "RelativeAbs", true );

      // pT doesn't change while applying in situ JMS
      TLorentzVector TLVjet_ta;
      TLVjet_ta.SetPtEtaPhiM( calibP4_ta.pt(), calibP4_ta.eta(), calibP4_ta.phi(), calibP4_ta_JMS.M() );
      calibP4_ta.SetPxPyPzE( TLVjet_ta.Px(), TLVjet_ta.Py(), TLVjet_ta.Pz(), TLVjet_ta.E() );
    }

    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumTA",calibP4_ta);
    jet.setJetP4( calibP4_ta );

  }

  return StatusCode::SUCCESS;
}

double InsituDataCorrection::getInsituCorr(double pt, double eta, std::string calibstep) const {
  if (m_insituCorr==NULL && m_insituCorr_ResidualMCbased==NULL) return 1.0;
  double myEta = eta, myPt = pt/m_GeV;

  //eta and pt ranges depends on the insitu calibration
  double etaMax = m_insituEtaMax;
  double ptMin  = m_insituPtMin;
  double ptMax  = m_insituPtMax;
  if (calibstep == "ResidualMCbased"){
    etaMax = m_insituEtaMax_ResidualMCbased;
    ptMin  = m_insituPtMin_ResidualMCbased;
    ptMax  = m_insituPtMax_ResidualMCbased;
  }

  //protection against values outside the histogram range, snap back to the lowest/highest bin edge
  if ( myPt <= ptMin ) myPt = ptMin + 1e-6;
  else if ( myPt >= ptMax ) myPt = ptMax - 1e-6;
  if (calibstep == "ResidualMCbased" && m_applyEtaRestrictionResidualMCbased) {
    if(myEta>=etaMax) return 1.0;
    return m_insituCorr_ResidualMCbased->Interpolate(myPt,myEta);
  }
  if (calibstep == "RelativeAbs" && m_applyEtaRestrictionRelativeandAbsolute) {
    if(myEta>=etaMax) return 1.0;
    else if(myEta<=-etaMax) return 1.0;
  }
  if (myEta <= -etaMax) myEta = 1e-6 - etaMax;
  else if (myEta >= etaMax) myEta = etaMax - 1e-6;
  if (calibstep == "ResidualMCbased" && !m_applyEtaRestrictionResidualMCbased){
    return m_insituCorr_ResidualMCbased->Interpolate(myPt,myEta);
  }
  return m_insituCorr->Interpolate(myPt,myEta);
}

double InsituDataCorrection::getInsituCorr_JMS(double pt, double mass, double eta, std::string calibstep, bool isTAmass) const {

  if(!isTAmass){
    if (!m_insituCorr_JMS) return 1.0;
  }
  else{
    if (!m_insituCorr_JMS_TA) return 1.0;
  }

  double myEta = eta, myPt = pt/m_GeV, myMass = mass/m_GeV;

  //eta and pt ranges depends on the insitu calibration
  double etaMax  = m_insituEtaMax_JMS;
  double ptMin   = m_insituPtMin_JMS;
  double ptMax   = m_insituPtMax_JMS;
  double massMin = m_insituMassMin_JMS;
  double massMax = m_insituMassMax_JMS;

  //protection against values outside the histogram range, snap back to the lowest/highest bin edge
  if ( myPt <= ptMin ) myPt = ptMin + 1e-6;
  else if ( myPt >= ptMax ) myPt = ptMax - 1e-6;
  if (calibstep == "RelativeAbs" && m_applyEtaRestrictionRelativeandAbsolute) {
    if(myEta>=etaMax) return 1.0;
    else if(myEta<=-etaMax) return 1.0;
  }
  if (myEta <= -etaMax) myEta = 1e-6 - etaMax;
  else if (myEta >= etaMax) myEta = etaMax - 1e-6;
  if (myMass <= massMin ) myMass = massMin + 1e-6;
  else if (myMass >= massMax ) myMass = massMax - 1e-6;

  double calibFactor = 1.0;
  if(!isTAmass){
    calibFactor = m_insituCorr_JMS->Interpolate(myPt,myMass);
  }
  else{
    calibFactor = m_insituCorr_JMS_TA->Interpolate(myPt,myMass);
  }

  return calibFactor;
}



std::unique_ptr<const TH2> InsituDataCorrection::combineCalibration(const TH2* h2d, const TH1* h) {
  std::unique_ptr<TH2> prod(static_cast<TH2*>(h2d->Clone()));
  for (int xi=1;xi<=prod->GetNbinsX();xi++) {
    double pt=prod->GetXaxis()->GetBinCenter(xi);
    const double R_abs=h->Interpolate(pt); // Rdata/RMC for the absolute scale
    const double inv_R_abs = 1. / R_abs;
    //printf("pT = %7.1f GeV, abs calib: %.4f\n",pt,abs);
    for (int yi=1;yi<=prod->GetNbinsY();yi++) {
      double c_rel = h2d->GetBinContent(xi,yi); // 1/Rrel = RMC/Rdata
      prod->SetBinContent(xi,yi,c_rel*inv_R_abs);
    }
  }
  return prod;
}

std::unique_ptr<const TH2> InsituDataCorrection::invertHistogram(const TH2* h2d){
  std::unique_ptr<TH2> inv(static_cast<TH2*>(h2d->Clone()));
  for (int xi=1;xi<=inv->GetNbinsX();xi++) {
    for (int yi=1;yi<=inv->GetNbinsY();yi++) {
      inv->SetBinContent(xi,yi,1./h2d->GetBinContent(xi,yi));

    }
  }
  return inv;
}
