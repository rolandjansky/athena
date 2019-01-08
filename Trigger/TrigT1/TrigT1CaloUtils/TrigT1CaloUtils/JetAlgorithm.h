/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetAlgorithm.h  -  description
                             -------------------
    begin                : Tues Sep 11 2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#ifndef JETALGORITHM_H
#define JETALGORITHM_H

#include <map>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloUtils/JetInputKey.h"


namespace TrigConf {
   class TriggerThreshold;
}

/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**
This is an internal class, used in the jet trigger. <P>
The JetAlgorithm:<br>
  - identifies JetInputs used in a particular trigger window (4x4 towers) <br>
  - forms sums required for trigger algorithm <br>
  - identifies whether passes the RoI condition (local ET maximum) <br>
  - returns sums (with appropriate precision) <br>
  - returns hit bits and RoI word (if all requirements met) 
  */

class JetAlgorithm {
public: 
  
  JetAlgorithm( double eta, double phi, const std::map<int, JetInput *>* jiContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config);

  ~JetAlgorithm();
  
  /** Accessors */
  double eta() const;
  double phi() const;
  int Core() const;
  int ET4x4() const;
  int ET6x6() const;
  int ET8x8() const;
  bool isEtMax() const;
  unsigned int Hits() const;
  unsigned int RoIWord() const;
  
  /** Put a new EmTauROI object (corresponding to this window) on the heap, and return a pointer to it*/
  JetROI* produceExternal() const;
  
private: //atribs
  double m_refEta;
  double m_refPhi;
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

  /** Algorithm results */
  double m_eta;
  double m_phi;
  int m_ET4x4;
  int m_ET6x6;
  int m_ET8x8;
  bool m_EtMax;
  unsigned int m_Hits;

  bool m_debug;

  /** Algorithm parameters */
  static const int m_sat;

private:  // methods
  void setRoICoord(double eta, double phi);
  void testEtMax(const std::vector<int>& cores);
  void passesTrigger();
  void getThresholds(TrigConf::TriggerThreshold* thresh, int& size, int& value);
  bool isForwardJet() const;

};

}
#endif

