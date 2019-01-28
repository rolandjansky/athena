/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPMTobAlgorithm.cxx  -  description
                             -------------------
    begin                : Thu 6 Mar 2014
                           Derived from CPAlgorithm (Run 1 version)
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#include "TrigT1CaloUtils/CPMTobAlgorithm.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/IsolationParam.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"


#include <math.h>

namespace LVL1 {
using namespace TrigConf;

const unsigned int CPMTobAlgorithm::m_maxClus = 0xFF;
const double CPMTobAlgorithm::m_maxEta = 2.5;

const unsigned int CPMTobAlgorithm::m_emLUT_ClusterFirstBit = 1;
const unsigned int CPMTobAlgorithm::m_emLUT_ClusterNBits = 6;
const unsigned int CPMTobAlgorithm::m_emLUT_EMIsolFirstBit = 0;
const unsigned int CPMTobAlgorithm::m_emLUT_EMIsolNBits = 4;
const unsigned int CPMTobAlgorithm::m_emLUT_HadVetoFirstBit = 0;
const unsigned int CPMTobAlgorithm::m_emLUT_HadVetoNBits = 3;
  
const unsigned int CPMTobAlgorithm::m_tauLUT_ClusterFirstBit = 1;
const unsigned int CPMTobAlgorithm::m_tauLUT_ClusterNBits = 7;
const unsigned int CPMTobAlgorithm::m_tauLUT_EMIsolFirstBit = 0;
const unsigned int CPMTobAlgorithm::m_tauLUT_EMIsolNBits = 6;

const unsigned int CPMTobAlgorithm::m_noIsol = 999;

LVL1::CPMTobAlgorithm::CPMTobAlgorithm( double eta, double phi, const xAOD::CPMTowerMap_t* ttContainer,
                                ServiceHandle<TrigConf::ITrigConfigSvc> config, int slice ):
  m_configSvc(config),
  m_Core(0),
  m_EMClus(0),
  m_TauClus(0),
  m_EMIsolWord(0),
  m_TauIsolWord(0),
  m_EMCore(0),
  m_EMIsol(0),
  m_HadCore(0),
  m_HadIsol(0),
  m_EtMax(false),
  m_EMThresh(false),
  m_TauThresh(false)
  //m_debug(false)
{
  /** RoI coordinates are centre of window, while key classes are designed
      for TT coordinates - differ by 0.5* TT_size. Using wrong coordinate
      convention can lead to precision errors in key generation.
      
      Offsetting by TT_size/4 will put coordinates inside reference TT
      whether input was reference TT coordinate or RoI centre coordinate.
      To be extra safe, the code below will then compute the centre
      of the reference tower.
      
      This should all ensure consistent & safe key generation however the
      initial coordinate was specified.
      */

  TriggerTowerKey get(phi-M_PI/128.,eta-0.025);
  Coordinate refCoord = get.coord();
  m_refEta = refCoord.eta();
  m_refPhi = refCoord.phi();

  // Set coordinate of centre of RoI, starting from reference tower coordinate
  setRoICoord(m_refEta, m_refPhi);
  
  /** Now loop over towers within window and form clusters.
      
      Interesting one is the 9 RoI cores. The logic below fills these
      as an a 9-element std::vector. The ordering of the elements is
      as follows:
          2  5  8
          1  4  7
          0  3  6
      So the RoI "ET maximum" condition is that 4 >= 0-3, 4 > 5-8
      If you picture the cores in a 3x3 array, cores[iphi][ieta], then the
      index in the vector above = iphi + 3*ieta.
      
  */

  std::vector<unsigned int> emEt(4);
  std::vector<unsigned int> cores(9);
  for (int etaOffset = -1; etaOffset <= 2; etaOffset++) {
    double tempEta = m_refEta + etaOffset*0.1;
    for (int phiOffset = -1; phiOffset <= 2; phiOffset++) {
      double tempPhi = m_refPhi + phiOffset*M_PI/32;
      int key = get.ttKey(tempPhi, tempEta);
      xAOD::CPMTowerMap_t::const_iterator tt = ttContainer->find(key);
      if (tt != ttContainer->end() && fabs(tempEta) < m_maxEta) {
        // Get the TT ET values once here, rather than repeat function calls
        int emTT = 0;
        int hadTT = 0;
        if (slice < 0) {  // Default to using peak slice
          emTT  = (tt->second)->emEnergy();
          hadTT = (tt->second)->hadEnergy();
        }
        else {            // Use user-specified slice
          emTT  = (tt->second)->emSliceEnergy(slice);
          hadTT = (tt->second)->hadSliceEnergy(slice);
        }
        // EM Trigger Clusters and Core clusters
        if (etaOffset >= 0 && etaOffset <= 1 && phiOffset >= 0 && phiOffset <= 1) {
          emEt[phiOffset+2*etaOffset] = emTT;
          m_EMCore += emTT;
          m_HadCore += hadTT;
        }
        // Isolation sums
        else {
          m_EMIsol  += emTT;
          m_HadIsol += hadTT;
        }
        // Each tower is part of up to 4 RoI core clusters
        if (etaOffset >= 0) {
          if (phiOffset >= 0) cores[phiOffset+3*etaOffset] += emTT + hadTT;
          if (phiOffset < 2)  cores[phiOffset+3*etaOffset+1] += emTT + hadTT;
        }
        if (etaOffset < 2) {
          if (phiOffset >= 0) cores[phiOffset+3*etaOffset+3] += emTT + hadTT;
          if (phiOffset < 2)  cores[phiOffset+3*etaOffset+4] += emTT + hadTT;
        }

      }
    } // phi offset loop
  }   // eta offset loop

  // Core of this window is cores[4]
  m_Core = cores[4];

  // Form most energetic 2-tower EM cluster
  m_EMClus = ( (emEt[0] > emEt[3]) ? emEt[0] : emEt[3] )
           + ( (emEt[1] > emEt[2]) ? emEt[1] : emEt[2] );

  // Tau cluster
  m_TauClus = m_EMClus + m_HadCore;
  
  // Check whether RoI condition is met.
  testEtMax(cores);

  // test cluster and isolation conditions
  emAlgorithm();
  tauAlgorithm();
  
}

LVL1::CPMTobAlgorithm::~CPMTobAlgorithm(){
}

/** Compute RoI coordinate. Input coordinate should be inside "reference tower"
    of window. Computes RoI coordinate as centre of 2x2 tower RoI core */
void LVL1::CPMTobAlgorithm::setRoICoord(double eta, double phi) {

  // TriggerTowerKey::coord() returns centre of TriggerTower
  TriggerTowerKey keyLL(phi,eta);
  Coordinate lowerLeft = keyLL.coord();

  // Get centre of tower at eta+1, phi+1, i.e. opposite corner of RoI core
  double dEta = keyLL.dEta(lowerLeft);
  double dPhi = keyLL.dPhi(lowerLeft);  

  TriggerTowerKey keyUR(phi+dPhi, eta + dEta); 
  Coordinate upperRight = keyUR.coord();

  // CoordinateRange object will compute centre, correcting for wrap-around
  CoordinateRange roi(lowerLeft.phi(),upperRight.phi(),lowerLeft.eta(),upperRight.eta());
  m_eta = roi.eta();
  m_phi = roi.phi();

  // Range check (shouldn't be needed, but floating point precision errors possible)
  if       (m_eta > 2.5) m_eta = 2.5;
  else if (m_eta < -2.5) m_eta = -2.5;
}

/** Form all 2x2 clusters within window and test centre is a local ET maximum */
void LVL1::CPMTobAlgorithm::testEtMax(const std::vector<unsigned int>& cores) {

  /** Clusters form a list, arranged as     <br>
        2 5 8                               <br>
        1 4 7                               <br>
        0 3 6                               <br>
      Then 4 = this window's RoI core, and ET max condition is <br>
         4 >= 0, 1, 2, 3   &&   4 < 5, 6, 7, 8 
  */  
  
  // RoI condition test
  m_EtMax = true;
  for (int i = 0; i < 4; i++) if (m_Core < cores[i])  m_EtMax = false;
  for (int i = 5; i < 9; i++) if (m_Core <= cores[i]) m_EtMax = false;
  
}


/** Check trigger condition and set hits if appropriate */
void LVL1::CPMTobAlgorithm::emAlgorithm() {
  
  // Initialise to correct defaults, to be sure
  m_EMThresh = false;
  m_EMIsolWord = 0;
  
  // Don't waste time if it isn't an RoI candidate
  if (!m_EtMax) return;
  
  // Does this pass min TOB pT cut?
  // Need to worry about digit scale not being GeV
  TrigConf::CaloInfo caloInfo = m_configSvc->thresholdConfig()->caloInfo();
  float scale = caloInfo.globalEmScale();
  unsigned int thresh = caloInfo.minTobEM().ptmin*scale;

  if (m_EMClus <= thresh) return;
  
  // Passed, so there is a TOB here
  m_EMThresh = true;

  // Now calculate isolation word
  // Define cluster and isolation values with appropriate scales and range for isolation LUT  
  unsigned int clusMask    = ( (1<<m_emLUT_ClusterNBits)-1 )<<m_emLUT_ClusterFirstBit;
  unsigned int emIsolMask  = ( (1<<m_emLUT_EMIsolNBits) -1 )<<m_emLUT_EMIsolFirstBit;
  unsigned int hadIsolMask = ( (1<<m_emLUT_HadVetoNBits)-1 )<<m_emLUT_HadVetoFirstBit;

  // If cluster overflows, set all threshold bits
  if (m_EMClus >= clusMask) {
    m_EMIsolWord = 0x1F;
    return;
  }
  
  // For consistency with LUT filling and menu parameters, convert ET sums to 100 MeV units
  int clus    = (( (m_EMClus < clusMask)     ? (m_EMClus  & clusMask)    : clusMask ) * 10)/scale;
  int emIsol  = (( (m_EMIsol < emIsolMask)   ? (m_EMIsol  & emIsolMask)  : emIsolMask ) * 10)/scale;
  int hadVeto = (( (m_HadCore < hadIsolMask) ? (m_HadCore & hadIsolMask) : hadIsolMask ) * 10)/scale;
 
  /** Disable isolation by default, then set cut if defined and in range.
    * Expect parameters to be on 100 MeV scale whatever the digit scale, so need to rescale if
      digit scale differs.
    * Also must rescale ET sums to same scale for comparison */
  std::vector<IsolationParam> emIsolParams  = caloInfo.isolationEMIsoForEMthr();
  std::vector<int> emisolcuts;
  emisolcuts.assign(5,m_noIsol);
  
  for (std::vector<IsolationParam>::const_iterator it = emIsolParams.begin(); it != emIsolParams.end(); ++it) {   
      // Offset & Mincut are in 100 MeV units. Slope = 10* the actual slope, but we will stick with integers here
      int offset = (*it).offset();
      int slope  = (*it).slope();
      int mincut = (*it).mincut();
      
      // Upperlimit is in GeV, so convert that to 100 MeV steps
      int upperlimit = (*it).upperlimit()*10;
      
      int bit = (*it).isobit();
      
      if (clus < upperlimit && bit >= 1 && bit <= 5) {
        // As "slope" = 10* slope, rescale clus too to get fraction right.
        int cut = offset + (slope != 0 ? 10*clus/slope : 0);
        if (cut < mincut) cut = mincut;
        emisolcuts[bit-1] = cut;
      }
  }
  
  std::vector<IsolationParam> hadIsolParams = caloInfo.isolationHAIsoForEMthr();  
  std::vector<int> hadisolcuts;
  hadisolcuts.assign(5,m_noIsol); 
  
  for (std::vector<IsolationParam>::const_iterator it = hadIsolParams.begin(); it != hadIsolParams.end(); ++it) {   
      int offset = (*it).offset();
      int slope  = (*it).slope();
      int mincut = (*it).mincut();
      int upperlimit = (*it).upperlimit()*10;
      int bit = (*it).isobit();
      
      if (clus < upperlimit && bit >= 1 && bit <= 5) {
        int cut = offset + (slope != 0 ? 10*clus/slope : 0);
        if (cut < mincut) cut = mincut;
        hadisolcuts[bit-1] = cut;
      }
  }

  // Set isolation bits
  for (unsigned int isol = 0; isol < 5; ++isol) 
    if (emIsol <= emisolcuts[isol] && hadVeto <= hadisolcuts[isol]) m_EMIsolWord += (1 << isol);
    
}


/** Check trigger condition and set hits if appropriate */
void LVL1::CPMTobAlgorithm::tauAlgorithm() {
  
  // Initialise to correct defaults, to be sure
  m_TauThresh = false;
  m_TauIsolWord = 0;
  
  // Don't waste time if it isn't an RoI candidate
  if (!m_EtMax) return;
  
  // Does this pass min TOB pT cut?
  // Need to worry about digit scale not being GeV
  TrigConf::CaloInfo caloInfo = m_configSvc->thresholdConfig()->caloInfo();
  float scale = caloInfo.globalEmScale();
  unsigned int thresh = caloInfo.minTobTau().ptmin*scale;
 
  if (m_TauClus <= thresh) return;
  
  // Passed, so there is a TOB here
  m_TauThresh = true;
  
  // Now calculate isolation word
  // Define cluster and isolation values with appropriate scales and range for isolation LUT
  unsigned int clusMask    = ( (1<<m_tauLUT_ClusterNBits)-1 )<<m_tauLUT_ClusterFirstBit;
  unsigned int emIsolMask  = ( (1<<m_tauLUT_EMIsolNBits) -1 )<<m_tauLUT_EMIsolFirstBit;

  // If cluster overflows, set all threshold bits
  if (m_TauClus >= clusMask) {
     m_TauIsolWord = 0x1F;
     return;
  }
                 
  // Convert to 100 MeV units to match parameters
  int clus    = (( (m_TauClus < clusMask)    ? (m_TauClus & clusMask)    : clusMask ) * 10)/scale;
  int emIsol  = (( (m_EMIsol < emIsolMask)   ? (m_EMIsol  & emIsolMask)  : emIsolMask ) * 10)/scale;
  
  // Get isolation values from menu (placeholder code - example logic)
  std::vector<IsolationParam> emIsolParams  = caloInfo.isolationEMIsoForTAUthr();
  std::vector<int> emisolcuts;
  emisolcuts.assign(5,m_noIsol);
  
  for (std::vector<IsolationParam>::const_iterator it = emIsolParams.begin(); it != emIsolParams.end(); ++it) {   
    if ((*it).isDefined()) {
      float offset = (*it).offset();
      float slope  = (*it).slope();
      float mincut = (*it).mincut();
      // upperlimit is in GeV, so convert to 100 MeV units
      int upperlimit = (*it).upperlimit()*10;
      int bit = (*it).isobit();
      
      if (clus < upperlimit && bit >= 1 && bit <= 5) {
        // slope parameter is 10* actual slope, so correct for that here
        int cut = offset + (slope != 0 ? 10*clus/slope : 0);
        if (cut < mincut) cut = mincut;
        emisolcuts[bit-1] = cut;
      }
    }
  }

  // Set isolation bits
  for (unsigned int isol = 0; isol < 5; ++isol) 
    if (emIsol <= emisolcuts[isol]) m_TauIsolWord += (1 << isol);

}

/**  Report whether this passed as an EM TOB */
bool LVL1::CPMTobAlgorithm::isEMRoI(){
  
  bool passed = ( m_EMThresh && m_EtMax );

  return passed;
}

/**  Report whether this passed as a Tau TOB */
bool LVL1::CPMTobAlgorithm::isTauRoI(){

  bool passed = ( m_TauThresh && m_EtMax );

  return passed;
}

// Public accessor methods follow

/** Returns RoI Core ET */
int LVL1::CPMTobAlgorithm::CoreET() {
  return m_Core;
}

/** Returns EM cluster ET, limited to 8 bits */
int LVL1::CPMTobAlgorithm::EMClusET() {
  return ( (m_EMClus < m_maxClus) ? m_EMClus : m_maxClus );
}

/** Returns Tau cluster ET, limited to 8 bits */
int LVL1::CPMTobAlgorithm::TauClusET() {
  return ( (m_TauClus < m_maxClus) ? m_TauClus : m_maxClus );
}

/** Returns EM isolation word (5 bits) */
int LVL1::CPMTobAlgorithm::EMIsolWord() {
  return m_EMIsolWord;
}

/** Returns Tau isolation word (5 bits) */
int LVL1::CPMTobAlgorithm::TauIsolWord() {
  return m_TauIsolWord;
}

/** Returns EM isolation ET */
int LVL1::CPMTobAlgorithm::EMIsolET() {
  return m_EMIsol;
}

/** Returns Had isolation ET */
int LVL1::CPMTobAlgorithm::HadIsolET() {
  return m_HadIsol;
}

/** Returns EM core ET  */
int LVL1::CPMTobAlgorithm::EMCoreET() {
  return m_EMCore;
}

/** Returns Had core ET (inner isolation sum) */
int LVL1::CPMTobAlgorithm::HadCoreET() {
  return m_HadCore;
}

/** Does this window pass the local ET maximum condition */
bool LVL1::CPMTobAlgorithm::isEtMax() {
  return m_EtMax;
}

/** Returns EM cluster ET with range and precision of LUT input */
int LVL1::CPMTobAlgorithm::EMLUTClus() {
  int ET = (m_EMClus >> m_emLUT_ClusterFirstBit);
  if (ET > (1<< m_emLUT_ClusterNBits) - 1) ET = (1<<m_emLUT_ClusterNBits) - 1;
  return (ET << m_emLUT_ClusterFirstBit);
}

/** Returns Tau cluster ET with range and precision of LUT input */
int LVL1::CPMTobAlgorithm::TauLUTClus() {
  int ET = (m_TauClus >> m_tauLUT_ClusterFirstBit);
  if (ET > (1<<m_tauLUT_ClusterNBits) - 1) ET = (1<<m_tauLUT_ClusterNBits) - 1;
  return (ET << m_tauLUT_ClusterFirstBit);
}

/** Returns EM isolation ET with range and precision of EM LUT input */
int LVL1::CPMTobAlgorithm::EMLUTEMIsol() {
  int ET = (m_EMIsol >> m_emLUT_EMIsolFirstBit);
  if (ET > (1<<m_emLUT_EMIsolNBits) - 1) ET = (1<<m_emLUT_EMIsolNBits) - 1;
  return (ET << m_emLUT_EMIsolFirstBit);
}

/** Returns Had veto ET with range and precision of EM LUT input */
int LVL1::CPMTobAlgorithm::EMLUTHadVeto() {
  int ET = (m_HadCore >> m_emLUT_HadVetoFirstBit);
  if (ET > (1<<m_emLUT_HadVetoNBits) - 1) ET = (1<<m_emLUT_HadVetoNBits) - 1;
  return (ET << m_emLUT_HadVetoFirstBit);
}

/** Returns EM isolation ET with range and precision of Tau LUT input */
int LVL1::CPMTobAlgorithm::TauLUTEMIsol() {
  int ET = (m_EMIsol >> m_tauLUT_EMIsolFirstBit);
  if (ET > (1<<m_tauLUT_EMIsolNBits) - 1) ET = (1<<m_tauLUT_EMIsolNBits) - 1;
  return (ET << m_tauLUT_EMIsolFirstBit);
}

/** Returns eta coordinate of RoI */
double LVL1::CPMTobAlgorithm::eta() {
  return m_eta;
}

/** Returns phi coordinate of RoI, using standard ATLAS convention */
double LVL1::CPMTobAlgorithm::phi() {
  return ( (m_phi <= M_PI) ? m_phi : m_phi - 2.*M_PI);
}

/** Returns CPMTobRoI for EM hypothesis, provided EM Tob conditions passed */
xAOD::CPMTobRoI* LVL1::CPMTobAlgorithm::EMCPMTobRoI() {
  if (isEMRoI()) {
    xAOD::CPMTobRoI* pointer = createTobRoI(TrigT1CaloDefs::emTobType); 
    return pointer;
  }
  
  return 0;

}

/** Returns CPMTobRoI for TAU hypothesis, provided TAU Tob conditions passed */
xAOD::CPMTobRoI* LVL1::CPMTobAlgorithm::TauCPMTobRoI() {
  
  xAOD::CPMTobRoI* pointer = 0;
  
  if (isTauRoI()) pointer = createTobRoI(TrigT1CaloDefs::tauTobType);
  
  return pointer;

}

/** Create a pointer to a CPMTobRoI and return it.
    This function does not check whether a valid RoI exists - that is left
    to the calling functions (but as this is a private method that should
    be enough) */
xAOD::CPMTobRoI* LVL1::CPMTobAlgorithm::createTobRoI(int type) {
  
  xAOD::CPMTobRoI* pointer = 0;
  
  int et = 0;
  int isol = 0;
  
  if (type == TrigT1CaloDefs::emTobType) {
    et   = EMClusET();
    isol = EMIsolWord();
  }
  else if (type == TrigT1CaloDefs::tauTobType) {
    et   = TauClusET();
    isol = TauIsolWord();
  }
  else return pointer;
  
  // Need to calculate hardware coordinate
  // Object coordinate is RoI centre, but this works better if use reference tower coord
  float eta = m_eta - 0.05;
  float phi = m_phi - M_PI/64;
  if (phi < 0) phi += 2*M_PI; 
  Coordinate coord(phi, eta);
  CoordToHardware convertor;
  
  int crate = convertor.cpCrate(coord);
  int module = convertor.cpModule(coord);
  int chip = convertor.cpModuleFPGA(coord);
  int location = convertor.cpModuleLocalRoI(coord);
  
  /*
  float phi = ( (m_phi >= 0) ?  m_phi : 2.*M_PI + m_phi );
  int crate = 2*phi/M_PI;
  int module = (m_eta + 2.8)/0.4 + 1;
  int iPhi = (phi - crate*M_PI/2)*32/M_PI;
  int chip = iPhi/2;
  int iEta = (m_eta + 2.8 - module*0.4)/0.1;
  int iSide = iEta/2;
  int location = (iSide << 2) + ((iPhi&1) << 1) + (iEta&1);
  */
  
  pointer = new xAOD::CPMTobRoI;
  pointer->makePrivateStore();
  pointer->initialize(crate, module, chip, location, type, et, isol);
  
  return pointer;
  
}


} // end of namespace bracket


