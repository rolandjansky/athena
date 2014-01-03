// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationFillerTool.h 501675 2012-05-21 13:59:19Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFIsolation.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PD {

   /**
    *  @short Tool filling information about TrigMuonEFIsolation objects
    *
    * @author Takayuki Kanno
    *
    * $Revision: 501675 $
    * $Date: 2012-05-21 15:59:19 +0200 (Mon, 21 May 2012) $
    */
   class TrigMuonEFIsolationFillerTool : public BlockFillerTool< TrigMuonEFIsolation > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFIsolationFillerTool( const std::string& type, const std::string& name,
                                     const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFIsolation& muon );

   private:

      /// Property: Switch for saving the ptcone variables
      bool m_savePtCone;
      /// Property: Switch for saving the etcone variables
      bool m_saveEtCone;
      /// Property: Switch for saving the track position index
      bool m_saveTrackPosition;

      /// sum of pT of tracks in 0.2 cone
      float*  m_sumTrkPtCone02;
      /// sum of pT of tracks in 0.3 cone
      float*  m_sumTrkPtCone03;

      /// sum of calorimeter Et in 0.1 cone
      float*  m_sumEtCone01;
      /// sum of calorimeter Et in 0.2 cone
      float*  m_sumEtCone02;
      /// sum of calorimeter Et in 0.3 cone
      float*  m_sumEtCone03;
      /// sum of calorimeter Et in 0.4 cone
      float*  m_sumEtCone04;

      /// Position of the muon in the TrigMuonEFInfoTrack container
      int*    m_trackPosition;

   }; // class TrigMuonEFIsolationFillerTool

} // namespace D3PD

#endif //TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONFILLERTOOL_H
