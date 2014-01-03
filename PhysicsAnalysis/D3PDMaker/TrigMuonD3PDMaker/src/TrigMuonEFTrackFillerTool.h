// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFTrackFillerTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFTRACKFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFTRACKFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFTrack.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the information about one TrigMuonEFTrack object
    *
    *         This tool is used in TrigMuonEFInfoD3PDObject to save the information
    *         about the tracks found inside the EventFilter RoI.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TrigMuonEFTrackFillerTool : public BlockFillerTool< TrigMuonEFTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFTrackFillerTool( const std::string& type, const std::string& name,
                                 const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFTrack& muon );

   private:
      bool m_saveFitResults;
      bool m_saveHitInfo;

      float* m_charge;
      float* m_d0;
      float* m_z0;
      float* m_chi2;
      float* m_chi2prob;
      float* m_posX;
      float* m_posY;
      float* m_posZ;

      int* m_nRpcHitsPhi;
      int* m_nTgcHitsPhi;
      int* m_nCscHitsPhi;
      int* m_nRpcHitsEta;
      int* m_nTgcHitsEta;
      int* m_nCscHitsEta;
      int* m_nMdtHits;

   }; // class TrigMuonEFTrackFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFTRACKFILLERTOOL_H
