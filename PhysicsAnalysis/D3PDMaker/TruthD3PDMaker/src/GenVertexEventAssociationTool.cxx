/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexEventAssociationTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $

// Local include(s):
#include "GenVertexEventAssociationTool.h"

namespace D3PD {

   GenVertexEventAssociationTool::GenVertexEventAssociationTool( const std::string& type,
                                                                     const std::string& name,
                                                                     const IInterface* parent )
      : Base( type, name, parent ) {

   }

   const HepMC::GenEvent* GenVertexEventAssociationTool::get( const HepMC::GenVertex& p ) {

      return p.parent_event();
   }

} // namespace D3PD
