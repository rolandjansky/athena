/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  JMS Calibration
 *
 *  Author: Jonathan Bossio (jbossios@cern.ch)
 *
 */

#include <TKey.h>

#include "JetCalibTools/CalibrationMethods/JMSCorrection.h"
#include "PathResolver/PathResolver.h"


JMSCorrection::JMSCorrection()
  : asg::AsgTool( "JMSCorrection::JMSCorrection" ), JetCalibrationToolBase::JetCalibrationToolBase(),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag("")
   
{ }

JMSCorrection::JMSCorrection(const std::string& name)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag("")
   
{ }

JMSCorrection::JMSCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag)
{ }

JMSCorrection::~JMSCorrection() {

}

StatusCode JMSCorrection::initializeTool(const std::string&) {

  ATH_MSG_INFO("  \nInitializing the JMS Calibration tool\n\n");

  m_jetStartScale = m_config->GetValue("JMSStartingScale","JetGSCScaleMomentum");
  // Starting pT value to calibrate
  m_pTMinCorr = m_config->GetValue("MinpT",180); 

  if ( !m_config ) { ATH_MSG_FATAL("Config file not specified. Aborting."); return StatusCode::FAILURE; }
  if ( m_jetAlgo.EqualTo("") ) { ATH_MSG_FATAL("No jet algorithm specified. Aborting."); return StatusCode::FAILURE; }

  //find the ROOT file containing response histograms, path comes from the config file.
  TString JMSFile = m_config->GetValue("MassCalibrationFile","empty");
  if ( JMSFile.EqualTo("empty") ) { 
    ATH_MSG_FATAL("NO JMSFactorsFile specified. Aborting.");
    return StatusCode::FAILURE;
  }
  JMSFile.Insert(14,m_calibAreaTag);
  TString fileName = PathResolverFindCalibFile(JMSFile.Data());
  TFile *inputFile = TFile::Open(fileName);
  if (!inputFile) ATH_MSG_FATAL("Cannot open JMS factors file" << fileName);

  ATH_MSG_INFO("  for " << m_jetAlgo << " jets\n\n");

  setMassEtaBins( JetCalibUtils::VectorizeD( m_config->GetValue("MassEtaBins","") ) );

  //Get a TList of TKeys pointing to the histograms contained in the ROOT file
  inputFile->cd();
  TList *keys = inputFile->GetListOfKeys();
  //fill the names of the TKeys into a vector of TStrings
  TIter ikeys(keys);
  while ( TKey *iterobj = (TKey*)ikeys() ) {
    TString histoName = iterobj->GetName();
    if ( histoName.Contains(m_jetAlgo) ) m_respFactorsMass.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoName.Data()) );
  }

  //Make sure we put something in the vector of TH2Fs
  if ( m_respFactorsMass.size() < 3 ) {
    ATH_MSG_FATAL("Vector of mass correction histograms may be empty. Please check your mass calibration file: " << JMSFile);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO("\n  JMS Tool has been initialized with binning and eta fit factors from " << fileName << "\n");
  return StatusCode::SUCCESS;

}

float JMSCorrection::getMassCorr(double pT_uncorr, double m_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_respFactorsMass[etabin]->GetXaxis()->GetBinLowEdge(m_respFactorsMass[etabin]->GetNbinsX()+1);
  double pTMin = m_respFactorsMass[etabin]->GetXaxis()->GetBinLowEdge(1);
  double massMax = m_respFactorsMass[etabin]->GetYaxis()->GetBinLowEdge(m_respFactorsMass[etabin]->GetNbinsY()+1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < m_pTMinCorr ) return 1; // no correction
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_uncorr > massMax ) m_uncorr = massMax-1e-6; //so it fits the up-most m-bin

  float mass_corr = m_respFactorsMass[etabin]->Interpolate( pT_uncorr, m_uncorr );

  return mass_corr;
}

StatusCode JMSCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

  //Apply the JMS calibration scale factor

  //Takes the uncorrected jet eta (in case the origin and/or 4vector jet area corrections were applied
  float detectorEta = jet.getAttribute<float>("DetectorEta");
  double absdetectorEta = fabs(detectorEta);

  xAOD::JetFourMom_t calibP4 = jet.jetP4();

  float mass_corr = jet.m();
  double pT_corr = jet.pt();

  // Determine mass eta bin to use
  int etabin=-99;
  if (m_massEtaBins.size()==0 || m_respFactorsMass.size() != m_massEtaBins.size()-1) ATH_MSG_FATAL("Please check that the mass correction eta binning is properly set in your config file");
  xAOD::JetConstituentVector constituents = jet.getConstituents();
  int nconstituents = constituents.size();
  if ( absdetectorEta < m_massEtaBins.back() && nconstituents>1 ) { //Fiducial Cuts
    for (uint i=0; i<m_massEtaBins.size()-1; ++i) {
        if(absdetectorEta >= m_massEtaBins[i] && absdetectorEta < m_massEtaBins[i+1]) etabin = i;
    }
    if (etabin< 0) ATH_MSG_FATAL("There was a problem determining the eta bin to use for the mass correction");
    mass_corr = jet.m()/getMassCorr( jet.pt()/m_GeV, jet.m()/m_GeV, etabin );
    pT_corr = sqrt(jet.e()*jet.e()-mass_corr*mass_corr)/cosh( jet.eta() );
  }
  
  TLorentzVector TLVjet;
  TLVjet.SetPtEtaPhiM( pT_corr, jet.eta(), jet.phi(), mass_corr );
  calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS;

}


