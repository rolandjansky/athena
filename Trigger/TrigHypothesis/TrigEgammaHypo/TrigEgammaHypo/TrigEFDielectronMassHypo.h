/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFDielectronMassHypo.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DielectronMassHypo by R. Goncalo
 **
 **   Author: T. Hrynova  <thrynova@mail.cern.ch>
 **
 **   Created:   Nov 13 2009
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFDielectronMassHypo_H 
#define TRIG_TrigEFDielectronMassHypo_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
// trigger EDM
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"

/* class StoreGateSvc; */
/* class TriggerElement; */


/**
 * \class TrigEFDielectronMassHypo
 * \brief TrigEFDielectronMassHypo is a Trigger Hypo Algorithm that retrieves the coomon vertex and
 * inv mass calculated by TrigEFDielectronFex
 * Apply inv mass cuts (upper and lower cut) to the two electrons and activates TE if condition is 
 * satisfied
 *
 */

class TrigEFDielectronMassHypo: public HLT::HypoAlgo  {
  
public:
    TrigEFDielectronMassHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFDielectronMassHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:
  bool checkMasses(const xAOD::TrigCompositeContainer* masses, 
          const std::string &key, const float mass_min, const float mass_max);
  bool checkComposites(const xAOD::TrigCompositeContainer* masses);

private:
  bool m_useElectronElectron;
  bool m_useElectronCluster;

  // cuts
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_lowerMassElectronClusterCut; //!<  lower inv mass cut (e,cluster)
  float m_upperMassElectronClusterCut; //!<  upper inv mass cut (e,cluster)
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll;  //!<  true will accept all events
  // monitoring
  double m_monMassAccepted;
  int    m_monCut;
};

#endif
