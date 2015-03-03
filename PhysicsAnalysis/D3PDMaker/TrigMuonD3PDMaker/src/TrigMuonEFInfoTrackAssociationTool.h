// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackAssociationTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/MultiAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool associating TrigMuonEFInfoTrack objects to TrigMuonEFInfo objects
    *
    *         This is a tricky tool. Each TrigMuonEFInfo object describes one EF
    *         muon RoI. But the EF algorithm is not restricted to only reconstructing
    *         one muon track per RoI. It can reconstruct any number of them. So each
    *         TrigMuonEFInfo object holds a collection of TrigMuonEFInfoTrack objects.
    *
    *         This tool associates each of the objects in the linked
    *         TrigMuonEFInfoTrackContainer to the original TrigMuonEFInfo object. This
    *         way making it possible for the framework to loop over those objects.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TrigMuonEFInfoTrackAssociationTool :
      public MultiAssociationTool< TrigMuonEFInfo, TrigMuonEFInfoTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoTrackAssociationTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

      /// Initialize the tool with a new object
      virtual StatusCode reset( const TrigMuonEFInfo& ef_info );
      /// Return the next associated object
      virtual const TrigMuonEFInfoTrack* next();

   private:
      const TrigMuonEFInfoTrackContainer* m_tracks; ///< Pointer to the track collection
      TrigMuonEFInfoTrackContainer::const_iterator m_currentTrack; ///< Pointer to the current track

   }; // class TrigMuonEFInfoTrackAssociationTool 

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOTRACKASSOCIATIONTOOL_H
