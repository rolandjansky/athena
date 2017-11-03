/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_DigitizationToolBox.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include <algorithm>
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"
#include "GaudiKernel/MsgStream.h"

//--------------------------------------------------------------------------
LUCID_DigitizationToolBox::LUCID_DigitizationToolBox(): 
  m_tubeID(0),
  m_npe   (0),
  m_npeGas(0), 
  m_npePmt(0),
  m_qdc   (0),
  m_tdcTot(0),
  m_tdc   (0)
{
  m_digitContainer = 0;
  m_tubeInfo       = 0;
  
  m_numTubes          = 0;  
  m_qdcChannelsPerPE  = 0;
  m_qdcPedestal       = 0;
  m_qdcFedNoiseFactor = 0;
  m_tdcPmtNoiseFactor = 0;
  m_tdcFedNoiseFactor = 0;

  m_TotalPMTgain      = 0;
  m_AmpFactor         = 0;
  m_Q1bin             = 0;
  m_NoiseCharge       = 0;
  m_numDyinodes       = 0;
  m_dynodeGammaFactor = 0;
  m_gainPerDynode     = 0;
  m_ChargeToQdcFactor = 0;

  m_fillRootTree = 0;
}

//--------------------------------------------------------------------------
LUCID_DigitizationToolBox::LUCID_DigitizationToolBox(int    numTubes,
						     double qdcChannelsPerPE,
						     double qdcPedestal,
						     double qdcFedNoiseFactor,
						     double tdcPmtNoiseFactor,
						     double tdcFedNoiseFactor,
						     double TotalPMTgain,
						     double AmpFactor,
						     double Q1bin,
						     int    NoiseCharge,
						     int    numDyinodes,
						     double dynodeGammaFactor,
						     std::vector<double> pmtSmearing,
						     std::vector<double> pmtScaling,
						     std::vector<double> gasScaling,
						     std::vector<double> npeThreshold,
 						     bool   fillRootTree): 
  m_numTubes            (numTubes),
  m_qdcChannelsPerPE    (qdcChannelsPerPE),
  m_qdcPedestal         (qdcPedestal),
  m_qdcFedNoiseFactor   (qdcFedNoiseFactor),
  m_tdcPmtNoiseFactor   (tdcPmtNoiseFactor),
  m_tdcFedNoiseFactor   (tdcFedNoiseFactor),
  m_TotalPMTgain        (TotalPMTgain),
  m_AmpFactor           (AmpFactor),
  m_Q1bin               (Q1bin),
  m_NoiseCharge         (NoiseCharge),
  m_numDyinodes         (numDyinodes),
  m_dynodeGammaFactor   (dynodeGammaFactor),
  m_pmtSmearing         (pmtSmearing),
  m_pmtScaling          (pmtScaling),
  m_gasScaling          (gasScaling),
  m_npeThreshold        (npeThreshold),
  m_fillRootTree        (fillRootTree),    
  m_tubeID              (0),
  m_npe                 (0),
  m_npeGas              (0), 
  m_npePmt              (0),
  m_qdc                 (0),
  m_tdcTot              (0),
  m_tdc                 (0)
{
  m_gainPerDynode     = std::pow(m_TotalPMTgain/m_dynodeGammaFactor, 1.0/static_cast<double>(m_numDyinodes)); 
  m_ChargeToQdcFactor = (double)1.6e-19 * m_AmpFactor/m_Q1bin;

  m_digitContainer = 0;
  m_tubeInfo       = 0;
}

//--------------------------------------------------------------------------
StatusCode LUCID_DigitizationToolBox::setDebugTree(ITHistSvc* digitHistSvc) {
  
  m_tubeInfo = new TTree("t", "LUCID_LUMI_SUMMARY");
  
  StatusCode sc = digitHistSvc->regTree("/AANT/LUCID_LUMI_SUMMARY", m_tubeInfo);
  
  if (sc.isFailure()) return StatusCode::FAILURE;
  
  m_tubeInfo->Branch("tubeID", &m_tubeID, "tubeID/s");
  m_tubeInfo->Branch("npe"   , &m_npe   , "npe/D");
  m_tubeInfo->Branch("npeGas", &m_npeGas, "npeGas/s");
  m_tubeInfo->Branch("npePmt", &m_npePmt, "npePmt/s");
  m_tubeInfo->Branch("qdc"   , &m_qdc   , "qdc/s");
  m_tubeInfo->Branch("tdc"   , &m_tdc   , "tdc/s");
  
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------
unsigned int LUCID_DigitizationToolBox::roundoff(double x) {
  
  assert(x >= INT_MIN-0.5);
  assert(x <= INT_MAX+0.5);
  
  if (x >= 0) return static_cast<int>(x+0.5);
  
  return static_cast<int>(x-0.5);
}

//--------------------------------------------------------------------------
StatusCode LUCID_DigitizationToolBox::createAndStoreDigit(unsigned short tubeID, CLHEP::HepRandomEngine* rndEngine) {
  
  double npePmt     = m_npePmt * m_pmtScaling[tubeID] * CLHEP::RandGaussQ::shoot(rndEngine, 1, m_pmtSmearing[tubeID]);
  double npeGas     = m_npeGas * m_gasScaling[tubeID];
  double npeTot     = npeGas + npePmt;
  double qdcSmeared = DynodeChainSimulation(npeTot, rndEngine);
    
  m_qdc = roundoff(qdcSmeared);
    
  m_npe = (m_qdc - m_qdcPedestal)/m_qdcChannelsPerPE;

  double tdcTotSmeared = 0;
    
  tdcTotSmeared = CLHEP::RandGaussQ::shoot(rndEngine, m_tdcTot     , m_tdcPmtNoiseFactor);
  tdcTotSmeared = CLHEP::RandGaussQ::shoot(rndEngine, tdcTotSmeared, m_tdcFedNoiseFactor);
  tdcTotSmeared = (npeTot) ? tdcTotSmeared/npeTot : tdcTotSmeared;
  tdcTotSmeared = (tdcTotSmeared < 0) ? 0 : tdcTotSmeared;

  m_tdc = static_cast<unsigned int>(tdcTotSmeared);
    
  if (m_fillRootTree) m_tubeInfo->Fill(); 

  m_digitContainer->push_back(new LUCID_Digit(m_tubeID, 
					      m_npe, 
					      m_npeGas, 
					      m_npePmt, 
					      m_qdc, 
					      m_tdc,
					      m_npe > m_npeThreshold[tubeID]));
  return StatusCode::SUCCESS;
}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_DigitizationToolBox::fillDigitContainer(TimedHitCollection<LUCID_SimHit>& thpclucid, CLHEP::HepRandomEngine* rndEngine) {
  
  for (m_tubeID = 0; m_tubeID < m_numTubes; m_tubeID++) {
    
    if (m_tubeID >= 16 &&  m_tubeID <= 19) continue; // skip fiber tubes on sideA
    if (m_tubeID >= 36 &&  m_tubeID <= 39) continue; // skip fiber tubes on sideC
    
    m_npe = m_npeGas = m_npePmt = m_qdc = m_tdc = m_tdcTot = 0;
    
    TimedHitCollection<LUCID_SimHit>                 thpc = thpclucid;
    TimedHitCollection<LUCID_SimHit>::const_iterator i, e, hitIt;
    
    while (thpc.nextDetectorElement(i, e)) for (hitIt = i; hitIt != e; hitIt++) {
      
      if (m_tubeID != (*hitIt)->GetTubeID()) continue;

      if (!(*hitIt)->isDetected(rndEngine)) continue; // applying quantum efficiency

      if ((*hitIt)->GetGenVolume() == 1) m_npeGas++;
      if ((*hitIt)->GetGenVolume() == 2) m_npeGas++;
      if ((*hitIt)->GetGenVolume() == 3) m_npePmt++;
      
      m_tdcTot += (*hitIt)->GetPreStepTime();
    }
    
    if (createAndStoreDigit(m_tubeID, rndEngine).isFailure()) return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_DigitizationToolBox::fillDigitContainer(LUCID_SimHitCollection* thpclucid, CLHEP::HepRandomEngine* rndEngine) {
      
  for (m_tubeID = 0; m_tubeID < m_numTubes; m_tubeID++) {
    
    if (m_tubeID >= 16 &&  m_tubeID <= 19) continue; // skip fiber tubes on sideA
    if (m_tubeID >= 36 &&  m_tubeID <= 39) continue; // skip fiber tubes on sideC
    
    m_npe = m_npeGas = m_npePmt = m_qdc = m_tdc = m_tdcTot = 0;
        
    LUCID_SimHitCollection::const_iterator hitIt  = thpclucid->begin();
    LUCID_SimHitCollection::const_iterator hitItE = thpclucid->end();

    for (; hitIt != hitItE; hitIt++) {
      
      if (m_tubeID != (*hitIt).GetTubeID()) continue;
      
      if (!(*hitIt).isDetected(rndEngine)) continue; // applying quantum efficiency

      if ((*hitIt).GetGenVolume() == 1) m_npeGas++;
      if ((*hitIt).GetGenVolume() == 2) m_npeGas++;
      if ((*hitIt).GetGenVolume() == 3) m_npePmt++;
      
      m_tdcTot += (*hitIt).GetPreStepTime();
    }

    if (createAndStoreDigit(m_tubeID, rndEngine).isFailure()) return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------
StatusCode LUCID_DigitizationToolBox::recordContainers(ServiceHandle<StoreGateSvc> digitsStore, std::string key_digitCnt) {
  
  m_digitContainer = new LUCID_DigitContainer();

  StatusCode sc = digitsStore->record(m_digitContainer, key_digitCnt);
  
  return sc;
}

//--------------------------------------------------------------------------
double LUCID_DigitizationToolBox::DynodeChainSimulation(double npe, CLHEP::HepRandomEngine* rndEngine){
  
  double smearedQDC = 0;
  
  if (npe == 0) return CLHEP::RandGaussQ::shoot(rndEngine, npe, m_ChargeToQdcFactor * m_NoiseCharge) + m_qdcPedestal;
  
  for (int i_pe=1; i_pe <= m_numDyinodes; i_pe++ ) {
    
    if (i_pe == 1) smearedQDC = DynodeGainSmearing(npe        * m_gainPerDynode * m_dynodeGammaFactor, rndEngine);  
    else           smearedQDC = DynodeGainSmearing(smearedQDC * m_gainPerDynode                      , rndEngine);  
  }
  
  smearedQDC = smearedQDC*m_ChargeToQdcFactor;
  
  return static_cast<double>(CLHEP::RandGaussQ::shoot(rndEngine, smearedQDC, m_NoiseCharge * m_ChargeToQdcFactor)) + m_qdcPedestal;  
}

//--------------------------------------------------------------------------
double LUCID_DigitizationToolBox::DynodeGainSmearing(double npe, CLHEP::HepRandomEngine* rndEngine){
  
  if (npe < 20) return CLHEP::RandPoissonQ::shoot(rndEngine, npe);
  else          return CLHEP::RandGaussQ::shoot  (rndEngine, npe, std::sqrt(npe));
}
