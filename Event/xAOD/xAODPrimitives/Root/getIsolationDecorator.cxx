/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODPrimitives/tools/getIsolationDecorator.h"

// System include(s):
#include <iostream>

/// Helper macro for Accessor objects
#define DEFINE_DECORATOR(TYPE)                                 \
  case xAOD::Iso::TYPE:                                       \
{                                                             \
  static const SG::AuxElement::Decorator< float > a( #TYPE);   \
  return &a;                                                  \
}                                                             \
break;


namespace xAOD {

const SG::AuxElement::Decorator< float >*
  getIsolationDecorator( Iso::IsolationType type ) {
    switch( type ) {
      //EtCone variables      
      DEFINE_DECORATOR( etcone20 );
      DEFINE_DECORATOR( etcone30 );
      DEFINE_DECORATOR( etcone40 );
        //PtCone variables      
      DEFINE_DECORATOR( ptcone20 );
      DEFINE_DECORATOR( ptcone30 ); 
      DEFINE_DECORATOR( ptcone40 ); 
      DEFINE_DECORATOR( ptcone50 ); 
      // TopoEtcone variables:
      DEFINE_DECORATOR( topoetcone20 );
      DEFINE_DECORATOR( topoetcone30 );
      DEFINE_DECORATOR( topoetcone40 );
      // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables:
      DEFINE_DECORATOR(ptvarcone20);
      DEFINE_DECORATOR(ptvarcone30);
      DEFINE_DECORATOR(ptvarcone40);
      // neutral eflow
      DEFINE_DECORATOR(neflowisol20);
      DEFINE_DECORATOR(neflowisol30);
      DEFINE_DECORATOR(neflowisol40);
    default:
      std::cerr << "xAOD::Primitives  ERROR Unknown float IsolationType ("
        << type << ") requested" << std::endl;
      return nullptr;
    }
  }
}// namespace xAOD
