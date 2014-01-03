// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackFillerTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the properties of TrigMuonEFInfoTrack objects
    *
    *         TrigMuonEFInfoTrack objects store very little information themselves.
    *         Most of the information is in the tracks that they point to. This tool
    *         is just used to save this very small amount of information directly from
    *         the TrigMuonEFInfoTrack objects.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TrigMuonEFInfoTrackFillerTool : public BlockFillerTool< TrigMuonEFInfoTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoTrackFillerTool( const std::string& type, const std::string& name,
                                     const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFInfoTrack& muon );

   private:
      int* m_muonType;

   }; // class TrigMuonEFInfoTrackFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKFILLERTOOL_H
