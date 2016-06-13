/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2DielectronMassFex.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DimuonMassHypo by C.Schiavi
 **
 **   Author: R.Goncalo  <r.goncalo@rhul.ac.uk>
 **
 **   Created:   Aug 13 2006
 **   Modified:  Jan 19 2007 Valeria Perez Reale Adapted to New Steering  
 **   Modified:  Apr 28 2007 Tomasz Bold major changes to run with new steering
 **              V. Perez Reale added doxygen comments 26/6/07
 **
 **************************************************************************/ 

#ifndef TRIGEGAMMAHYPO_TRIGL2DIELECTRONMASSFEX_H 
#define TRIGEGAMMAHYPO_TRIGL2DIELECTRONMASSFEX_H

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
#include "TrigInterfaces/ComboAlgo.h"

// trigger EDM
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"

/* class StoreGateSvc; */
/* class TriggerElement; */

/**
 * \class TrigL2DielectronMassFex
 * \brief TrigL2DielectronMassFex is a Trigger Fex Algorithm that retrieves the L2 TrigElectron
 * container
 * check charge: track pT is signed according to charge but not electron pT
 * check vertex position: two electrons should come from common 
 * reject combination of electrons if distance squared between perigee of tracks is greater than a value
 * (should accept 99% of good combinations)
 * apply loose inv mass cuts (upper and lower cut) to the two electrons
 *
 * (May 2011) 
 * Add use cluster mode for chains to collect J/psi sample for Tag & Probe Analysis
 * Change to attach the calculated invariant mass to outputTE as TrigOperationalInfo
 */

namespace TrigL2DielectronMassHelpers {


  bool validElectrons(const xAOD::TrigElectron* electron1, const xAOD::TrigElectron* electron2);

  
  enum Vertex {  Common, NotCommon, Unapplicable };
  Vertex commonVertex(const xAOD::TrigElectron* electron1, const xAOD::TrigElectron* electron2);

  bool opositeCharge(const xAOD::TrigElectron* electron1, const xAOD::TrigElectron* electron2);

  double invariantMass(const xAOD::TrigElectron* electron1, const xAOD::TrigElectron* electron2, const double electronMass);
  double invariantMass(const xAOD::TrigElectron* electron, const xAOD::TrigEMCluster* cluster, const double electromMass);
  
}


class TrigL2DielectronMassFex: public HLT::ComboAlgo  {
  
public:
  TrigL2DielectronMassFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2DielectronMassFex();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
private:
  // use cluster mode: Calculate inv. mass from TrigElectron and TrigEMCluster
  // This mode will be used in chains to collect sample for Tag & Probe
  // In such chains no cut will be apply to probe RoI object therfore 
  // TrigElectron is not necessary
  bool m_useClusterAsProbe; //!< true will use TrigEMCluster as probe object
  // Electron mass(use in calculation Mee)
  float m_electronMass; //! in MeV

  // container to hold the calculated inariant mass
  std::vector<float> m_massCont;
  xAOD::TrigCompositeContainer *m_cont;

  // cuts (not used in use cluster mode)
  bool  m_electronValid; //!< true if electrons have different RoI positions 
  bool  m_sameTrackAlgo; //!<  true if tracks come from same track algo
  bool  m_oppositeCharge; //!<  true if electtons have opposite charge
  bool  m_commonVertex; //!<  true if electrons tracks have common vertex
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll; //!<  true will accept all events

  // monitoring
  double m_monMassAccepted;
  int    m_monCut;
};




#endif
