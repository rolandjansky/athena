/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletVertexAssociationTool.cxx 485019 2012-02-24 16:16:04Z krasznaa $

// Local include(s):
#include "QuadrupletVertexAssociationTool.h"

namespace D3PD {

   QuadrupletVertexAssociationTool::QuadrupletVertexAssociationTool( const std::string& type,
                                                                     const std::string& name,
                                                                     const IInterface* parent )
      : SingleAssociationTool< HSG2::Quadruplet, Trk::VxCandidate >( type, name, parent ) {

   }

   const Trk::VxCandidate* QuadrupletVertexAssociationTool::get( const  HSG2::Quadruplet& q ) {

      return *( q.vertex() );
   }

} // namespace D3PD
