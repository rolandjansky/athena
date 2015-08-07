/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigRingerNeuralHypo.h
 * @author Danilo Enoque Ferreira de Lima 
 * $Author: dferreir $
 * $Date: 2009-01-12 17:51:36 $
 *
 * @brief Describes a ring neural processor
 */


#ifndef TRIGRINGERNEURALHYPO_H
#define TRIGRINGERNEURALHYPO_H

#include <string>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigCaloEvent/RingerRings.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
//#include "TrigCaloEvent/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigCalo/TrigEMCluster.h"
//#include "CaloEvent/CaloSampling.h"
#include "CaloGeoHelpers/CaloSampling.h"

class TrigRingerNeuralHypo: public HLT::HypoAlgo {
  
 public:

  TrigRingerNeuralHypo(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigRingerNeuralHypo(){};
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  private:
  
  // Properties:
  bool m_acceptAll;
  float m_cut; ///< the cut for electrons/jets (left are electrons)
  float m_emEtCut; ///< The cut on eletromagnetic Et. Applied if it's greater than zero.
  float m_decision;

  std::string m_hlt_feature;
};

#endif /* TRIGRINGERNEURALHYPO_H */
