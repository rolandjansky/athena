/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauCalibrateLC.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"

#define GeV 1000

/********************************************************************/
TauCalibrateLC::TauCalibrateLC(const std::string& name) :
  TauRecToolBase(name) {
  declareProperty("calibrationFile", m_calibrationFile = "");
  declareProperty("doEnergyCorrection", m_doEnergyCorr = false);
  declareProperty("doPtResponse", m_doPtResponse = false);
  declareProperty("countOnlyPileupVertices", m_countOnlyPileupVertices = false);
  declareProperty("doAxisCorrection", m_doAxisCorr = false);
  declareProperty("usePantauAxis", m_usePantauAxis = false);
  declareProperty("isCaloOnly", m_isCaloOnly = false);
}

/********************************************************************/
TauCalibrateLC::~TauCalibrateLC() {
}

/********************************************************************/
StatusCode TauCalibrateLC::initialize() {

  if (inTrigger()) {
    ATH_CHECK( m_aveIntPerXKey.initialize() );
  }
  else {
    ATH_CHECK( m_vertexInputContainer.initialize() );
  }
  
  std::string fullPath = find_file(m_calibrationFile);

  std::unique_ptr<TFile> file(TFile::Open(fullPath.c_str(), "READ"));

  if (!file) {
    ATH_MSG_FATAL("Failed to open " << fullPath);
    return StatusCode::FAILURE;
  }

  // get the histogram defining eta binning
  std::string key = "etaBinning";
  TH1* histo = dynamic_cast<TH1*>(file->Get(key.c_str()));
  if (histo) {
    histo->SetDirectory(0);
    m_etaBinHist = std::unique_ptr<TH1>(histo);
  }
  else {
    ATH_MSG_FATAL("Failed to get an object with  key " << key);
    return StatusCode::FAILURE;
  }

  //retrieve number of eta bins from file
  m_nEtaBins = m_etaBinHist->GetNbinsX(); //member var
  if (m_nEtaBins==6)
    ATH_MSG_INFO("using 2011 tau energy calibration");
  else if (m_nEtaBins==5)
    ATH_MSG_INFO("using 2012 tau energy calibration");
  else {
    ATH_MSG_FATAL("Wrong or broken tau energy calibration file");
    return StatusCode::FAILURE;
  }
    
  // get the histogram with eta corrections
  key = "etaCorrection";
  histo = dynamic_cast<TH1*>(file->Get(key.c_str()));
  if (histo) {
    histo->SetDirectory(0);
    m_etaCorrectionHist = std::unique_ptr<TH1>(histo);
  }
  else {
    ATH_MSG_FATAL("Failed to get an object with  key " << key);
    return StatusCode::FAILURE;
  }

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
      histo->SetDirectory(0);
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
  m_minNTrackAtVertex = static_cast<unsigned int> (m_slopeNPVHist[0]->GetBinContent(m_slopeNPVHist[0]->GetNbinsX() + 1)); //overflow is the minimum number of tracks at vertex 

  ATH_MSG_DEBUG("averageNPV                                 = " << m_averageNPV);
  ATH_MSG_DEBUG("minimum number of tracks at primary vertex = " << m_minNTrackAtVertex);

  file->Close();

  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauCalibrateLC::execute(xAOD::TauJet& pTau) const
{ 
  // energy calibration depends on number of tracks - 1p or Mp
  int prongBin = 1; //Mp
  if (pTau.nTracks() <= 1) prongBin = 0; //1p

  // set tau energy scale 
  if (m_doEnergyCorr) {

    // get detector axis values
    double eta = pTau.etaDetectorAxis();
    double absEta = std::abs(eta);
    int etaBin = m_etaBinHist->GetXaxis()->FindBin(absEta) - 1;
        
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
    else { // offline: count the primary vertex container
      SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexInputContainer );
      if (!vertexInHandle.isValid()) {
        ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << vertexInHandle.key());
        return StatusCode::FAILURE;
      }
      const xAOD::VertexContainer * vxContainer = vertexInHandle.cptr();
      for (const auto vertex : *vxContainer) {
        if (m_countOnlyPileupVertices && vertex->vertexType() == xAOD::VxType::PileUp) {
          ++nVertex;
        }
        else if (!m_countOnlyPileupVertices && vertex->nTrackParticles() >= m_minNTrackAtVertex) {
          ++nVertex;
        }
      } 
      ATH_MSG_DEBUG("calculated nVertex " << nVertex );           
    } 
    
    double calibConst = 1.0;

    double slopeNPV = m_slopeNPVHist[prongBin]->GetBinContent(etaBin + 1);
    double offset = slopeNPV * (nVertex - m_averageNPV);

    // energy response parameterized as a function of pileup-corrected E_LC
    double energyLC = pTau.p4(xAOD::TauJetParameters::DetectorAxis).E() / GeV; 
    if(m_doPtResponse) energyLC = pTau.ptDetectorAxis() / GeV;

    if (energyLC <= 0) {
      ATH_MSG_DEBUG("tau energy at LC scale is " << energyLC << "--> set energy=0.001");           
      pTau.setP4(0.001, pTau.eta(), pTau.phi(), pTau.m());
      return StatusCode::SUCCESS;
    }

    if (energyLC - offset <= 0) {
      ATH_MSG_DEBUG("after pile-up correction energy would be = " << energyLC - offset << " --> setting offset=0 now!");
      offset = 0;
    }
      
    // apply offset correction
    double energyPileupCorr = energyLC - offset;
      
    if (energyPileupCorr > 0 and energyPileupCorr < 10000) // from 0 to 10 TeV
      {
	calibConst = m_calibFunc[prongBin][etaBin]->Eval(energyPileupCorr);
	  
	if (calibConst <= 0) {
	  ATH_MSG_DEBUG("calibration constant = " << calibConst);
	  ATH_MSG_DEBUG("prongBin             = " << prongBin);
	  ATH_MSG_DEBUG("etaBin               = " << etaBin);
	  ATH_MSG_DEBUG("energyPileupCorr     = " << energyPileupCorr);
	  ATH_MSG_DEBUG("energyLC             = " << energyLC);
	  calibConst = 1.0;
	}
      }
      
    double energyFinal = energyPileupCorr / calibConst;
      
    if (not m_doPtResponse) energyFinal /= cosh(pTau.eta()) ; 
    pTau.setP4( energyFinal * GeV, pTau.eta(), pTau.phi(), pTau.m());

    ATH_MSG_DEBUG("Energy at LC scale = " << energyLC << " pile-up offset " << offset << " calib. const. = " << calibConst << " final energy = " << energyFinal);

    pTau.setP4(xAOD::TauJetParameters::TauEnergyScale, pTau.pt(), pTau.eta(), pTau.phi(), pTau.m());      
    pTau.setDetail(xAOD::TauJetParameters::TESCalibConstant, static_cast<float>( calibConst ) );
    pTau.setDetail(xAOD::TauJetParameters::TESOffset, static_cast<float>( offset * GeV ) );      

  }
    
  // final tau axis
  if (m_doAxisCorr) {

    // get tau intermediate axis values
    double eta = pTau.etaIntermediateAxis();
    double absEta = std::abs(eta);
    double etaCorr = eta;
    
    double phi = pTau.phiIntermediateAxis();
    double phiCorr = phi;

    // TauCalibrateLC should then only be called after Pantau !!
    if(m_usePantauAxis && std::abs(pTau.etaPanTauCellBased()) < 111) {      
      etaCorr = pTau.etaPanTauCellBased();
      phiCorr = pTau.phiPanTauCellBased();      
    }
    else if (absEta) {
      etaCorr = (eta / absEta)*(absEta - m_etaCorrectionHist->GetBinContent(m_etaCorrectionHist->GetXaxis()->FindBin(absEta)));
    }      

    ATH_MSG_DEBUG("eta " << eta << "; corrected eta = " << etaCorr << " ; phi " << phi << "; corrected phi " << phiCorr );
    pTau.setP4( pTau.e() / cosh( etaCorr ), etaCorr, phiCorr, pTau.m());
    pTau.setP4(xAOD::TauJetParameters::TauEtaCalib, pTau.pt(), pTau.eta(), pTau.phi(), pTau.m());
  }

  if (m_isCaloOnly == true && inTrigger()) {
    pTau.setP4(xAOD::TauJetParameters::TrigCaloOnly, pTau.pt(), pTau.eta(), pTau.phi(), pTau.m());
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauCalibrateLC::finalize() {
  return StatusCode::SUCCESS;
}
