// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFCbTrackFillerTool.h 310395 2010-07-14 05:38:11Z kanno $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFCBTRACKFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFCBTRACKFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the information about one TrigMuonEFCbTrack object
    *
    *         Since TrigMuonEFCbTrack inherits from TrigMuonEFTrack, this tool
    *         only needs to care about the variables that are provided by
    *         TrigMuonEFCbTrack directly. The rest is handled by the tools developed
    *         for the parent classes.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 310395 $
    * $Date: 2010-07-14 07:38:11 +0200 (Wed, 14 Jul 2010) $
    */
   class TrigMuonEFCbTrackFillerTool : public BlockFillerTool< TrigMuonEFCbTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFCbTrackFillerTool( const std::string& type, const std::string& name,
                                   const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFCbTrack& muon );

   private:
      bool m_saveFitResults;
      bool m_saveHitInfo;

      float* m_matchChi2;

      int*   m_nIdSctHits;
      int*   m_nIdPixelHits;
      int*   m_nTrtHits;

   }; // class TrigMuonEFCbTrackFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFCBTRACKFILLERTOOL_H
