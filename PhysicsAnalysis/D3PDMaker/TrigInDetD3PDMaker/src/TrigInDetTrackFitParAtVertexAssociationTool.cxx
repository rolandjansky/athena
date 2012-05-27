/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParAtVertexAssociationTool.cxx 340544 2011-01-18 12:51:04Z krasznaa $

// Local include(s):
#include "TrigInDetTrackFitParAtVertexAssociationTool.h"

namespace D3PD {

   TrigInDetTrackFitParAtVertexAssociationTool::
   TrigInDetTrackFitParAtVertexAssociationTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
      : SingleAssociationTool< TrigVertex, TrigInDetTrackFitPar >( type, name, parent ) {

   }

   const TrigInDetTrackFitPar*
   TrigInDetTrackFitParAtVertexAssociationTool::get( const TrigVertex& vertex ) {

      TrigVertex& v = const_cast< TrigVertex& >( vertex );
      return v.getMotherTrack();
   }

}
