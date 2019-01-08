/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPAlgorithm.h  -  description
                             -------------------
    begin                : Tues Sep 11 2007
                           Loosely evolved from Ed Moyse's InternalEmTauRoI
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#ifndef CPALGORITHM_H
#define CPALGORITHM_H

#include <map>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/EmTauROI.h"
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
The CPAlgorithm:<br>
  - identifies towers used in a particular trigger window (4x4 towers) <br>
  - forms sums required for trigger algorithm <br>
  - identifies whether passes the RoI condition (local ET maximum) <br>
  - returns sums (with appropriate precision) <br>
  - returns hit bits and RoI word (if all requirements met) 
  */

class CPAlgorithm {
public: 
  
  CPAlgorithm( double eta, double phi, const CPMTowerMap_t* ttContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config, int slice = -1);

  ~CPAlgorithm();
  
  /** Accessors */
  double eta() const;
  double phi() const;
  int Core() const;
  int EMClus() const;
  int TauClus() const;
  int EMIsol() const;
  int HadIsol() const;
  int HadVeto() const;
  bool isEtMax() const;
  unsigned int Hits() const;
  unsigned int RoIWord() const;
  
  /** Put a new EmTauROI object (corresponding to this window) on the heap, and return a pointer to it*/
  EmTauROI* produceExternal() const;
  
private: //atribs
  double m_refEta;
  double m_refPhi;
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

  /** Algorithm results */
  double m_eta;
  double m_phi;
  int m_Core;
  int m_EMClus;
  int m_TauClus;
  int m_EMIsol;
  int m_HadCore;
  int m_HadIsol;
  bool m_EtMax;
  unsigned int m_Hits;

  //bool m_debug;

  /** Algorithm parameters */
  static const int m_maxClus;
  static const int m_maxIsol;
  static const double m_maxEta;

  /** threshold values */
  int m_ClusterThreshold;
  int m_emRingIsolationThreshold;
  int m_hadRingIsolationThreshold;
  int m_hadCoreIsolationThreshold;

private:  // methods
  void setRoICoord(double eta, double phi);
  void testEtMax(const std::vector<int>& cores);
  void passesTrigger();
  void getThresholds(TrigConf::TriggerThreshold* thresh);
  bool emAlgorithm();
  bool tauAlgorithm();

};

}
#endif

