/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPAlgorithm.cxx  -  description
                             -------------------
    begin                : Tues Sep 11 2007
                           Very loosely derived from Ed Moyse's InternalEmTauRoI
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#include "TrigT1CaloUtils/CPAlgorithm.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include <math.h>

namespace LVL1 {
using namespace TrigConf;

const int CPAlgorithm::m_maxClus = 0xFF;
const int CPAlgorithm::m_maxIsol = 0x3F;
const double CPAlgorithm::m_maxEta = 2.5;


LVL1::CPAlgorithm::CPAlgorithm( double eta, double phi, const CPMTowerMap_t* ttContainer,
                                ServiceHandle<TrigConf::ITrigConfigSvc> config, int slice):
  m_configSvc(config),
  m_Core(0),
  m_EMClus(0),
  m_TauClus(0),
  m_EMIsol(0),
  m_HadCore(0),
  m_HadIsol(0),
  m_EtMax(false),
  m_Hits(0)
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

  std::vector<int> emEt(4);
  std::vector<int> cores(9);
  for (int etaOffset = -1; etaOffset <= 2; etaOffset++) {
    double tempEta = m_refEta + etaOffset*0.1;
    for (int phiOffset = -1; phiOffset <= 2; phiOffset++) {
      double tempPhi = m_refPhi + phiOffset*M_PI/32;
      int key = get.ttKey(tempPhi, tempEta);
      CPMTowerMap_t::const_iterator tt = ttContainer->find(key);
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
        // Trigger clusters & hadronic veto
        if (etaOffset >= 0 && etaOffset <= 1 && phiOffset >= 0 && phiOffset <= 1) {
          emEt[phiOffset+2*etaOffset] = emTT;
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

  // test trigger conditions
  passesTrigger();
  
}

LVL1::CPAlgorithm::~CPAlgorithm(){
}

/** Compute RoI coordinate. Input coordinate should be inside "reference tower"
    of window. Computes RoI coordinate as centre of 2x2 tower RoI core */
void LVL1::CPAlgorithm::setRoICoord(double eta, double phi) {

  // TriggerTowerKey::coord() returns centre of TriggerTower
  TriggerTowerKey keyLL(phi,eta);
  Coordinate lowerLeft = keyLL.coord();

  // Get centre of tower at eta+1, phi+1, i.e. opposite corner of RoI core
  double dEta = keyLL.dEta(lowerLeft);
  double dPhi = keyLL.dPhi(lowerLeft);  
  TriggerTowerKey keyUR(phi+dPhi,eta+dEta);
  Coordinate upperRight = keyUR.coord();

  // CoordinateRange object will compute centre, correcting for wrap-around
  CoordinateRange roi(lowerLeft.phi(),upperRight.phi(),lowerLeft.eta(),upperRight.eta());
  m_eta = roi.eta();
  m_phi = roi.phi();
}

/** Form all 2x2 clusters within window and test centre is a local ET maximum */
void LVL1::CPAlgorithm::testEtMax(const std::vector<int>& cores) {

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
void LVL1::CPAlgorithm::passesTrigger() {
  
  m_Hits = 0;
  // Don't waste time if it isn't an RoI candidate
  if (!m_EtMax) return;

  // Get thresholds from menu and test whether any are passed
  L1DataDef def;
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  
  for (std::vector<TriggerThreshold*>::const_iterator it = thresholds.begin();
       it != thresholds.end(); ++it) {

    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      /** get threshold values from TriggerMenu */
      getThresholds((*it));

      /** Get threshold number and check validity */
      int numThresh = (*it)->thresholdNumber();
      if (numThresh >= 0 && numThresh < (int)TrigT1CaloDefs::numOfCPThresholds) {

        bool passes = false;
        if ((*it)->type() == def.emType()) {
          passes=emAlgorithm();
        }
        else {
          passes=tauAlgorithm();
        }

        /** Set bit if passed */
        if (passes) m_Hits = m_Hits|(1<<numThresh);
        
      } // end if valid threshold number
    } // end if EM/tau
  } // end loop over thresholds 
   
}

/** Get threshold values for RoI eta/phi */
void LVL1::CPAlgorithm::getThresholds(TriggerThreshold* thresh) {

  m_ClusterThreshold = 999;      // Set impossible default in case no threshold found
  m_emRingIsolationThreshold = 0;
  m_hadRingIsolationThreshold = 0;
  m_hadCoreIsolationThreshold = 0;

  /** eta bins run from -49 -> 49
     "reference tower" coordinate should ensure correct RoI->module association.
      Firmware upgrade will allow variation of threshold values at full eta granularity,
      but phi granularity is still that of the module*/
  int ieta = int(m_refEta/0.1) + ((m_refEta>0) ? 0 : -1);
  int iphi = int(m_refPhi*32/M_PI);
  // quantise by module
  //if (ieta > 0) ieta = 4*(ieta/4) + 2;
  //else          ieta = 4*((ieta-3)/4) + 2;
  iphi = 16*(iphi/16) + 8;

  TriggerThresholdValue* tv = thresh->triggerThresholdValue(ieta,iphi);

  // cluster/isolation ET in counts, not GeV, so get values in same units
  if (tv != 0) { 
    ClusterThresholdValue* ctv;
    ctv = dynamic_cast<ClusterThresholdValue*> (tv);
    if (ctv) {
      m_ClusterThreshold = ctv->thresholdValueCount();
      m_emRingIsolationThreshold = ctv->emIsolationCount();
      m_hadRingIsolationThreshold = ctv->hadIsolationCount();
      m_hadCoreIsolationThreshold = ctv->hadVetoCount();
    }
  }

}

/**  Check whether EM cluster and isolation passed */
bool LVL1::CPAlgorithm::emAlgorithm(){
  bool passed=false;

  // Use accessors to limit word lengths for sums
  if ( (EMClus()  > m_ClusterThreshold)
    && (EMIsol()  <= m_emRingIsolationThreshold)
    && (HadIsol() <= m_hadRingIsolationThreshold)
    && (HadVeto() <= m_hadCoreIsolationThreshold) )
    passed=true;

  return passed;
}

/**  Check whether Tau cluster and isolation passed */
bool LVL1::CPAlgorithm::tauAlgorithm(){
  bool passed=false;

  // Use accessors to limit word lengths for sums
  if ( (TauClus()  > m_ClusterThreshold)
    && (EMIsol()  <= m_emRingIsolationThreshold)
    && (HadIsol() <= m_hadRingIsolationThreshold) )
    passed=true;

  return passed;
}

// Public accessor methods follow

/** Returns RoI Core ET */
int LVL1::CPAlgorithm::Core() const {
  return m_Core;
}

/** Returns EM cluster ET, limited to 8 bits */
int LVL1::CPAlgorithm::EMClus() const {
  return ( (m_EMClus < m_maxClus) ? m_EMClus : m_maxClus );
}

/** Returns Tau cluster ET, limited to 8 bits */
int LVL1::CPAlgorithm::TauClus() const {
  return ( (m_TauClus < m_maxClus) ? m_TauClus : m_maxClus );
}

/** Returns EM isolation ET, limited to 6 bits */
int LVL1::CPAlgorithm::EMIsol() const {
  return ( (m_EMIsol < m_maxIsol) ? m_EMIsol : m_maxIsol );
}

/** Returns Had isolation ET, limited to 6 bits */
int LVL1::CPAlgorithm::HadIsol() const {
  return ( (m_HadIsol < m_maxIsol) ? m_HadIsol : m_maxIsol );
}

/** Returns Had veto ET (inner isolation sum), limited to 6 bits */
int LVL1::CPAlgorithm::HadVeto() const {
  return ( (m_HadCore < m_maxIsol) ? m_HadCore : m_maxIsol );
}

/** Does this window pass the local ET maximum condition */
bool LVL1::CPAlgorithm::isEtMax() const {
  return m_EtMax;
}

/** Returns eta coordinate of RoI */
double LVL1::CPAlgorithm::eta() const {
  return m_eta;
}

/** Returns phi coordinate of RoI, using standard ATLAS convention */
double LVL1::CPAlgorithm::phi() const {
  return ( (m_phi <= M_PI) ? m_phi : m_phi - 2.*M_PI);
}

/** Returns hitword for this window */
unsigned int LVL1::CPAlgorithm::Hits() const {
  return m_Hits;
}


/** Returns RoI word for this window */
unsigned int LVL1::CPAlgorithm::RoIWord() const {
  unsigned int roiWord = (TrigT1CaloDefs::cpRoIType<<30);
  
  CoordToHardware convertor;
  Coordinate coord(m_refPhi, m_refEta);
  roiWord |= (convertor.cpCoordinateWord(coord))<<18;
  
  roiWord |= m_Hits;
  
  return roiWord;
}

EmTauROI* LVL1::CPAlgorithm::produceExternal() const {
  
  L1DataDef def;
  std::vector<unsigned int> algorithmType(TrigT1CaloDefs::numOfCPThresholds);
  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();  
  for (std::vector<TriggerThreshold*>::const_iterator it = thresholds.begin();
       it != thresholds.end(); ++it) {
    int numThresh = (*it)->thresholdNumber();
    if (numThresh >= 0 && numThresh < (int)TrigT1CaloDefs::numOfCPThresholds) {
      if ( (*it)->type() == def.emType() ) {
        algorithmType[numThresh] = TrigT1CaloDefs::EMAlg;
      }
      else if ( (*it)->type() == def.tauType() ) {
        algorithmType[numThresh] = TrigT1CaloDefs::TauAlg;
      }
    }
  };
  
  EmTauROI* temp=new EmTauROI(phi(), eta(), Core(), RoIWord(), algorithmType,
                              HadVeto(), HadIsol(), EMIsol(), EMClus(), TauClus());


  return temp;

}

} // end of namespace bracket


