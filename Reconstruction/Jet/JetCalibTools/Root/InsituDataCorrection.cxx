/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "PathResolver/PathResolver.h"

InsituDataCorrection::InsituDataCorrection()
  : JetCalibrationToolBase::JetCalibrationToolBase("InsituDataCorrection::InsituDataCorrection"),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_insituCorr(NULL), m_insituCorr_ResidualMCbased(NULL)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_insituCorr(NULL), m_insituCorr_ResidualMCbased(NULL)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev), m_insituCorr(NULL), m_insituCorr_ResidualMCbased(NULL)
{ }

InsituDataCorrection::~InsituDataCorrection() {

  if(m_insituCorr) delete m_insituCorr;
  if(m_insituCorr_ResidualMCbased) delete m_insituCorr_ResidualMCbased;

}

StatusCode InsituDataCorrection::initializeTool(const std::string&) {

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


  //Find the absolute path to the insitu root file
  if ( !insitu_filename.EqualTo("None") ){
    if(m_dev){
      insitu_filename.Remove(0,32);
      insitu_filename.Insert(0,"JetCalibTools/");
    }
    else{insitu_filename.Insert(14,m_calibAreaTag);}
    insitu_filename=PathResolverFindCalibFile(insitu_filename.Data());
  }

  TFile *insitu_file = TFile::Open(insitu_filename);
  if ( !insitu_file ) { ATH_MSG_FATAL( "Cannot open InsituCalibrationFile: " << insitu_filename ); return StatusCode::FAILURE; }

  ATH_MSG_INFO("Reading In-situ correction factors from: " << insitu_filename);

  rel_histoname.ReplaceAll("JETALGO",m_jetAlgo); abs_histoname.ReplaceAll("JETALGO",m_jetAlgo);
  if(m_applyRelativeandAbsoluteInsitu){
    TH2D * rel_histo = (TH2D*)JetCalibUtils::GetHisto2(insitu_file,rel_histoname);
    TH1D * abs_histo = (TH1D*)JetCalibUtils::GetHisto(insitu_file,abs_histoname);
    if ( !rel_histo || !abs_histo ) {
      ATH_MSG_FATAL( "\n  Tool configured for data, but no residual in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
    else {
      gROOT->cd();
      // save pTmax of the relative and absolute in situ calibrations
      m_relhistoPtMax = rel_histo->GetXaxis()->GetBinLowEdge(rel_histo->GetNbinsX()+1);
      m_abshistoPtMax = abs_histo->GetBinLowEdge(abs_histo->GetNbinsX()+1);
      // combine in situ calibrations
      m_insituCorr = combineCalibration(rel_histo,abs_histo);
      m_insituEtaMax = m_insituCorr->GetYaxis()->GetBinLowEdge(m_insituCorr->GetNbinsY()+1);
      m_insituPtMin = m_insituCorr->GetXaxis()->GetBinLowEdge(1);
      m_insituPtMax = m_insituCorr->GetXaxis()->GetBinLowEdge(m_insituCorr->GetNbinsX()+1);
    }
    if(m_applyEtaRestrictionRelativeandAbsolute) m_insituEtaMax = insitu_etarestriction_relativeandabsolute;
  }
  if(m_applyResidualMCbasedInsitu){
    m_insituCorr_ResidualMCbased = (TH2D*)JetCalibUtils::GetHisto2(insitu_file,residualmcbased_histoname);
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
  if(!m_applyRelativeandAbsoluteInsitu & !m_applyResidualMCbasedInsitu){
    ATH_MSG_FATAL( "\n  Tool configured for Insitu correction, but no in-situ histograms could be retrieved. Aborting..." );
    return StatusCode::FAILURE;
  }

  //insitu_file->Close();
  ATH_MSG_INFO("Tool configured to calibrate data");
  ATH_MSG_INFO("In-situ correction to be applied: " << insitu_desc);
  return StatusCode::SUCCESS;

}

StatusCode InsituDataCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

float detectorEta = jet.getAttribute<float>("DetectorEta");

// For small R jets
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

  // If the jet mass calibration was applied the calibrated mass needs to be used!
  /*
  if(m_jetStartScale.Contains("JetJMSScaleMomentum")){
    TLorentzVector TLVjet;
    TLVjet.SetPtEtaPhiM( calibP4.pt(), jetStartP4.eta(), jetStartP4.phi(), jetStartP4.mass() ); // mass at JMS
    calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() ); 
  }
  */

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );
}

  // For Large R jets: insitu needs to be apply also to calo mass calibrated jets and TA mass calibrated jets (by default it's only applied to combined mass calibrated jets)
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

TH2D * InsituDataCorrection::combineCalibration(TH2D *h2d, TH1D *h) {
  TH2D *prod = (TH2D*)h2d->Clone();
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
