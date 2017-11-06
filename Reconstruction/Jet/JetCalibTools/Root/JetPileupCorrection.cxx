/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"

JetPileupCorrection::JetPileupCorrection()
  : JetCalibrationToolBase::JetCalibrationToolBase("JetPileupCorrection::JetPileupCorrection"),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_doResidual(false), m_doOrigin(false), m_isData(false),
    m_useFull4vectorArea(false), m_residualOffsetCorr(NULL), m_originScale("JetOriginConstitScaleMomentum")
{ }

JetPileupCorrection::JetPileupCorrection(const std::string& name)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(), m_dev(false), m_doResidual(false), m_doOrigin(false), m_isData(false),
    m_useFull4vectorArea(false), m_residualOffsetCorr(NULL), m_originScale("JetOriginConstitScaleMomentum")
{ }

JetPileupCorrection::JetPileupCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool doResidual, bool doOrigin, bool isData, bool dev, std::string originScale)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev), m_doResidual(doResidual), m_doOrigin(doOrigin), m_isData(isData),
    m_useFull4vectorArea(false), m_residualOffsetCorr(NULL), m_originScale(originScale)
{ }

JetPileupCorrection::~JetPileupCorrection() {

  if(m_residualOffsetCorr) delete m_residualOffsetCorr;

}


//bool JetPileupCorrection::initializeTool(const std::string& name, TEnv * config, TString jetAlgo, bool doResidual, bool isData) {
StatusCode JetPileupCorrection::initializeTool(const std::string& name) {

  ATH_MSG_INFO("OriginScale: " << m_originScale);

  m_jetStartScale = m_config->GetValue("PileupStartingScale","JetConstitScaleMomentum");
  ATH_MSG_INFO("JetPileupCorrection: Starting scale: " << m_jetStartScale);
  if ( m_jetStartScale == "DO_NOT_USE" ) {
    ATH_MSG_WARNING("Configuration file does not specify the jet starting scale!");
  }

  m_useFull4vectorArea = m_config->GetValue("ApplyFullJetArea4MomentumCorrection", false);
  ATH_MSG_INFO("Jet area pile up correction will be applied.");
  if ( m_useFull4vectorArea ) ATH_MSG_INFO("  Full 4-vector jet area correction is activated."); 
  //ATH_MSG_INFO(" \n");

  if ( m_doResidual ) { 
    std::string suffix = "_Residual";
    m_residualOffsetCorr = new ResidualOffsetCorrection(name+suffix,m_config,m_jetAlgo,m_calibAreaTag,m_isData,m_dev);
    m_residualOffsetCorr->msg().setLevel( this->msg().level() );
    ATH_CHECK( m_residualOffsetCorr->initializeTool(name+suffix) );
  }

  if ( m_doResidual && m_useFull4vectorArea ) { 
    ATH_MSG_WARNING("JetPileupCorrection::initializeTool : WARNING!! You have requested the 4 vector jet area correction and the residual offset correction. This configuration is not currently supported, the residual offset correction will be deactivated.");
     return StatusCode::SUCCESS;
  } else if ( !m_doResidual && !m_useFull4vectorArea ) {
    ATH_MSG_VERBOSE("JetPileupCorrection::initializeTool : You have requested the transverse jet area correction without the residual offset correction. This configuration is not recommended.");
    return StatusCode::SUCCESS;
  } else { 
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode JetPileupCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {
  ATH_MSG_VERBOSE("  Applying pileup calibration to jet " << jet.index());
  ATH_MSG_VERBOSE("    Initial jet pT = " << 0.001*jet.pt() << " GeV");

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  const double E_det = jetStartP4.e();
  const double pT_det = jetStartP4.pt();
  const double eta_det = jetStartP4.eta();
  const double mass_det = jetStartP4.mass();

  if ( E_det < mass_det ) {
    ATH_MSG_WARNING( "JetPileupCorrection::calibrateImpl : Current jet has mass=" << mass_det/m_GeV << " GeV, which is greater than it's energy=" << E_det/m_GeV << " GeV?? Aborting." );
    return StatusCode::FAILURE;
  }

  xAOD::JetFourMom_t jetareaP4 = jet.getAttribute<xAOD::JetFourMom_t>("ActiveArea4vec");
  ATH_MSG_VERBOSE("    Area = " << jetareaP4);
  const double rho = jetEventInfo.rho();
  ATH_MSG_VERBOSE("    Rho = " << rho);

  if(m_useFull4vectorArea) {
    ATH_MSG_VERBOSE("  Applying area-subtraction calibration to jet " << jet.index() << " with pT = " << 0.001*jet.pt() << " GeV");
    //subtract rho * the jet area from the jet
    xAOD::JetFourMom_t calibP4 = jetStartP4 - rho*jetareaP4;
    //Attribute to track if a jet has received the pileup subtraction (always true if this code was run)
    jet.setAttribute<int>("PileupCorrected",true);
    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );

  } else if ( m_residualOffsetCorr && !m_useFull4vectorArea ) {
    ATH_MSG_VERBOSE("  Applying residual pileup calibration to jet " << jet.index() << " with pT = " << 0.001*jet.pt() << " GeV");

    const double NPV = jetEventInfo.NPV();
    const double mu = jetEventInfo.mu();

    //Retrieve the offset correction from the residual correction class
    const double offsetET = m_residualOffsetCorr->GetResidualOffset(fabs(eta_det), mu, NPV);

    //Calculate the pT after jet areas and residual offset
    double pT_offset = pT_det - rho*jetareaP4.pt() - offsetET;

    //Set the jet pT to 10 MeV if the pT is negative after the jet area and residual offset corrections
    const double pileup_SF = pT_offset >= 0 ? pT_offset / pT_det : 0.01*m_GeV/pT_det;

    xAOD::JetFourMom_t calibP4;
    if ( m_doOrigin ) { 
      xAOD::JetFourMom_t jetOriginP4;
      static unsigned int originWarnings = 0;
      if ( jet.getAttribute<xAOD::JetFourMom_t>(m_originScale.c_str(),jetOriginP4) )
	calibP4 = jetOriginP4*pileup_SF;
      else {
	if ( originWarnings < 20 ) {
	  ATH_MSG_WARNING("Could not retrieve " << m_originScale << " jet attribute, origin correction will not be applied.");
	  ++originWarnings;
	}
	calibP4 = jetStartP4*pileup_SF;
      }
    } else
      calibP4 = jetStartP4*pileup_SF;

    //Attribute to track if a jet has received the origin correction
    jet.setAttribute<int>("OriginCorrected",m_doOrigin);
    //Attribute to track if a jet has received the pileup subtraction (always true if this code was run)
    jet.setAttribute<int>("PileupCorrected",true);
    
    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );

  } else {
    ATH_MSG_VERBOSE("  Applying postive-only area-subtraction calibration to jet " << jet.index() << " with pT = " << 0.001*jet.pt() << " GeV");
    //Set the jet pT to 10 MeV if the pT or energy is negative after the jet area correction
    const double area_SF = (pT_det-rho*jetareaP4.pt()<=0 || E_det-rho*jetareaP4.e()<=0) ? 10/pT_det : (pT_det-rho*jetareaP4.pt())/pT_det;
    xAOD::JetFourMom_t calibP4;
    if ( m_doOrigin ) { 
      xAOD::JetFourMom_t jetOriginP4;
      static unsigned int originWarnings = 0;
      if ( jet.getAttribute<xAOD::JetFourMom_t>(m_originScale.c_str(),jetOriginP4) )
	calibP4 = jetOriginP4*area_SF;
      else {
	if ( originWarnings < 20 ) {
	  ATH_MSG_WARNING("Could not retrieve " << m_originScale << " jet attribute, origin correction will not be applied.");
	  ++originWarnings;
	}
	calibP4 = jetStartP4*area_SF;
      }
    } else
      calibP4 = jetStartP4*area_SF;

    //Attribute to track if a jet has received the origin correction
    jet.setAttribute<int>("OriginCorrected",m_doOrigin);
    //Attribute to track if a jet has received the pileup subtraction (always true if this code was run)
    jet.setAttribute<int>("PileupCorrected",true);

    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );
  }

  ATH_MSG_VERBOSE("    Calibrated jet pT = " << 0.001*jet.pt() << " GeV");
  return StatusCode::SUCCESS;
}
