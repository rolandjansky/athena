/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2DielectronMassHypo.h
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

#ifndef TRIG_TrigL2DielectronMassHypo_H 
#define TRIG_TrigL2DielectronMassHypo_H

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
//#include "TrigInDetEvent/TrigInDetTrack.h"
//#include "TrigInDetEvent/TrigInDetTrackCollection.h"
//#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"

/* class StoreGateSvc; */
/* class TriggerElement; */


/**
 * \class TrigL2DielectronMassHypo
 * \brief TrigL2DielectronMassHypo is a Trigger Hypo Algorithm that retrieves the coomon vertex and
 * inv mass calculated by TrigL2DielectronFex
 * Apply inv mass cuts (upper and lower cut) to the two electrons and activates TE if condition is 
 * satisfied
 *
 */

class TrigL2DielectronMassHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2DielectronMassHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DielectronMassHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  private:
        
    // cuts
    bool  m_sameTrackAlgo; //!<  true if tracks come from same track algo
    bool  m_oppositeCharge; //!<  true if electtons have opposite charge
    bool  m_commonVertex;  //!<  true if electrons tracks have common vertex
    bool  m_electronValid;
    float m_lowerMassCut; //!<  lower inv mass cut
    float m_upperMassCut; //!<  upper inv mass cut

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;  //!<  true will accept all events
    // monitoring
    double m_monMassAccepted;
    int    m_monCut;
};

#endif
