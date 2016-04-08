/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexAlgo.h
 **
 **   Description: - Combo algorithm: search for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo 
					and TrigEFBMuMuFex.cxx by J. Kirk
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Feb 13, 2011
 **
 **************************************************************************/

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEFEXALGO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEFEXALGO_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
#include <vector>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"

// trigger EDM
#include "egammaEvent/egammaContainer.h"

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"


/* class StoreGateSvc; */
/* class TriggerElement; */

/**
 * \class TrigEFElectronMuonAngleFexAlgo
 * \brief TrigEFElectronMuonAngleFexAlgo is a Trigger Fex Algorithm that retrieves the EF egamma
 * container and EF MuID track container
 * Reject electron candidates with bad track (nSiHits < 4) to have good trackParticle object( should be true for medium and better 
 * electrons) Use direction and energy from trackParticle ( egamma recomendation for 2010 data)
 * apply loose cuts on distance (it is possible to cut delta phi and on the full distance dR)
 * check opposite charge: track pT is signed according to charge 
 * check vertex position: two leptons should come from common 
 * reject combination of leptons if distance squared between perigee of tracks is greater than a value
 * (should accept 99% of good combinations)
 * apply loose inv mass cuts (upper and lower cut) to the two leptons
 * 
 */

namespace Reco  { class ITrackToVertex; }


class TrigEFElectronMuonAngleFexAlgo: public HLT::ComboAlgo  {
  
public:
  TrigEFElectronMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFElectronMuonAngleFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

    
private:
  // cuts
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MaxDPhi; //!< upper cut on electron-muon Delta Phi
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_DPhi; //!< calculated Delta Phi
  float m_DR; //!< calculated Delta R
  float m_mass; //!< calculated mass
  bool  m_useRecommended; //!< Using Egamma group recommendations for electron direction and energy (2010 data)
  
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll; //!<  true will accept all events

  // monitoring
  double m_monMassAccepted;
  double m_monMassAll;  
  int    m_monCut;
  double m_monDPhiAccepted;
  double m_monDPhiAll;
  double m_monDRAccepted;
  double m_monDRAll;
  double m_monPull;
  int    m_monVxState;
  
  
  Amg::Vector3D m_primaryVertex;
  ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool; 
  
  const Rec::TrackParticle* m_electron; //!< stores pointer to a good electron
  const Trk::Track* m_muon; //!< stores pointer to a good muon

  
  ElectronMuonTopoInfoContainer* m_egMuTopoColl; //!< Pointer to ElectronMuonTopoInfoCollection

  std::vector<const Rec::TrackParticle*> electronCollection; //!< In case more electrons will be present in input collections which would pass the cuts, store them all
  std::vector<const Trk::Track*> muonCollection; //!<In case more electrons will be present in input collections which would pass the cuts, store them all
  	
  
};




#endif
