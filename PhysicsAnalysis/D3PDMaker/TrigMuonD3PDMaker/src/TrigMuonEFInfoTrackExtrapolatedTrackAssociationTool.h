// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKEXTRAPOLATEDTRACKASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKEXTRAPOLATEDTRACKASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool associating extrapolated (standalone) TrigMuonEFTrack objects
    *
    *         This tool associates the extrapolated TrigMuonEFTrack object
    *         to a given TrigMuonEFInfoTrack object. Each TrigMuonEFInfoTrack
    *         object has pointers to 3 tracks. The extrapolated track is
    *         one of them.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool :
      public SingleAssociationTool< TrigMuonEFInfoTrack, TrigMuonEFTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool( const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent );

      /// Get the associated TrigMuonEFTrack
      virtual const TrigMuonEFTrack* get( const TrigMuonEFInfoTrack& info_track );

   }; // class TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKEXTRAPOLATEDTRACKASSOCIATIONTOOL_H
