/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JEMJetAlgorithm.h  -  description
                             -------------------
    begin                : Thurs Mar 13 2014
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#ifndef JETALGORITHM_H
#define JETALGORITHM_H

#include <map>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "TrigT1CaloEvent/JetInput.h"
#include "xAODTrigL1Calo/JEMTobRoI.h"

/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**
This is an internal class, used in the jet trigger. <P>
The JEMJetAlgorithm:<br>
  - identifies JetInputs used in a particular trigger window (4x4 towers) <br>
  - forms sums required for trigger algorithm <br>
  - identifies whether passes the RoI condition (local ET maximum) <br>
  - returns sums (with appropriate precision) <br>
  - returns hit bits and RoI word (if all requirements met) 
  */

class JEMJetAlgorithm {
public: 
  
  JEMJetAlgorithm( double eta, double phi, const std::map<int, JetInput *>* jiContainer, ServiceHandle<TrigConf::ITrigConfigSvc> config);

  ~JEMJetAlgorithm();
  
  /** Accessors */
  double eta();
  double phi();
  int Core();
  int ET4x4();
  int ET6x6();
  int ET8x8();
  int ETLarge();
  int ETSmall();
  bool isEtMax();
  bool isRoI();

  /** Create JEMTobRoI and return pointers to it.
      Will return a null pointer if object does not pass hypothesis.
      It is the user's responsibility to check */
  xAOD::JEMTobRoI* jemTobRoI();
  
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
  int m_ETLarge;
  int m_ETSmall;  
  bool m_EtMax;

  //bool m_debug;

  /** Algorithm parameters */
  static const int m_satLarge;
  static const int m_satSmall;

private:  // methods
  void setRoICoord(double eta, double phi);
  void testEtMax(const std::vector<int>& cores);
  void passesTrigger();

};

}
#endif

