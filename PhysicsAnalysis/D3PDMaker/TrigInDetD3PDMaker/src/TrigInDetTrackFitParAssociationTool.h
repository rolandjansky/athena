// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParAssociationTool.h 340544 2011-01-18 12:51:04Z krasznaa $
#ifndef TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARASSOCIATIONTOOL_H
#define TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" TrigInDetTrackFitPar objects to TrigInDetTracks
    *
    *         This tool returns to the framework the TrigInDetTrackFitPar object
    *         which is given by the TrigInDetTrack::param() function.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 340544 $
    * $Date: 2011-01-18 13:51:04 +0100 (Tue, 18 Jan 2011) $
    */
   class TrigInDetTrackFitParAssociationTool :
      public SingleAssociationTool< TrigInDetTrack, TrigInDetTrackFitPar > {

   public:
      /// Regular AlgTool constructor
      TrigInDetTrackFitParAssociationTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );

      /// Function performing the association
      virtual const TrigInDetTrackFitPar* get( const TrigInDetTrack& track );

   }; // class TrigInDetTrackFitParAssociationTool

} // namespace D3PD

#endif // TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARASSOCIATIONTOOL_H
