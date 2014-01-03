// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureTileMuFeatureAssociationTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TILETRACKMUFEATURETILEMUFEATUREASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TILETRACKMUFEATURETILEMUFEATUREASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileMuFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" TileMuFeature objects to TileTrackMuFeature ones
    *
    *         A TileTrackMuFeature object have a TileMuFeature object associated
    *         to it, or not. This relationship can be expressed towards the D3PDMaker
    *         code using this associator tool.
    *
    * @author Takayuki Kanno
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TileTrackMuFeatureTileMuFeatureAssociationTool :
      public SingleAssociationTool< TileTrackMuFeature, TileMuFeature > {

   public:
      /// Regular AlgTool constructor
      TileTrackMuFeatureTileMuFeatureAssociationTool( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent );

      /// Function performing the association
      virtual const TileMuFeature* get( const TileTrackMuFeature& muon );

   }; // class TileTrackMuFeatureTileMuFeatureAssociationTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TILETRACKMUFEATURETILEMUFEATUREASSOCIATIONTOOL_H
