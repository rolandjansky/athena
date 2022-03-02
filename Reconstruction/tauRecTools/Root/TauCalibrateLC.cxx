/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauCalibrateLC.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include <utility>

#define GeV 1000

/********************************************************************/
TauCalibrateLC::TauCalibrateLC(const std::string& name) :
  TauRecToolBase(name) {
  declareProperty("calibrationFile", m_calibrationFile = "");
  declareProperty("doPtResponse", m_doPtResponse = true);
  declareProperty("VertexCorrection", m_doVertexCorrection = true);
  declareProperty("isCaloOnly", m_isCaloOnly = false);
}

/********************************************************************/
TauCalibrateLC::~TauCalibrateLC() {
}

/********************************************************************/
StatusCode TauCalibrateLC::initialize() {

  ATH_CHECK( m_aveIntPerXKey.initialize(inTrigger()) );
  ATH_CHECK( m_vertexInputContainer.initialize(!inTrigger()) );
  
  std::string fullPath = find_file(m_calibrationFile);
  ATH_MSG_INFO("Using calibration file: " << fullPath);

  std::unique_ptr<TFile> file(TFile::Open(fullPath.c_str(), "READ"));
  if (!file) {
    ATH_MSG_FATAL("Failed to open " << fullPath);
    return StatusCode::FAILURE;
  }

  // get the histogram defining eta binning
  std::string key = "etaBinning";
  TH1* histo = dynamic_cast<TH1*>(file->Get(key.c_str()));
  if (histo) {
    histo->SetDirectory(nullptr);
    m_etaBinHist = std::unique_ptr<TH1>(histo);
  }
  else {
    ATH_MSG_FATAL("Failed to get an object with  key " << key);
    return StatusCode::FAILURE;
  }

  //retrieve number of eta bins from file
  m_nEtaBins = m_etaBinHist->GetNbinsX();
    
  TString tmpSlopKey[s_nProngBins] = {"slopeNPV1P", "slopeNPV3P"};
  TString tmpFuncBase[s_nProngBins] = {"OneP_Eta_", "MultiP_Eta_"};

  /// m_slopeNPVHist with size of s_nProngBins
  m_slopeNPVHist.resize(s_nProngBins);
  
  /// m_calibFunc with size of (s_nProngBins, m_nEtaBins)
  m_calibFunc.resize(s_nProngBins);
  for (int i = 0; i < s_nProngBins; ++i) {
    m_calibFunc[i].resize(m_nEtaBins);
  }

  for (int i = 0; i < s_nProngBins; i++) {
    histo = dynamic_cast<TH1*>(file->Get(tmpSlopKey[i]));  // get pile-up slope histograms
    if (histo) {
      histo->SetDirectory(nullptr);
      m_slopeNPVHist[i] = std::unique_ptr<TH1>(histo);
    }
    else {
      ATH_MSG_FATAL("Failed to get an object with  key " << tmpSlopKey[i]);
      return StatusCode::FAILURE;
    }

    for (int j = 0; j < m_nEtaBins; j++) {
      TString key = tmpFuncBase[i];
      key += j;
      TF1* fcn = dynamic_cast<TF1*>(file->Get(key));
      if (fcn) {
        m_calibFunc[i][j] = std::unique_ptr<TF1>(fcn);
      }
      else {
        ATH_MSG_FATAL("Failed to get an object with  key " << key);
        return StatusCode::FAILURE;
      }
    }
  }
  m_averageNPV = m_slopeNPVHist[0]->GetBinContent(0); // underflow is the average number of reconstructed primary vertices
  ATH_MSG_DEBUG("averageNPV = " << m_averageNPV);

  file->Close();

  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauCalibrateLC::execute(xAOD::TauJet& tau) const
{ 
  // energy calibration depends on number of tracks - 1p or Mp
  int prongBin = 1; //Mp
  if (tau.nTracks() <= 1) prongBin = 0; //1p

  // get IntermediateAxis or DetectorAxis momentum
  auto tau_p4 = m_doVertexCorrection ? tau.p4(xAOD::TauJetParameters::IntermediateAxis) : tau.p4(xAOD::TauJetParameters::DetectorAxis);

  double absEta = std::abs( tau_p4.Eta() );
  int etaBin = std::as_const(m_etaBinHist)->GetXaxis()->FindBin(absEta) - 1;
        
  if (etaBin>=m_nEtaBins) etaBin = m_nEtaBins-1; // correction from last bin should be applied on all taus outside stored eta range

  int nVertex = 0;
    
  // Obtain pileup
  if (inTrigger())  { // online: retrieved from EventInfo 
    SG::ReadDecorHandle<xAOD::EventInfo, float> eventInfoDecorHandle( m_aveIntPerXKey );
    if (!eventInfoDecorHandle.isPresent()) {
      ATH_MSG_WARNING ( "EventInfo decoration not available! Will set nVertex = " << m_averageNPV );
      nVertex = m_averageNPV;
    }
    else {
      nVertex = eventInfoDecorHandle(0);
      ATH_MSG_DEBUG("AvgInteractions object in tau candidate = " << nVertex);
    } 
  }  
  else { // offline: pileup vertex multiplicity
    SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexInputContainer );
    if (!vertexInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << vertexInHandle.key());
      return StatusCode::FAILURE;
    }
    const xAOD::VertexContainer * vxContainer = vertexInHandle.cptr();
    for (const auto *const vertex : *vxContainer) {
      if (vertex->vertexType() == xAOD::VxType::PileUp) {
	++nVertex;
      }
    } 
    ATH_MSG_DEBUG("calculated nVertex " << nVertex );           
  } 
    
  double calibConst = 1.;

  double slopeNPV = m_slopeNPVHist[prongBin]->GetBinContent(etaBin + 1);
  double offset = slopeNPV * (nVertex - m_averageNPV);

  // pt (energy) response parameterized as a function of pileup-corrected pt (energy)
  double energyLC = m_doPtResponse ? tau_p4.Pt()/GeV : tau_p4.E()/GeV;

  if (energyLC <= 0.) {
    ATH_MSG_DEBUG("tau energy at LC scale is " << energyLC << "--> set energy=0.001");           
    tau.setP4(0.001, tau_p4.Eta(), tau_p4.Phi(), tau.m());
    tau.setP4(xAOD::TauJetParameters::TauEnergyScale, tau.pt(), tau.eta(), tau.phi(), tau.m());
    return StatusCode::SUCCESS;
  }

  if (energyLC - offset <= 0.) {
    ATH_MSG_DEBUG("after pile-up correction energy would be = " << energyLC - offset << " --> setting offset=0 now!");
    offset = 0.;
  }
      
  // apply offset correction
  double energyPileupCorr = energyLC - offset;
      
  if (energyPileupCorr > 0. and energyPileupCorr < 10000.) // from 0 to 10 TeV
    {
      calibConst = m_calibFunc[prongBin][etaBin]->Eval(energyPileupCorr);
	  
      if (calibConst <= 0.) {
	ATH_MSG_DEBUG("calibration constant = " << calibConst);
	ATH_MSG_DEBUG("prongBin             = " << prongBin);
	ATH_MSG_DEBUG("etaBin               = " << etaBin);
	ATH_MSG_DEBUG("energyPileupCorr     = " << energyPileupCorr);
	ATH_MSG_DEBUG("energyLC             = " << energyLC);
	calibConst = 1.;
      }
    }
      
  double energyFinal = energyPileupCorr / calibConst;
      
  if (not m_doPtResponse) energyFinal /= std::cosh(tau_p4.Eta());

  // tau.m() is 0 by convention in tauRecTools
  // while mDetectorAxis and mIntermediateAxis are not forced to 0, mTauEnergyScale is
  tau.setP4( energyFinal * GeV, tau_p4.Eta(), tau_p4.Phi(), tau.m());
  tau.setP4(xAOD::TauJetParameters::TauEnergyScale, tau.pt(), tau.eta(), tau.phi(), tau.m());
  ATH_MSG_DEBUG("Energy at LC scale = " << energyLC << " pile-up offset " << offset << " calib. const. = " << calibConst << " final energy = " << energyFinal);
     
  if (m_isCaloOnly && inTrigger()) {
    tau.setP4(xAOD::TauJetParameters::TrigCaloOnly, tau.pt(), tau.eta(), tau.phi(), tau.m());
  }

  return StatusCode::SUCCESS;
}
