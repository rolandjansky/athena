/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tool to compute corrections to calorimeter isolation variables for photons

#include "PhotonAnalysisUtils/PAUcaloIsolationTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "CxxUtils/fpcompare.h"

#include "JetEvent/JetCollection.h"
#include "EventKernel/SignalStateHelper.h"


#include "egammaEvent/egamma.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "McParticleEvent/TruthParticle.h"

#include "TMath.h"

#include <iomanip>

#include <map>
#include <vector>

// constructor
PAUcaloIsolationTool::PAUcaloIsolationTool(const std::string& t, 
					   const std::string& n, 
					   const IInterface* p) : 
  AthAlgTool(t,n,p) {

  declareInterface<IPAUcaloIsolationTool>(this);

  declareProperty("DoAreaCorrections"  , m_do_area_corrections=false);
  declareProperty("JetCollectionName"  , m_jet_collection_name="Kt5TopoJets");
  declareProperty("JetAreaEtaBins"     , m_eta_bins_areaoffset);
  declareProperty("UseOldPtCorrections", m_useOldLeakageCorrections=false);
  declareProperty("UseOldAreaCorrections", m_useOldAreaCorrections=false);

  declareProperty("EMCaloIsoPtCorrectionTool", m_emCaloIsoPtCorrectionTool,
		  "Handle of the EMCaloIsoPtCorrectionTool");

  declareProperty("AreaCorrectionEtaWindow", m_eta_range = .75);
  declareProperty("JetAreaMomentName", m_jetAreaMomentName = "FJetArea");
}


// destructor
PAUcaloIsolationTool::~PAUcaloIsolationTool() {}


// initialize
StatusCode PAUcaloIsolationTool::initialize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;

  m_area_corrections_done = false;

  if(m_do_area_corrections){
    IIncidentSvc* incsvc;
    StatusCode sc = service("IncidentSvc", incsvc);
    int priority = 100;
    if( sc.isSuccess() ) {
      incsvc->addListener( this, "BeginEvent", priority);
      incsvc->addListener( this, "EndEvent");
    } else {
      log << MSG::ERROR << "PAUcaloIsolationTool::initialize(), could not get IncidentSvc" << endreq ;
      return sc ;
    }
  }

  if(m_emCaloIsoPtCorrectionTool.retrieve().isFailure()) {
    log << MSG::ERROR << "Unable to retrieve " << m_emCaloIsoPtCorrectionTool << endreq;
    return StatusCode::FAILURE;
  }

  // these numbers should not be changed without re-computing all of the coefficients
  // filled into the maps below.
  m_eta_bins.push_back(0.00);
  m_eta_bins.push_back(0.10);
  m_eta_bins.push_back(0.60);
  m_eta_bins.push_back(0.80);
  m_eta_bins.push_back(1.15);
  m_eta_bins.push_back(1.37);
  m_eta_bins.push_back(1.52);
  m_eta_bins.push_back(1.81);
  m_eta_bins.push_back(2.01);
  m_eta_bins.push_back(2.37);
  m_eta_bins.push_back(2.47);
  m_eta_bins.push_back(5.00); // there needs to be large value of eta in the last bin.


  // Some defaults, just in case the constants are not set properly in the jobOptions
  if(m_eta_bins_areaoffset.size() <= 0){
    m_eta_bins_areaoffset.push_back(0.00);
    m_eta_bins_areaoffset.push_back(1.50);
    m_eta_bins_areaoffset.push_back(3.0);
    m_eta_bins_areaoffset.push_back(4.0);
    m_eta_bins_areaoffset.push_back(5.0);
  }

  m_photon_pt_corrections[ 0][10] = 0.0084780; 
  m_photon_pt_corrections[ 1][10] = 0.0076953;
  m_photon_pt_corrections[ 2][10] = 0.0069350;
  m_photon_pt_corrections[ 3][10] = 0.0084322;
  m_photon_pt_corrections[ 4][10] = 0.0089547;
  m_photon_pt_corrections[ 5][10] = 0.0102446;
  m_photon_pt_corrections[ 6][10] = 0.0107278;
  m_photon_pt_corrections[ 7][10] = 0.0128994;
  m_photon_pt_corrections[ 8][10] = 0.0146968;
  m_photon_pt_corrections[ 9][10] = 0.0176137;
  m_photon_pt_corrections[10][10] = 0.0312793;

  m_photon_pt_corrections[ 0][15] = 0.013415; 
  m_photon_pt_corrections[ 1][15] = 0.012689;
  m_photon_pt_corrections[ 2][15] = 0.012406;
  m_photon_pt_corrections[ 3][15] = 0.015901;
  m_photon_pt_corrections[ 4][15] = 0.018028;
  m_photon_pt_corrections[ 5][15] = 0.019722;
  m_photon_pt_corrections[ 6][15] = 0.020109;
  m_photon_pt_corrections[ 7][15] = 0.023924;
  m_photon_pt_corrections[ 8][15] = 0.029580;
  m_photon_pt_corrections[ 9][15] = 0.035296;
  m_photon_pt_corrections[10][15] = 0.040739;

  m_photon_pt_corrections[ 0][20] = 0.014582; 
  m_photon_pt_corrections[ 1][20] = 0.013887;
  m_photon_pt_corrections[ 2][20] = 0.013786;
  m_photon_pt_corrections[ 3][20] = 0.017952;
  m_photon_pt_corrections[ 4][20] = 0.020799;
  m_photon_pt_corrections[ 5][20] = 0.022892;
  m_photon_pt_corrections[ 6][20] = 0.022947;
  m_photon_pt_corrections[ 7][20] = 0.027274;
  m_photon_pt_corrections[ 8][20] = 0.034607;
  m_photon_pt_corrections[ 9][20] = 0.04111 ;
  m_photon_pt_corrections[10][20] = 0.043765;

  m_photon_pt_corrections[ 0][25] = 0.0149287; 
  m_photon_pt_corrections[ 1][25] = 0.0142438;
  m_photon_pt_corrections[ 2][25] = 0.0142233;
  m_photon_pt_corrections[ 3][25] = 0.018665 ;
  m_photon_pt_corrections[ 4][25] = 0.0217781;
  m_photon_pt_corrections[ 5][25] = 0.0244446;
  m_photon_pt_corrections[ 6][25] = 0.0240101;
  m_photon_pt_corrections[ 7][25] = 0.0284588;
  m_photon_pt_corrections[ 8][25] = 0.0365284;
  m_photon_pt_corrections[ 9][25] = 0.0438571;
  m_photon_pt_corrections[10][25] = 0.0450998;

  m_photon_pt_corrections[ 0][30] = 0.0149955; 
  m_photon_pt_corrections[ 1][30] = 0.0143672;
  m_photon_pt_corrections[ 2][30] = 0.01439  ;
  m_photon_pt_corrections[ 3][30] = 0.0189413;
  m_photon_pt_corrections[ 4][30] = 0.0221763;
  m_photon_pt_corrections[ 5][30] = 0.0251387;
  m_photon_pt_corrections[ 6][30] = 0.0244524;
  m_photon_pt_corrections[ 7][30] = 0.028928 ;
  m_photon_pt_corrections[ 8][30] = 0.0373025;
  m_photon_pt_corrections[ 9][30] = 0.044983 ;
  m_photon_pt_corrections[10][30] = 0.0450644;

  m_photon_pt_corrections[ 0][35] = 0.0150418; 
  m_photon_pt_corrections[ 1][35] = 0.0144198;
  m_photon_pt_corrections[ 2][35] = 0.0144512;
  m_photon_pt_corrections[ 3][35] = 0.0190654;
  m_photon_pt_corrections[ 4][35] = 0.0223608;
  m_photon_pt_corrections[ 5][35] = 0.0254074;
  m_photon_pt_corrections[ 6][35] = 0.0246607;
  m_photon_pt_corrections[ 7][35] = 0.0291474;
  m_photon_pt_corrections[ 8][35] = 0.0376543;
  m_photon_pt_corrections[ 9][35] = 0.0455282;
  m_photon_pt_corrections[10][35] = 0.0452159;

  m_photon_pt_corrections[ 0][40] = 0.0150641; 
  m_photon_pt_corrections[ 1][40] = 0.0144416;
  m_photon_pt_corrections[ 2][40] = 0.014464 ;
  m_photon_pt_corrections[ 3][40] = 0.0191335;
  m_photon_pt_corrections[ 4][40] = 0.0224708;
  m_photon_pt_corrections[ 5][40] = 0.0256225;
  m_photon_pt_corrections[ 6][40] = 0.0248054;
  m_photon_pt_corrections[ 7][40] = 0.029223 ;
  m_photon_pt_corrections[ 8][40] = 0.0378147;
  m_photon_pt_corrections[ 9][40] = 0.0457925;
  m_photon_pt_corrections[10][40] = 0.0457752;



  m_electron_pt_corrections[ 0][10] =  0.00840523; 
  m_electron_pt_corrections[ 1][10] =  0.00759521;
  m_electron_pt_corrections[ 2][10] =  0.00702451;
  m_electron_pt_corrections[ 3][10] =  0.00861699;
  m_electron_pt_corrections[ 4][10] =  0.0101184 ;
  m_electron_pt_corrections[ 5][10] = -0.00878057;  // crack region - low statistics
  m_electron_pt_corrections[ 6][10] =  0.00944832;
  m_electron_pt_corrections[ 7][10] =  0.0128942 ;
  m_electron_pt_corrections[ 8][10] =  0.0147304 ;
  m_electron_pt_corrections[ 9][10] =  0.0151534 ;
  m_electron_pt_corrections[10][10] =  0.0148789 ;

  m_electron_pt_corrections[ 0][15] = 0.0134659;
  m_electron_pt_corrections[ 1][15] = 0.0127816;
  m_electron_pt_corrections[ 2][15] = 0.0128006;
  m_electron_pt_corrections[ 3][15] = 0.0165686;
  m_electron_pt_corrections[ 4][15] = 0.0209503;
  m_electron_pt_corrections[ 5][15] = 0.0193797;
  m_electron_pt_corrections[ 6][15] = 0.0201402;
  m_electron_pt_corrections[ 7][15] = 0.0240031;
  m_electron_pt_corrections[ 8][15] = 0.0297296;
  m_electron_pt_corrections[ 9][15] = 0.0336832;
  m_electron_pt_corrections[10][15] = 0.0395363;

  m_electron_pt_corrections[ 0][20] = 0.0146779;
  m_electron_pt_corrections[ 1][20] = 0.0140476;
  m_electron_pt_corrections[ 2][20] = 0.0142736;
  m_electron_pt_corrections[ 3][20] = 0.0187919;
  m_electron_pt_corrections[ 4][20] = 0.0242244;
  m_electron_pt_corrections[ 5][20] = 0.0327271;
  m_electron_pt_corrections[ 6][20] = 0.0236067;
  m_electron_pt_corrections[ 7][20] = 0.0273382;
  m_electron_pt_corrections[ 8][20] = 0.0347824;
  m_electron_pt_corrections[ 9][20] = 0.0405783;
  m_electron_pt_corrections[10][20] = 0.0488965;

  m_electron_pt_corrections[ 0][25] = 0.0150446; 
  m_electron_pt_corrections[ 1][25] = 0.0144188;
  m_electron_pt_corrections[ 2][25] = 0.0147615;
  m_electron_pt_corrections[ 3][25] = 0.0195745;
  m_electron_pt_corrections[ 4][25] = 0.0253666;
  m_electron_pt_corrections[ 5][25] = 0.0377775;
  m_electron_pt_corrections[ 6][25] = 0.0250662;
  m_electron_pt_corrections[ 7][25] = 0.028478 ;
  m_electron_pt_corrections[ 8][25] = 0.0367565;
  m_electron_pt_corrections[ 9][25] = 0.0437064;
  m_electron_pt_corrections[10][25] = 0.0533984;

  m_electron_pt_corrections[ 0][30] = 0.0151632; 
  m_electron_pt_corrections[ 1][30] = 0.0145416;
  m_electron_pt_corrections[ 2][30] = 0.0149607;
  m_electron_pt_corrections[ 3][30] = 0.0198783;
  m_electron_pt_corrections[ 4][30] = 0.0258172;
  m_electron_pt_corrections[ 5][30] = 0.0404185;
  m_electron_pt_corrections[ 6][30] = 0.0257027;
  m_electron_pt_corrections[ 7][30] = 0.0289142;
  m_electron_pt_corrections[ 8][30] = 0.0375322;
  m_electron_pt_corrections[ 9][30] = 0.0451606;
  m_electron_pt_corrections[10][30] = 0.0555181;

  m_electron_pt_corrections[ 0][35] = 0.0152574; 
  m_electron_pt_corrections[ 1][35] = 0.0145853;
  m_electron_pt_corrections[ 2][35] = 0.0150267;
  m_electron_pt_corrections[ 3][35] = 0.0200184;
  m_electron_pt_corrections[ 4][35] = 0.0260585;
  m_electron_pt_corrections[ 5][35] = 0.0421324;
  m_electron_pt_corrections[ 6][35] = 0.0260337;
  m_electron_pt_corrections[ 7][35] = 0.0291783;
  m_electron_pt_corrections[ 8][35] = 0.0378692;
  m_electron_pt_corrections[ 9][35] = 0.0458775;
  m_electron_pt_corrections[10][35] = 0.0565319;

  m_electron_pt_corrections[ 0][40] = 0.0152929; 
  m_electron_pt_corrections[ 1][40] = 0.0146146;
  m_electron_pt_corrections[ 2][40] = 0.0150541;
  m_electron_pt_corrections[ 3][40] = 0.0200918;
  m_electron_pt_corrections[ 4][40] = 0.0261464;
  m_electron_pt_corrections[ 5][40] = 0.043179 ;
  m_electron_pt_corrections[ 6][40] = 0.0262671;
  m_electron_pt_corrections[ 7][40] = 0.0292846;
  m_electron_pt_corrections[ 8][40] = 0.0380784;
  m_electron_pt_corrections[ 9][40] = 0.0462397;
  m_electron_pt_corrections[10][40] = 0.056945 ;

  return StatusCode::SUCCESS;
}


// finalize
StatusCode PAUcaloIsolationTool::finalize(){
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}


void PAUcaloIsolationTool::handle(const Incident& inc) {
  MsgStream log(msgSvc(), name());

  log << MSG::VERBOSE << "Got informed of incident: " << inc.type()
      << " generated by: " << inc.source() << endreq;

  m_area_corrections_done = false;
}

float PAUcaloIsolationTool::EtConeCorrectionPt_new(const egamma* eg, float /*dR*/) const {
  const EMShower* shower = eg->detail<EMShower>();
  if(!shower) return 0.;

  //float etap = shower->parameter(egammaParameters::etap);

  //return m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg, etap, dR);
  MsgStream log(msgSvc(), name());
  log <<  MSG::ERROR << "... pt corrections need to be migrated to xAOD ..." << endreq ;
  return 0;
}

float PAUcaloIsolationTool::EtConeCorrectionPt_old(const Analysis::Photon* eg, float dR) const {
  float correction = 0.0;

  double eta = eg->eta();
  int eta_bin = GetEtaBin(eta);
  if(eta_bin >= (int)m_eta_bins.size()) return correction;

  // in case people pass in .1 instead of 10
  if(dR < 1) dR *= 100;

  // make it an integer
  int dR_int = (int)dR;

  std::map<int,std::map<int,float> >::const_iterator m_iter = m_photon_pt_corrections.find(eta_bin);
  if(m_iter == m_photon_pt_corrections.end()) return correction;

  std::map<int, float>::const_iterator mm_iter = (m_iter->second).find(dR_int);
  if(mm_iter == (m_iter->second).end()) return correction;

  correction = mm_iter->second;

  correction *= eg->pt();

  /*
  std::cout << "Photon with pt=" << std::setw(10) << eg->pt() << ", eta = " << std::setw(10) << eg->eta()
	    << ", gets correction = " << std::setw(10) << mm_iter->second << " * pT = " << std::setw(10) << correction
	    << " for cone with dR = " << dR << std::endl;
  */

  return correction;
}
float PAUcaloIsolationTool::EtConeCorrectionPt(const Analysis::Photon* eg, float dR) const {
  if(m_useOldLeakageCorrections) return EtConeCorrectionPt_old(eg, dR);
  return EtConeCorrectionPt_new(eg, dR);
}

float PAUcaloIsolationTool::EtConeCorrectionPt(const egamma* eg, float dR) const {
  const Analysis::Photon* ph = dynamic_cast<const Analysis::Photon*>(eg);
  if(ph){
    if(m_useOldLeakageCorrections) return EtConeCorrectionPt_old(ph, dR);
    return EtConeCorrectionPt_new(ph, dR);
  }
  
  const Analysis::Electron* el = dynamic_cast<const Analysis::Electron*>(eg);
  if(el){
    if(m_useOldLeakageCorrections) return EtConeCorrectionPt_old(el, dR);
    return EtConeCorrectionPt_new(el, dR);
  }

  return -999999.;
  
}


float PAUcaloIsolationTool::EtConeCorrectionPt_old(const Analysis::Electron* eg, float dR) const {
  float correction = 0.0;

  double eta = eg->eta();
  int eta_bin = GetEtaBin(eta);
  if(eta_bin >= (int)m_eta_bins.size()) return correction;

  // in case people pass in .1 instead of 10
  if(dR < 1) dR *= 100;

  // make it an integer
  int dR_int = (int)dR;

  std::map<int,std::map<int,float> >::const_iterator m_iter = m_electron_pt_corrections.find(eta_bin);
  if(m_iter == m_electron_pt_corrections.end()) return correction;

  std::map<int, float>::const_iterator mm_iter = (m_iter->second).find(dR_int);
  if(mm_iter == (m_iter->second).end()) return correction;

  correction = mm_iter->second;

  correction *= eg->pt();

  /*
  std::cout << "Electron with pt=" << std::setw(10) << eg->pt() << ", eta = " << std::setw(10) << eg->eta()
	    << ", gets correction = " << std::setw(10) << mm_iter->second << " * pT = " << std::setw(10) << correction
	    << " for cone with dR = " << dR << std::endl;
  */

  return correction;
}
float PAUcaloIsolationTool::EtConeCorrectionPt(const Analysis::Electron* eg, float dR) const {
  if(m_useOldLeakageCorrections) return EtConeCorrectionPt_old(eg, dR);
  return EtConeCorrectionPt_new(eg, dR);
}


float PAUcaloIsolationTool::EtConeCorrectionJetAreas(float eta, float dR, int jetsToIgnore) {
  float  area      = TMath::Pi()*dR*dR;
  int neta = 5, nphi = 7;
  float deta = 0.0125*neta*2;
  float dphi = M_PI/256.*nphi*2;
  area -= (deta * dphi);

  double ptDensity = EnergyDensityMedian(eta, jetsToIgnore);
  double correction= ptDensity * area;

  return correction;
}

float PAUcaloIsolationTool::EtConeCorrectionJetAreas(const egamma* eg, float dR, int jetsToIgnore) {
  return EtConeCorrectionJetAreas(eg->eta(), dR, jetsToIgnore);
}

float PAUcaloIsolationTool::EtConeCorrectionJetAreas(const TruthParticle* tp, float dR, int jetsToIgnore) {
  return EtConeCorrectionJetAreas(tp->eta(), dR, jetsToIgnore);
}

double PAUcaloIsolationTool::EnergyDensityMedian(float eta, int jetsToIgnore) {
  if(!m_do_area_corrections)
    return 0.;

  MsgStream log(msgSvc(), name());
  
  if(m_useOldAreaCorrections){
    if (!m_area_corrections_done || jetsToIgnore != m_previous_jetsToIgnore){
      StatusCode sc = this->CalculateEventBackground(jetsToIgnore);
      if ( sc.isFailure() ){
	log << MSG::WARNING << "Could not execute CalculateEventBackground." << endreq;
	return 0.;
      }
    }

    eta = fabs(eta);

    int    etaBin    = GetEtaBin(eta,true);
    double ptDensity = ((int)m_ptDensity.size()>etaBin ? m_ptDensity[etaBin]:0);

    return ptDensity;
  }
  else{
    EnergyDensityResults edr = CalculateEventBackground(jetsToIgnore, eta, m_eta_range);
    return edr.median;
  }
}

double PAUcaloIsolationTool::EnergyDensityMedian(const egamma* eg, int jetsToIgnore) {
  return EnergyDensityMedian(eg->eta(), jetsToIgnore);
}

double PAUcaloIsolationTool::EnergyDensityMedian(const TruthParticle* tp, int jetsToIgnore) {
  return EnergyDensityMedian(tp->eta(), jetsToIgnore);
}


double PAUcaloIsolationTool::EnergyDensitySigma(float eta, int jetsToIgnore) {
  if(!m_do_area_corrections)
    return 0.;

  MsgStream log(msgSvc(), name());
  
  if(m_useOldAreaCorrections){
    if (!m_area_corrections_done || jetsToIgnore != m_previous_jetsToIgnore){
      StatusCode sc = this->CalculateEventBackground(jetsToIgnore);
      if ( sc.isFailure() ){
	log << MSG::WARNING << "Could not execute CalculateEventBackground." << endreq;
	return 0.;
      }
    }

    eta = fabs(eta);
    int    etaBin    = GetEtaBin(eta,true);
    double sigma     = ((int)m_sigma.size()>etaBin ? m_sigma[etaBin]:0);

    return sigma;
  }
  else{
    EnergyDensityResults edr = CalculateEventBackground(jetsToIgnore, eta, m_eta_range);
    return edr.sigma;
  }
}

double PAUcaloIsolationTool::EnergyDensitySigma(const egamma* eg, int jetsToIgnore) {
  return EnergyDensitySigma(eg->eta(), jetsToIgnore);
}

double PAUcaloIsolationTool::EnergyDensitySigma(const TruthParticle* tp, int jetsToIgnore) {
  return EnergyDensitySigma(tp->eta(), jetsToIgnore);
}

int PAUcaloIsolationTool::EnergyDensityNjets(float eta, int jetsToIgnore) {
  if(!m_do_area_corrections)
    return 0;

  MsgStream log(msgSvc(), name());
  
  if(m_useOldAreaCorrections){
    if (!m_area_corrections_done || jetsToIgnore != m_previous_jetsToIgnore){
      StatusCode sc = this->CalculateEventBackground(jetsToIgnore);
      if ( sc.isFailure() ){
	log << MSG::WARNING << "Could not execute CalculateEventBackground." << endreq;
	return 0;
      }
    }

    eta = fabs(eta);
    int    etaBin      = GetEtaBin(eta,true);
    int    Njets       = ((int)m_Njets.size()>etaBin ? m_Njets[etaBin]:0);
    return Njets;
  }
  else{
    EnergyDensityResults edr = CalculateEventBackground(jetsToIgnore, eta, m_eta_range);
    return edr.nJets;
  }
}

int PAUcaloIsolationTool::EnergyDensityNjets(const egamma* eg, int jetsToIgnore) {
  return EnergyDensityNjets(eg->eta(), jetsToIgnore);
}

int PAUcaloIsolationTool::EnergyDensityNjets(const TruthParticle* tp, int jetsToIgnore) {
  return EnergyDensityNjets(tp->eta(), jetsToIgnore);
}

int PAUcaloIsolationTool::GetEtaBin(double eta_w, bool area_eta) const {
  double eta = fabs(eta_w);

  int v_iter=0;
  if(!area_eta){
    for(v_iter=0; v_iter < (int)m_eta_bins.size()-1; v_iter++){
      if(eta >= m_eta_bins.at(v_iter) && eta < m_eta_bins.at(v_iter+1))
	break;
    }
  }
  else{
    for(v_iter=0; v_iter < (int)m_eta_bins_areaoffset.size()-1; v_iter++){
      if(eta >= m_eta_bins_areaoffset.at(v_iter) && eta < m_eta_bins_areaoffset.at(v_iter+1))
	break;
    }
  }

  return v_iter;
}


StatusCode PAUcaloIsolationTool::CalculateEventBackground(int jetsToIgnore) {

  MsgStream log(msgSvc(), name());

  if(!evtStore()->contains<JetCollection>(m_jet_collection_name)){
    log << MSG::INFO << m_jet_collection_name << " jet container not found in TDS.  Aborting." << endreq;
    return StatusCode::SUCCESS;
  }

  bool m_useUncalibrated=true;

  //Clear vector that stores the event pTDensities (in eta bins)
  m_ptDensity.clear();
  m_sigma.clear();
  m_Njets.clear();
  //===== Compute the event pT Density =====
  // Get special low pt jet collection  (THESE JETS SHOULD NOT BE SCHEDULED FOR THE OFFSET CORRECTION)
  const JetCollection*  jets = 0;
  if (evtStore()->retrieve(jets, m_jet_collection_name).isFailure() ) {
    log << MSG::WARNING << m_jet_collection_name << " jet container NOT found in TDS" << endreq; 
    return StatusCode::SUCCESS;
  }

  // For each jet compute its pT density and store in vector for this event
  std::vector< std::vector<double> > ptDensities;
  std::vector<double> emptyVec;
  ptDensities.assign(m_eta_bins_areaoffset.size()-1,emptyVec);
  JetCollection::const_iterator jetItr  = jets->begin();
  JetCollection::const_iterator jetItrE = jets->end();

  // if we want to ignore some of the higher pT jets in the event, then figure out which jets
  // those are here.  this map will store the jets in decreasing order of pT.
  std::multimap<float,const Jet*, CxxUtils::fpcompare_fn::greater > jet_pt_map;
  for (jetItr = jets->begin();jetItr!=jetItrE;jetItr++){
    float jet_pt = (*jetItr)->pt();
    jet_pt_map.insert(std::make_pair(jet_pt, *jetItr));
  }

  // now, remove everything in the map except for the highest pT jets.
  std::multimap<float,const Jet*>::iterator jpmi = jet_pt_map.begin();
  for(int i=0; i<jetsToIgnore && jpmi!=jet_pt_map.end(); i++) ++jpmi;
  jet_pt_map.erase(jpmi, jet_pt_map.end());

  for (jetItr = jets->begin();jetItr!=jetItrE;jetItr++)
    {

      // we've got a map that's got the highest pT jets in it - if the jet we're
      // looking at is one of those jets, then don't store its pT density.
      bool ignore=false;
      for(jpmi = jet_pt_map.begin(); jpmi != jet_pt_map.end(); ++jpmi){
	if(jpmi->second == *jetItr){
	  ignore = true;
	  break;
	}
      }
      
      if(ignore) continue;

      double jet_area = 0.0;
      double jet_pt   = 0.0;
      double jet_eta  = 0.0;

      // By default this correction is run at the EM-scale (but this is configurable)
      //P4SignalState::State s = (*jetItr)->signalState();
      if(m_jet_collection_name.find("Truth") == std::string::npos){
	SignalStateHelper sigstateH ( P4SignalState::JETEMSCALE );
	if(m_useUncalibrated)  
	  {
	    sigstateH.controlObject( *jetItr);
	  }
	jet_area = (*jetItr)->getMoment(m_jetAreaMomentName);
	jet_pt   = (*jetItr)->pt();
	jet_eta   = (*jetItr)->eta();
	// return to original state (do it explicetely here)
	sigstateH.releaseObject();
      }
      else{
	jet_area = (*jetItr)->getMoment(m_jetAreaMomentName);
	jet_pt   = (*jetItr)->pt();
	jet_eta   = (*jetItr)->eta();
      }

      if(jet_area>10e-4 && fabs(jet_eta) < m_eta_bins_areaoffset[m_eta_bins_areaoffset.size()-1])
	{
	  log << MSG::DEBUG << "Event pT density algorithm: " << "jet pt = " << jet_pt << ", jet area = " << jet_area << "\t pt=" << jet_pt << "\t eta=" << jet_eta << endreq;
	  ptDensities.at(GetEtaBin(fabs(jet_eta),true)).push_back(jet_pt/jet_area);
	}
      else
	{
	  log << MSG::DEBUG << "Event pT density algorithm: cannot calculate jet density, area = " << jet_area << "\t pt=" << jet_pt << "\t eta=" << jet_eta << endreq;
	}
    }


  // get median of ptDensities
  for(int b=0;b< (int)m_eta_bins_areaoffset.size()-1;b++)
    {
      double median = 0.0;
      double sigma = 0.0;
      int Njets = 0;
      if(ptDensities[b].size() > 0)
	{
	  std::sort(ptDensities[b].begin(), ptDensities[b].end());
	  int nDens = ptDensities[b].size();
	  if( nDens%2 == 0 )
	    median = (ptDensities[b][nDens/2]+ptDensities[b][(nDens-2)/2])/2;
	  else
	    median = ptDensities[b][(nDens-1)/2];
	  sigma = ptDensities[b][(int)(.15865*nDens)];
	  Njets = nDens;
	}
      m_ptDensity.push_back(median);
      m_sigma.push_back(sigma);
      m_Njets.push_back(Njets);
      log << MSG::DEBUG << "Median pT Density for Eta Bin #" << b << " = " << median << ", with sigma = " << sigma << " and Njets = " << Njets << endreq; 
    }
  
  m_area_corrections_done = true;
  m_previous_jetsToIgnore = jetsToIgnore;

  return StatusCode::SUCCESS;
}


EnergyDensityResults PAUcaloIsolationTool::CalculateEventBackground(int jetsToIgnore, float eta, float eta_range) {

  MsgStream log(msgSvc(), name());

  EnergyDensityResults return_results;
  return_results.median = 0.0;
  return_results.sigma = 0.0;
  return_results.nJets = 0;

  if(!evtStore()->contains<JetCollection>(m_jet_collection_name)){
    log << MSG::INFO << m_jet_collection_name << " jet container not found in TDS.  Aborting." << endreq;
    return return_results;
  }

  bool m_useUncalibrated=true;

  //===== Compute the event pT Density =====
  // Get special low pt jet collection  (THESE JETS SHOULD NOT BE SCHEDULED FOR THE OFFSET CORRECTION)
  const JetCollection*  jets = 0;
  if (evtStore()->retrieve(jets, m_jet_collection_name).isFailure() ) {
    log << MSG::WARNING << m_jet_collection_name << " jet container NOT found in TDS" << endreq; 
    return return_results;
  }

  // For each jet compute its pT density and store in vector for this event
  std::vector<double> ptDensities;
  JetCollection::const_iterator jetItr  = jets->begin();
  JetCollection::const_iterator jetItrE = jets->end();

  // if we want to ignore some of the higher pT jets in the event, then figure out which jets
  // those are here.  this map will store the jets in decreasing order of pT.
  std::multimap<float,const Jet*, CxxUtils::fpcompare_fn::greater > jet_pt_map;
  for (jetItr = jets->begin();jetItr!=jetItrE;jetItr++){
    float jet_pt = (*jetItr)->pt();
    jet_pt_map.insert(std::make_pair(jet_pt, *jetItr));
  }

  // now, remove everything in the map except for the highest pT jets.
  // this map will be used to reject jets later on
  std::multimap<float,const Jet*>::iterator jpmi = jet_pt_map.begin();
  for(int i=0; i<jetsToIgnore && jpmi != jet_pt_map.end(); i++) ++jpmi;
  jet_pt_map.erase(jpmi, jet_pt_map.end());

  for (jetItr = jets->begin();jetItr!=jetItrE;jetItr++)
    {

      // we've got a map that's got the highest pT jets in it - if the jet we're
      // looking at is one of those jets, then don't store its pT density.
      bool ignore=false;
      for(jpmi = jet_pt_map.begin(); jpmi != jet_pt_map.end(); ++jpmi){
	if(jpmi->second == *jetItr){
	  ignore = true;
	  break;
	}
      }
      
      if(ignore) continue;

      double jet_area = 0.0;
      double jet_pt   = 0.0;
      double jet_eta  = 0.0;

      // By default this correction is run at the EM-scale (but this is configurable)
      //P4SignalState::State s = (*jetItr)->signalState();
      if(m_jet_collection_name.find("Truth") == std::string::npos){
	SignalStateHelper sigstateH ( P4SignalState::JETEMSCALE );
	if(m_useUncalibrated)  
	  {
	    sigstateH.controlObject( *jetItr);
	  }
	jet_area = (*jetItr)->getMoment(m_jetAreaMomentName);
	jet_pt   = (*jetItr)->pt();
	jet_eta   = (*jetItr)->eta();
	// return to original state (do it explicetely here)
	sigstateH.releaseObject();
      }
      else{
	jet_area = (*jetItr)->getMoment(m_jetAreaMomentName);
	jet_pt   = (*jetItr)->pt();
	jet_eta   = (*jetItr)->eta();
      }

      if(jet_area>10e-4 && fabs(eta-jet_eta) < fabs(eta_range))
	{
	  log << MSG::DEBUG << "Event pT density algorithm: " << "jet pt = " << jet_pt << ", jet area = " << jet_area << "\t pt=" << jet_pt << "\t eta=" << jet_eta << endreq;
	  ptDensities.push_back(jet_pt/jet_area);
	}
      else
	{
	  log << MSG::DEBUG << "Event pT density algorithm: cannot calculate jet density, area = " << jet_area << "\t pt=" << jet_pt << "\t eta=" << jet_eta << endreq;
	}
    }


  // get median of ptDensities

  if(ptDensities.size() > 0){
    std::sort(ptDensities.begin(), ptDensities.end());
    int nDens = ptDensities.size();
    if( nDens%2 == 0 )
      return_results.median = (ptDensities[nDens/2]+ptDensities[(nDens-2)/2])/2;
    else
      return_results.median = ptDensities[(nDens-1)/2];
    return_results.sigma = ptDensities[(int)(.15865*nDens)];
    return_results.nJets = nDens;
  }

  log << MSG::DEBUG 
      << "Median pT Density for object with eta = " << eta << " = " << return_results.median 
      << ", with sigma = " << return_results.sigma 
      << " and Njets = " << return_results.nJets << endreq; 
  
  return return_results;
}
