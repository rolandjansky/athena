/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "PathResolver/PathResolver.h"

EtaJESCorrection::EtaJESCorrection()
  : asg::AsgTool( "EtaJESCorrection::EtaJESCorrection" ), JetCalibrationToolBase::JetCalibrationToolBase(),
    m_config(NULL), m_jetAlgo(""),
    m_minPt_JES(10), m_minPt_EtaCorr(8), m_maxE_EtaCorr(2500),
    m_lowPtExtrap(0), m_lowPtMinR(0.25),
    m_etaBinAxis(NULL)
{ }

EtaJESCorrection::EtaJESCorrection(const std::string& name)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""),
    m_minPt_JES(10), m_minPt_EtaCorr(8), m_maxE_EtaCorr(2500),
    m_lowPtExtrap(0), m_lowPtMinR(0.25),
    m_etaBinAxis(NULL)
{ }

EtaJESCorrection::EtaJESCorrection(const std::string& name, TEnv * config, TString jetAlgo)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo),
    m_minPt_JES(10), m_minPt_EtaCorr(8), m_maxE_EtaCorr(2500),
    m_lowPtExtrap(0), m_lowPtMinR(0.25),
    m_etaBinAxis(NULL)
{ }

EtaJESCorrection::~EtaJESCorrection() {

  if (m_etaBinAxis) delete m_etaBinAxis;

}

//bool EtaJESCorrection::initializeTool(TEnv * config, TString jetAlgo) {
StatusCode EtaJESCorrection::initializeTool(const std::string&) {

  m_jetStartScale = m_config->GetValue("EtaJESStartingScale","JetPileupScaleMomentum");

  //TString calibFile = FindFile(m_config->GetValue("AbsoluteJES.CalibFile",""));
  TString calibFile = PathResolverFindCalibFile(m_config->GetValue("AbsoluteJES.CalibFile",""));
  m_config->ReadFile(calibFile, kEnvLocal);
  ATH_MSG_INFO("  \n\nReading absolute calibration factors from:\n    " << calibFile << "\n");
  m_jesDesc = m_config->GetValue("AbsoluteJES.Description","");
  ATH_MSG_INFO("  Description:\n    " << m_jesDesc << "\n");

  // minPt_JES (always in GeV) determines at which point we stop using the correction curve and switch to an extrapolated value
  m_minPt_JES = m_config->GetValue(m_jetAlgo+".MinPtForETAJES",10);
  //Which extrapolation method to use at low Et (Et < _minPt_JES)
  m_lowPtExtrap = m_config->GetValue("LowPtJESExtrapolationMethod",0);
  //For order 2 extrapolation only, set the minimum value of the response for Et = 0
  m_lowPtMinR = m_config->GetValue("LowPtJESExtrapolationMinimumResponse",0.25);

  // From mswiatlo, help from dag: variable eta binning
  std::vector<double> etaBins = JetCalibUtils::VectorizeD(m_config->GetValue("JES.EtaBins",""));
  if (etaBins.size()==0){ // default binning
    for (int i=0;i<=90; i++) 
      etaBins.push_back(0.1*i-4.5);
  }
  else if (etaBins.size()==0) { ATH_MSG_FATAL("JES.EtaBins incorrectly specified"); return StatusCode::FAILURE; }
  else if (etaBins.size()>s_nEtaBins+1) {
    ATH_MSG_FATAL( "JES.EtaBins has " << etaBins.size()-1 << " bins, can be maximally 90!" );
    return StatusCode::FAILURE;
  }
  m_etaBinAxis = new TAxis(etaBins.size()-1,&etaBins[0]);

  for (uint ieta=0;ieta<etaBins.size()-1;++ieta) {

    // Read in absolute JES calibration factors
    TString key=Form("JES.%s_Bin%d",m_jetAlgo.Data(),ieta);
    std::vector<double> params = JetCalibUtils::VectorizeD(m_config->GetValue(key,""));
    if (params.size()!=s_nPar) { ATH_MSG_FATAL( "Cannot read JES calib constants " << key ); return StatusCode::FAILURE; }
    for (uint ipar=0;ipar<s_nPar;++ipar) m_JESFactors[ieta][ipar] = params[ipar];

      //Protections for high order extrapolation methods at low Et (Et < _minPt_JES)
      if(m_lowPtExtrap > 0) {
	//Calculate the slope of the response curve at the minPt for each eta bin
	//Used in the GetLowPtJES method when Pt < minPt
	const double *factors = m_JESFactors[ieta];
	const double Ecutoff = m_minPt_JES*cosh(etaBins[ieta]);
	const double Rcutoff = getLogPolN(factors,Ecutoff);
	const double Slope = getLogPolNSlope(factors,Ecutoff);
	if(Slope > Rcutoff/Ecutoff) ATH_MSG_FATAL("Slope of calibration curve at minimum ET is too steep for the JES factors of etabin " << ieta << ", eta = " << etaBins[ieta] );

	m_JES_MinPt_E[ieta] = Ecutoff;
	m_JES_MinPt_R[ieta] = Rcutoff;
	m_JES_MinPt_Slopes[ieta] = Slope;
      
	//Calculate the parameters for a 2nd order polynomial extension to the calibration curve below minimum ET
	//Used in the GetLowPtJES method when Pt < minPt
	if(m_lowPtExtrap == 2) {
	  const double h = m_lowPtMinR;
	  const double Param1 = (2/Ecutoff)*(Rcutoff-h)-Slope;
	  const double Param2 = (0.5/Ecutoff)*(Slope-Param1);
	  //Slope of the calibration curve should always be positive
	  if( Param1 < 0 || Param1 + 2*Param2*Ecutoff < 0) ATH_MSG_FATAL("Polynomial extension to calibration curve below minimum ET is not monotonically increasing for etabin " << ieta << ", eta = " << etaBins[ieta] );
	  m_JES_MinPt_Param1[ieta] = Param1;
	  m_JES_MinPt_Param2[ieta] = Param2;
	}
      }

    // Read in jet eta calibration factors
    key=Form("EtaCorr.%s_Bin%d",m_jetAlgo.Data(),ieta);
    params = JetCalibUtils::VectorizeD(m_config->GetValue(key,""));
    if (params.size()!=s_nPar) { ATH_MSG_FATAL( "Cannot read jet eta calib constants " << key ); return StatusCode::FAILURE; }
    for (uint ipar=0;ipar<s_nPar;++ipar) m_etaCorrFactors[ieta][ipar] = params[ipar];

  }

  return StatusCode::SUCCESS;
}

StatusCode EtaJESCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  //Apply the JES calibration scale factor
  //Takes the uncorrected jet eta (in case the origin and/or 4vector jet area corrections were applied
  float detectorEta = jet.getAttribute<float>("DetectorEta");
  xAOD::JetFourMom_t calibP4 = jetStartP4*getJES( jetStartP4.e(), detectorEta );

  const double etaCorr = calibP4.eta() + getEtaCorr( calibP4.e(), detectorEta );
  TLorentzVector TLVjet;
  TLVjet.SetPtEtaPhiM( calibP4.P()/cosh(etaCorr),etaCorr,calibP4.phi(),calibP4.mass() );
  calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );
  
  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetEtaJESScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS; 
}

// E_uncorr is the EM-scale, or LC-scale jet energy
// eta_det is the eta of the raw, constituent-scale jet (constscale_eta)
double EtaJESCorrection::getJES(double E_uncorr, double eta_det) const {

  double E = E_uncorr/m_GeV; // E in GeV
  //Check if the Pt goes below the minimum value, if so use the special GetLowPtJES method
  if ( E/cosh(eta_det) < m_minPt_JES ) {
    double R = getLowPtJES(E,eta_det);
    return 1.0/R;
  }

  // Get the factors
  int ieta = getEtaBin(eta_det);
  const double *factors = m_JESFactors[ieta];
  
  // Calculate the jet response and then the JES as 1/R
  double R = getLogPolN(factors,E);
  return 1.0/R;
}

double EtaJESCorrection::getLowPtJES(double E_uncorr, double eta_det) const {
  int ieta = getEtaBin(eta_det);
  if (m_lowPtExtrap == 0) {
    const double *factors = m_JESFactors[ieta];
    double E = m_minPt_JES*cosh(eta_det);
    double R = getLogPolN(factors,E);
    return R;
  }
  else if (m_lowPtExtrap == 1) {
    double Ecutoff = m_JES_MinPt_E[ieta];
    double Rcutoff = m_JES_MinPt_R[ieta];
    double slope = m_JES_MinPt_Slopes[ieta];
    double R = slope*(E_uncorr-Ecutoff)+Rcutoff;
    return R;
  }
  else if(m_lowPtExtrap == 2) {
    double minR = m_lowPtMinR;
    double R = minR + m_JES_MinPt_Param1[ieta]*E_uncorr + m_JES_MinPt_Param2[ieta]*E_uncorr*E_uncorr;
    return R;
  }
  else ATH_MSG_WARNING("Incorrect specification of low Pt JES extrapolation, please check the value of the LowPtJESExtrapolationMethod config flag.");
  return 1;
}

double EtaJESCorrection::getEtaCorr(double E_corr, double eta_det) const {
  int ieta = getEtaBin(eta_det);
  const double *factors = m_etaCorrFactors[ieta];
  
  double E = E_corr/m_GeV;
  if ( E < m_minPt_EtaCorr*cosh(eta_det) ) 
    E = m_minPt_EtaCorr*cosh(eta_det);
  if ( E>m_maxE_EtaCorr ) E=m_maxE_EtaCorr;
  
  double etaCorr = getLogPolN(factors,E);
  
  // This is ( reco_eta - truth_eta )
  // to make it an additive correction return the negative value
  return -etaCorr;
}

double EtaJESCorrection::getLogPolN(const double *factors, double x) const {
  double y=0;
  for ( uint i=0; i<s_nPar; ++i )
    y += factors[i]*TMath::Power(log(x),Int_t(i));
  return y;
}

double EtaJESCorrection::getLogPolNSlope(const double *factors, double x) const {
  double y=0;
  for ( uint i=0; i<s_nPar; ++i )
    y += i*factors[i]*TMath::Power(log(x),Int_t(i-1))/x;
  return y;
}

int EtaJESCorrection::getEtaBin(double eta_det) const {
  int bin = m_etaBinAxis->FindBin(eta_det);
  if (bin<=0) return 0;
  if (bin>m_etaBinAxis->GetNbins()) return bin-2; // overflow
  return bin-1;
}

