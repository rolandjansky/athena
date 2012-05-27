// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParAtVertexAssociationTool.h 340544 2011-01-18 12:51:04Z krasznaa $
#ifndef TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARATVERTEXASSOCIATIONTOOL_H
#define TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARATVERTEXASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" TrigInDetTrackFitPar objects to TrigVertex objects
    *
    * $Revision: 340544 $
    * $Date: 2011-01-18 13:51:04 +0100 (Tue, 18 Jan 2011) $
    */
   class TrigInDetTrackFitParAtVertexAssociationTool :
      public SingleAssociationTool< TrigVertex, TrigInDetTrackFitPar > {

   public:
      /// Regular AlgTool constructor
      TrigInDetTrackFitParAtVertexAssociationTool( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent );

      /// Function performing the association
      virtual const TrigInDetTrackFitPar* get( const TrigVertex& vertex );

   }; // class TrigInDetTrackFitParAtVertexAssociationTool

} // namespace D3PD

#endif // TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARATVERTEXASSOCIATIONTOOL_H
