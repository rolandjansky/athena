// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureIDTrackAssociationTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TILETRACKMUFEATUREIDTRACKASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TILETRACKMUFEATUREIDTRACKASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" TrigInDetTrack objects to TileTrackMuFeature ones
    *
    *         A TileTrackMuFeature object can have a TrigInDetTrack object associated
    *         to it, or not. This relationship can be expressed towards the D3PDMaker
    *         code using this associator tool. It tells the framework which
    *         TrigInDetTrack object belongs to a given TileTrackMuFeature object.
    *
    * @author Takayuki Kanno
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TileTrackMuFeatureIDTrackAssociationTool :
      public SingleAssociationTool< TileTrackMuFeature, TrigInDetTrack > {

   public:
      /// Regular AlgTool constructor
      TileTrackMuFeatureIDTrackAssociationTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent );

      /// Function performing the association
      virtual const TrigInDetTrack* get( const TileTrackMuFeature& muon );

   }; // class TileTrackMuFeatureIDTrackAssociationTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TILETRACKMUFEATUREIDTRACKASSOCIATIONTOOL_H
