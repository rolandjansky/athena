/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JEMJetAlgorithm.cxx  -  description
                             -------------------
    begin                : Thurs Mar 13 2014
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#include "TrigT1CaloUtils/JEMJetAlgorithm.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/JetInputKey.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/ThresholdConfig.h"


#include <math.h>

namespace LVL1 {
using namespace TrigConf;

const int JEMJetAlgorithm::m_satLarge = 0x3FF;
const int JEMJetAlgorithm::m_satSmall = 0x1FF;

LVL1::JEMJetAlgorithm::JEMJetAlgorithm( double eta, double phi, const std::map<int, JetInput *>* jiContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config):
  m_configSvc(config),
  m_ET4x4(0),
  m_ET6x6(0),
  m_ET8x8(0),
  m_ETLarge(0),
  m_ETSmall(0),
  m_EtMax(false)
  //m_debug(false)
{
  /** This could potentially be called with the coordinate being the centre <br>
      of an RoI _or_ the centre of a JetElement. We want to ensure that we  <br>
      do the right thing in either case.                                    <br>
                                                                            <br>
      Offsetting by JE size/4 should put the coordinates inside the         <br>
      reference JetElement whether input was reference JetElement           <br>
      coordinate or RoI centre coordinate. Can't subtract a fixed amount    <br>
      as the eta size of JetElements varies, so need to work out how much   <br>
      to subtract first!                                                    <br>
      To be extra safe, the code below will then compute the centre         <br>
      of the reference JetElement.                                          <br>
                                                                            <br> 
      This should all ensure consistent & safe key generation however the   <br>
      initial coordinate was specified.
      */

  // Offset depends on eta. Need to protect against rounding errors even here
  JetInputKey get(phi-0.01,eta-0.01);
  double de = get.dEta()/4.;
  
  // Get coordinate of centre of this "reference element"
  Coordinate refCoord = get.getCentre(phi-M_PI/64., eta-de);
  m_refEta = refCoord.eta();
  m_refPhi = refCoord.phi();

  // Set coordinate of centre of RoI, starting from reference tower coordinate
  setRoICoord(m_refEta, m_refPhi);

  // Get coordinate of bottom-left JetInput in this window
  int minEtaOffset = -1;
  Coordinate startCoord = get.lowerLeft(m_refPhi,m_refEta);
  if (startCoord.eta() == TrigT1CaloDefs::RegionERROREtaCentre) { // already at left edge
    startCoord = get.downPhi(m_refPhi,m_refEta);
    minEtaOffset = 0;
  }
  double tempEta = startCoord.eta();
  double startPhi = startCoord.phi();
    
  /** Now loop over JetInputs within window and form clusters.
      ET8x8 = all elements
      ET4x4 = central 2x2
      ET6x6 = most energetic of 4 clusters
      Interesting one is the 9 RoI cores. The logic below fills these
      as an a 9-element std::vector. The ordering of the elements is
      as follows:
          2  5  8
          1  4  7
          0  3  6
      So the RoI "ET maximum" condition is that 4 >= 0-3, 4 > 5-8
      If you picture the cores in a 3x3 array, cores[iphi][ieta], then the
      index in the vector above = iphi + 3*ieta.
      
      We use the fact that core[4] = ET4x4 to safe a little time here
  */

  std::vector<int> et6x6(4);
  std::vector<int> cores(9);
  for (int etaOffset=minEtaOffset; etaOffset<=2 ; etaOffset++){
    Coordinate tempCoord(startPhi,tempEta);
    for (int phiOffset=-1; phiOffset<=2 ; phiOffset++){
      int tempKey = get.jeKey(tempCoord);
      std::map<int, JetInput*>::const_iterator ji = jiContainer->find(tempKey);
      if (ji != jiContainer->end() ){
        // get ET once here, rather than repeat function calls
        int ET = (ji->second)->energy();
        // 8x8 jet is easy
        m_ET8x8 += ET;
        // but there are 4 possible 6x6 clusters
        if (phiOffset > -1) {
          if (etaOffset < 2)  et6x6[1] += ET;
          if (etaOffset > -1) et6x6[2] += ET;
        }
        if (phiOffset < 2) {
          if (etaOffset < 2)  et6x6[0] += ET;
          if (etaOffset > -1) et6x6[3] += ET;
        }
        // Each JetInput is part of up to 4 RoI core clusters
        
        if (etaOffset >= 0) {
          if (phiOffset >= 0) cores[phiOffset+3*etaOffset] += ET;
          if (phiOffset < 2) cores[phiOffset+3*etaOffset+1] += ET;
        }
        if (etaOffset < 2) {
          if (phiOffset >= 0) cores[phiOffset+3*etaOffset+3] += ET;
          if (phiOffset < 2) cores[phiOffset+3*etaOffset+4] += ET;
        }
      } // end of check that jetinput exists in container
      tempCoord = get.upPhi(tempCoord); // Increment phi coordinate
    } // end phi offset loop
    tempCoord = get.rightEta(tempCoord); // Increment eta coordinate
    tempEta = tempCoord.eta();
    if (tempEta == TrigT1CaloDefs::RegionERROREtaCentre) break; // gone outside coverage
  } // end eta offset loop

  // 4x4 cluster = central RoI core
  m_ET4x4 = cores[4];
  
  // find most energetic 6x6 cluster
  for (int i = 0; i < 4; i++) if (et6x6[i] > m_ET6x6) m_ET6x6 = et6x6[i];

  // Check whether RoI condition is met.
  testEtMax(cores);

  // test trigger conditions
  passesTrigger();
  
}

LVL1::JEMJetAlgorithm::~JEMJetAlgorithm(){
}

/** Compute RoI coordinate. Input coordinate should be inside "reference tower"
    of window. Computes RoI coordinate as centre of 2x2 element RoI core */
void LVL1::JEMJetAlgorithm::setRoICoord(double eta, double phi) {

  JetInputKey keyLL(phi,eta);
  // Get coordinate of centre of this "reference element"
  Coordinate lowerLeft = keyLL.getCentre(phi, eta);

  // Hence find lower-left corner of RoI core
  double lowerEta = lowerLeft.eta() - ( keyLL.dEta()/2. );
  double lowerPhi = lowerLeft.phi() - ( keyLL.dPhi()/2. );

  // Get coordinate of opposite corner of RoI core:
  Coordinate upperRight;
  if (keyLL.isFCAL(lowerLeft.eta()) && eta > 0) {
    // Special case: no JE to right, so get centre of element at eta, phi+1
    upperRight = keyLL.upPhi(phi, eta);
  }
  else {
    // Get centre of element at eta+1, phi+1, i.e. opposite corner of RoI core
    upperRight = keyLL.upperRight(phi, eta);
  }

  // Now get upper-right corner of RoI core:
  JetInputKey keyUR(upperRight);
  double upperEta = upperRight.eta() + ( keyUR.dEta()/2. );
  double upperPhi = upperRight.phi() + ( keyUR.dPhi()/2. );
  

  // CoordinateRange object will compute centre, correcting for wrap-around
  CoordinateRange roi(lowerPhi,upperPhi,lowerEta,upperEta);
  m_eta = roi.eta();
  m_phi = roi.phi();

}

/** Form all 2x2 clusters within window and test centre is a local ET maximum */
void LVL1::JEMJetAlgorithm::testEtMax(const std::vector<int>& cores) {

  /** Input clusters form a list, arranged as  <br>
        2 5 8                                  <br>
        1 4 7                                  <br>
        0 3 6                                  <br>
      Then 4 = this window's RoI core, and ET max condition is <br>
         4 >= 0, 1, 2, 3   &&   4 < 5, 6, 7, 8 
  */  

  // RoI condition test
  m_EtMax = true;
  for (int i = 0; i < 4; i++) if (cores[4] < cores[i])  m_EtMax = false;
  for (int i = 5; i < 9; i++) if (cores[4] <= cores[i]) m_EtMax = false;
  
}

/** Check trigger condition and set ET values if TOB created */
void LVL1::JEMJetAlgorithm::passesTrigger() {
  
  // Belt and braces
  m_ETLarge = 0;
  m_ETSmall = 0;
  
  // Don't waste time if it isn't an RoI candidate
  if (!m_EtMax) return;
  
  // Does this pass min TOB pT cut?
  TrigConf::CaloInfo caloInfo = m_configSvc->thresholdConfig()->caloInfo();
  float scale = caloInfo.globalJetScale(); 
 
  unsigned int sizeLarge = caloInfo.jetWindowSizeLarge();
  int threshLarge = caloInfo.minTobJetLarge().ptmin*scale;

  int etLarge = m_ET8x8;
  if (sizeLarge == 6)      etLarge = m_ET6x6;
  else if (sizeLarge == 4) etLarge = m_ET4x4;

  unsigned int sizeSmall = caloInfo.jetWindowSizeSmall();  
  int threshSmall = caloInfo.minTobJetSmall().ptmin*scale;

  int etSmall = m_ET4x4;
  if (sizeSmall == 6)      etLarge = m_ET6x6;
  else if (sizeSmall == 8) etLarge = m_ET8x8;

  if (etLarge <= threshLarge && etSmall <= threshSmall) return;

  m_ETLarge = etLarge;
  m_ETSmall = etSmall;
  
   
}

// Public accessor methods follow

/** Returns RoI Core ET */
int LVL1::JEMJetAlgorithm::Core() {
  return m_ET4x4;
}

/** Returns 4x4 TT cluster ET */
int LVL1::JEMJetAlgorithm::ET4x4() {
  return ( (m_ET4x4 < m_satLarge) ? m_ET4x4 : m_satLarge );
}

/** Returns 6x6 TT cluster ET */
int LVL1::JEMJetAlgorithm::ET6x6() {
  return ( (m_ET6x6 < m_satLarge) ? m_ET6x6 : m_satLarge );
}

/** Returns 8x8 TT cluster ET */
int LVL1::JEMJetAlgorithm::ET8x8() {
  return ( (m_ET8x8 < m_satLarge) ? m_ET8x8 : m_satLarge );
}

/** Returns Large cluster ET */
int LVL1::JEMJetAlgorithm::ETLarge() {
  return ( (m_ETLarge < m_satLarge) ? m_ETLarge : m_satLarge );
}

/** Returns Small cluster ET */
int LVL1::JEMJetAlgorithm::ETSmall() {
  return ( (m_ETSmall < m_satSmall) ? m_ETSmall : m_satSmall );
}

/** Does this window pass the local ET maximum condition */
bool LVL1::JEMJetAlgorithm::isEtMax() {
  return m_EtMax;
}

/** Does this window pass the local ET maximum condition */
bool LVL1::JEMJetAlgorithm::isRoI() {
  return ( m_EtMax && (m_ETLarge > 0 || m_ETSmall > 0) );
}

/** Returns eta coordinate of RoI */
double LVL1::JEMJetAlgorithm::eta() {
  return m_eta;
}

/** Returns phi coordinate of RoI, using standard ATLAS convention */
double LVL1::JEMJetAlgorithm::phi() {
  return ( (m_phi <= M_PI) ? m_phi : m_phi - 2.*M_PI);
}

/** Returns a JEMTobRoI object, provided the TOB conditions were met.
    If not will return a null pointer - user's responsibility to check */

xAOD::JEMTobRoI* LVL1::JEMJetAlgorithm::jemTobRoI() { 
  
  if (isRoI()) {
    
    // Need to calculate hardware coordinate
    Coordinate coord(m_refPhi, m_refEta);
    CoordToHardware convertor;
  
    int crate = convertor.jepCrate(coord);
    int jem = convertor.jepModule(coord);
    unsigned int jemCoord = convertor.jepLocalCoordinate(coord);
    int frame = (jemCoord>>2);
    int lc = jemCoord&3;
    
    /*
    JetEnergyModuleKey get();
    Coordinate tempCoord(m_refPhi, m_refEta);
    unsigned int quadrant = m_refPhi*2/M_PI;
    unsigned int row = get.row(tempCoord);
    unsigned int col = get.col(tempCoord);
    
    unsigned int crate = quadrant&1;
    unsigned int jem = get.jem(tempCoord);
    unsigned int frame = ((col&2)<<1) + (row>>1);
    unsigned int lc = (col&1) + (row&1)*2;
    */
    xAOD::JEMTobRoI* roi = new xAOD::JEMTobRoI();
    roi->makePrivateStore();
    roi->initialize(crate, jem, frame, lc, ETLarge(), ETSmall());
    return roi;
  }
  
  return 0;

}

} // end of namespace bracket


