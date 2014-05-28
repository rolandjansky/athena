/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#ifndef TRIGMUGIRLALG_H
#define TRIGMUGIRLALG_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"

//* Trigger includes *//
#include <string>
#include "TrigInterfaces/FexAlgo.h"

// for the optional output object
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "TrigMuonToolInterfaces/TrigMuonEFMonVars.h"

// Tracking includes
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

namespace HLT {
  class TriggerElement;
}

class ITrigMuGirlTool;

class TrigMuGirlAlg : public HLT::FexAlgo {
  
  public:
  
  TrigMuGirlAlg(const std::string&, ISvcLocator*);
  ~TrigMuGirlAlg();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* , HLT::TriggerElement*);
  HLT::ErrorCode hltFinalize(); 
  
  private:

    template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
	      msg() << MSG::ERROR << "Cannot retrieve " << pHandle << endreq;
            else
	      msg() << MSG::WARNING << "Cannot retrieve " << pHandle << endreq;
        }
        else
	  msg() << MSG::INFO << "Retrieved " << pHandle << endreq;
        return sc;
    }

    // Tool Handles
    ToolHandle<ITrigMuGirlTool>                      m_trigMuGirlTool;       /**< The MuGirl Trigger Tool */
    ToolHandle< Trk::ITrackParticleCreatorTool >     m_particleCreatorTool;  /**< The ID Particle Creator Tool */


    std::string                  m_idTrackParticlesName;

    BooleanProperty              m_doMuonFeature;

    TrigMuonEFInfoContainer*     m_eMuonEFInfoCont;
    Rec::TrackParticleContainer* m_eTrkParCont;

    TrigMuGirlMonVars            m_TrigMuGirlMonVars;
    std::vector<TrigTimer*>      m_TrigMuGirlTimers;
};

#endif
