// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoTrackAssocTool.h 493591 2012-04-02 12:06:17Z kanno $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKASSOCTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKASSOCTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    * @short Tool "associating" TrigMuonEFIsolation objects to TrigMuonEFInfoTrack ones
    * @author Takayuki Kanno
    *
    * $Revision: 493591 $
    * $Date: 2012-04-02 14:06:17 +0200 (Mon, 02 Apr 2012) $
    */
   class TrigMuonEFIsolationEFInfoTrackAssocTool :
      public SingleAssociationTool< TrigMuonEFIsolation, TrigMuonEFInfoTrack > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFIsolationEFInfoTrackAssocTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent );

      /// Function performing the association
      virtual const TrigMuonEFInfoTrack* get( const TrigMuonEFIsolation& muon );

   }; // class TrigMuonEFIsolationEFInfoTrackAssocTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKASSOCTOOL_H
