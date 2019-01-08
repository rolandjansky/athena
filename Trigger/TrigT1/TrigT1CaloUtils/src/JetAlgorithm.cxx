/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetAlgorithm.cxx  -  description
                             -------------------
    begin                : Tues Sep 11 2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include <math.h>

namespace LVL1 {
using namespace TrigConf;

const int JetAlgorithm::m_sat = 0x3FF;

LVL1::JetAlgorithm::JetAlgorithm( double eta, double phi, const std::map<int, JetInput *>* jiContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config):
  m_configSvc(config),
  m_ET4x4(0),
  m_ET6x6(0),
  m_ET8x8(0),
  m_EtMax(false),
  m_Hits(0),
  m_debug(false)
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

LVL1::JetAlgorithm::~JetAlgorithm(){
}

/** Compute RoI coordinate. Input coordinate should be inside "reference tower"
    of window. Computes RoI coordinate as centre of 2x2 element RoI core */
void LVL1::JetAlgorithm::setRoICoord(double eta, double phi) {

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
void LVL1::JetAlgorithm::testEtMax(const std::vector<int>& cores) {

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

/** Check trigger condition and set hits if appropriate */
void LVL1::JetAlgorithm::passesTrigger() {
  
  m_Hits = 0;
  // Don't waste time if it isn't an RoI candidate
  if (!m_EtMax) return;

  L1DataDef def;
  std::string jetTriggerType = def.jetType();
  std::string fjetTriggerType = ( (m_eta > 0) ? def.jfType() : def.jbType() );

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();

  for (std::vector<TrigConf::TriggerThreshold*>::const_iterator it = thresholds.begin(); it != thresholds.end(); ++it) {
    
    if ((*it)->type()==jetTriggerType || (*it)->type()==fjetTriggerType){  //only use Jet Thresholds
      int threshNum = (*it)->thresholdNumber();
      if (threshNum >= 0 && threshNum < (int)TrigT1CaloDefs::numOfJetThresholds) {
        int size = 0;
        int value = m_sat;
        getThresholds((*it), size, value);

        if (m_debug) {
        std::cout << "JetAlgorithm: Test jet threshold " << threshNum << " with type = " << jetTriggerType
                  << ", threshold = " << value << " and window = " << size << std::endl;
        }
        bool passes = false;
        switch (size){
          case 4 :
                  passes = (ET4x4() > value);
                  break;
          case 6 :
                  passes = (ET6x6() > value);
                  break;
          case 8 :
                  passes = (ET8x8() > value);
                  break;
          default : std::cout << "ERROR IN JetAlgorithm WITH COORDS "<<m_phi<<", "<<m_eta
                         <<". WINDOW SIZE OF "<<size<<" NOT RECOGNISED"<<std::endl;
        }//end switch
        
        if (passes) {
          if ((*it)->type()==jetTriggerType) m_Hits = m_Hits|(1<<threshNum);    // central jet
          else                               m_Hits = m_Hits|(1<<(threshNum+8));  // forward jet
        }

      } // end if valid threshold
                
    }//endif - is jet threshold
  }//end thresh for-loop
   
}

// Public accessor methods follow

/** Returns RoI Core ET */
int LVL1::JetAlgorithm::Core() const {
  return m_ET4x4;
}

/** Returns 4x4 TT cluster ET */
int LVL1::JetAlgorithm::ET4x4() const {
  return ( (m_ET4x4 < m_sat) ? m_ET4x4 : m_sat );
}

/** Returns 6x6 TT cluster ET */
int LVL1::JetAlgorithm::ET6x6() const {
  return ( (m_ET6x6 < m_sat) ? m_ET6x6 : m_sat );
}

/** Returns 8x8 TT cluster ET */
int LVL1::JetAlgorithm::ET8x8() const {
  return ( (m_ET8x8 < m_sat) ? m_ET8x8 : m_sat );
}

/** Does this window pass the local ET maximum condition */
bool LVL1::JetAlgorithm::isEtMax() const {
  return m_EtMax;
}

/** Returns eta coordinate of RoI */
double LVL1::JetAlgorithm::eta() const {
  return m_eta;
}

/** Returns phi coordinate of RoI, using standard ATLAS convention */
double LVL1::JetAlgorithm::phi() const {
  return ( (m_phi <= M_PI) ? m_phi : m_phi - 2.*M_PI);
}

/** Returns hitword for this window */
unsigned int LVL1::JetAlgorithm::Hits() const {
  return m_Hits;
}

/** Returns RoI word for this window */
unsigned int LVL1::JetAlgorithm::RoIWord() const {
  unsigned int roiWord = (TrigT1CaloDefs::jetRoIType<<30) +  (TrigT1CaloDefs::jetRoI<<29);
  
  CoordToHardware convertor;
  Coordinate coord(m_refPhi, m_refEta);
  roiWord |= (convertor.jepCoordinateWord(coord))<<19;

  roiWord += m_Hits;
  if ((m_Hits & 0xf00) != 0) roiWord+=1<<18; // forward jet flag
  
  return roiWord;
}

/** Returns a JetROI object summarising the results for this location */

JetROI* LVL1::JetAlgorithm::produceExternal() const { 
  
  JetROI* temp=new JetROI(phi(), eta(), Core(), RoIWord(),
                          ET4x4(), ET6x6(), ET8x8(), 0);

  return temp;

}

/** returns true if this jet is a forward jet - i.e. has passed fwd jet thresholds */
bool LVL1::JetAlgorithm::isForwardJet() const {
  return ( (m_Hits & 0xf00) > 0 );
}

/** Returns threshold value and cluster size */
void LVL1::JetAlgorithm::getThresholds(TrigConf::TriggerThreshold* thresh, int& size, int& value) {

  size = 0;
  value = 1023;  // Set impossible default in case no threshold found
      
  /** Use reference JE for coordinate.
      Thresholds can be set at full granularity in eta, but module granularity in phi */
  // protect against rounding errors (as JE centre == TT edge)
  int ieta = int((m_refEta + (m_refEta>0 ? 0.005 : -0.005))/0.1);
  int iphi = int((m_refPhi-0.005)*32/M_PI);
  iphi = 16*(iphi/16) + 8;

  TriggerThresholdValue* tv = thresh->triggerThresholdValue(ieta,iphi);

  if (tv != 0) {  
    JetThresholdValue* jtv;
    jtv = dynamic_cast<JetThresholdValue*> (tv);
    if (jtv) {
      size=jtv->window();
      value = jtv->thresholdValueCount();
    }
  }

}

} // end of namespace bracket


