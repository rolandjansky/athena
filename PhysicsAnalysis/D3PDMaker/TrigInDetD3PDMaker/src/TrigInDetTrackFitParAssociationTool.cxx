/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParAssociationTool.cxx 340544 2011-01-18 12:51:04Z krasznaa $

// Local include(s):
#include "TrigInDetTrackFitParAssociationTool.h"

namespace D3PD {

   TrigInDetTrackFitParAssociationTool::
   TrigInDetTrackFitParAssociationTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
      : SingleAssociationTool< TrigInDetTrack,
                               TrigInDetTrackFitPar >( type, name, parent ) {

   }

   const TrigInDetTrackFitPar*
   TrigInDetTrackFitParAssociationTool::get( const TrigInDetTrack& track ) {

      return track.param();
   }

}
