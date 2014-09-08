// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronHypoMoni.h
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after 
 **                tracking algorithms: creates TrigElectrons. with 
 **                montoring histograms added.
 **
 **   Authour: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     Tue Oct 17 James Keates added monitoring.
 **                 Jan 29 2007 Valeria Perez Reale-migration to new steering 
 **                 V. Perez Reale added doxygen comments 26/6/07
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2ElectronHypoMoni_H 
#define TRIG_TrigL2ElectronHypoMoni_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
//#include "TrigSteeringEvent/key2keyStore.h"
// trigger includes
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
// trigger EDM
///#include "TrigInDetEvent/TrigInDetTrack.h"
//#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
//#include "TrigParticle/TrigElectron.h"
//#include "TrigParticle/TrigElectronContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "TH1F.h"


class StoreGateSvc;
class TriggerElement;
//class ITrigInDetTrackExtrapolator;
class IExtrapolateToCaloTool;
class TH1F;


/**
 * \class TrigL2ElectronHypoMoni
 * \brief TrigL2ElectronHypoMoni is a Trigger Hypo Algorithm that does the same selection as
 *  TrigL2ElectronHypo but then also adds monitoring histograms
 *
 *
 */

class TrigL2ElectronHypoMoni: public HLT::HypoAlgo  {
  
  public:
    TrigL2ElectronHypoMoni(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2ElectronHypoMoni();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:

    // Properties:

    TH1F* m_trackPtHist;
    TH1F* m_CaloTrackdETAHist;
    TH1F* m_CaloTrackdPHIHist;
    TH1F* m_CaloTrackdEoverPHist;
    TH1F* m_trackAlgoIDHist;
    TH1F* m_dETACLUSTERHist;
    TH1F* m_dPHICLUSTERHist;
    
    StoreGateSvc* m_storeGate;
    
    unsigned int m_trackalgoID;

    //L1-L2 matching variables
    float  m_detacluster;
    float  m_dphicluster;

    //tracking cut
    float  m_trackPtthr;
    
    //calo-tracking cuts
    std::vector<float> m_etabin;
    std::vector<float> m_calotrackdeta; //!<  deta between calo and track
    std::vector<float> m_calotrackdphi; //!<  dphi between calo and track
    std::vector<float> m_calotrackdeoverp_low; //!<  E/p lower cut between calo and track
    std::vector<float> m_calotrackdeoverp_high; //!<  E/p upper cut between calo and track 

    //radius and Z of calorimeter face
    float m_RCAL; //!<  radious of calorimeter face
    float m_ZCAL; //!<  Z of calorimeter face

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;  //!<  accept all events if true



  //ITrigInDetTrackExtrapolator* m_trackExtrapolator;
  ToolHandle<IExtrapolateToCaloTool> m_trackExtrapolator;

};

#endif
