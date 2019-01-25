/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPMTobAlgorithm.h  -  description
                             -------------------
    begin                : Thu 6 Mar 2014
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#ifndef CPMTobAlgorithm_H
#define CPMTobAlgorithm_H

#include <map>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTobRoI.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"

namespace TrigConf {
   class TriggerThreshold;
}


/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**
This is an internal class, used in the EM/Tau trigger. <P>
The CPMTobAlgorithm:<br>
  - identifies towers used in a particular trigger window (4x4 towers) <br>
  - forms sums required for trigger algorithm <br>
  - identifies whether passes the RoI condition (local ET maximum) <br>
  - returns sums (with appropriate precision) <br>
  - returns hit bits and RoI word (if all requirements met) 
  */

class CPMTobAlgorithm {
public: 
  
  CPMTobAlgorithm(double eta, double phi, const xAOD::CPMTowerMap_t* ttContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config, int slice = -1);

  ~CPMTobAlgorithm();
  
  /** Accessors for TOB data */
  double eta();
  double phi();
  int EMClusET();
  int EMIsolWord();
  int TauClusET();
  int TauIsolWord();
  
  bool isEMRoI();
  bool isTauRoI();
  bool isEtMax();

  /** Additional information for reconstruction & performance studies */
  int CoreET();
  int EMCoreET();
  int HadCoreET();
  int EMIsolET();
  int HadIsolET();
  
  /** Sums with the range & precision of isolation LUT inputs */
  int EMLUTClus();
  int EMLUTEMIsol();
  int EMLUTHadVeto();
  int TauLUTClus();
  int TauLUTEMIsol();
  
  /** Create CPMTobRoIs and return pointers to them.
      Will return a null pointer if object does not pass hypothesis.
      It is the user's responsibility to check */
  xAOD::CPMTobRoI* EMCPMTobRoI();
  xAOD::CPMTobRoI* TauCPMTobRoI();
    
private: //atribs
  double m_refEta;
  double m_refPhi;
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

  /** Algorithm results */
  double m_eta;
  double m_phi;
  unsigned int m_Core;
  unsigned int m_EMClus;
  unsigned int m_TauClus;
  unsigned int m_EMIsolWord;
  unsigned int m_TauIsolWord;
  unsigned int m_EMCore;
  unsigned int m_EMIsol;
  unsigned int m_HadCore;
  unsigned int m_HadIsol;
  bool m_EtMax;
  bool m_EMThresh;
  bool m_TauThresh;

  //bool m_debug;

  /** Algorithm parameters */
  static const unsigned int m_maxClus;
  
  static const unsigned int m_emLUT_ClusterFirstBit;
  static const unsigned int m_emLUT_ClusterNBits;
  static const unsigned int m_emLUT_EMIsolFirstBit;
  static const unsigned int m_emLUT_EMIsolNBits;
  static const unsigned int m_emLUT_HadVetoFirstBit;
  static const unsigned int m_emLUT_HadVetoNBits;
  
  static const unsigned int m_tauLUT_ClusterFirstBit;
  static const unsigned int m_tauLUT_ClusterNBits;
  static const unsigned int m_tauLUT_EMIsolFirstBit;
  static const unsigned int m_tauLUT_EMIsolNBits;
  
  static const unsigned int m_noIsol;

  static const double m_maxEta;

  /** threshold values */
  /* vscharf 2015-04-22: not used anymore causing Coverity issue 19609 */
  /* int m_ClusterThreshold; */
  /* int m_emRingIsolationThreshold; */
  /* int m_hadRingIsolationThreshold; */
  /* int m_hadCoreIsolationThreshold; */

private:  // methods
  void setRoICoord(double eta, double phi);
  void testEtMax(const std::vector<unsigned int>& cores);
  void emAlgorithm();
  void tauAlgorithm();
  xAOD::CPMTobRoI* createTobRoI(int type);

};

}
#endif

