/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAccessors_v1.cxx 584400 2014-02-20 12:32:03Z jerdmann $

// System include(s):
#include <iostream>

// Local include(s):
#include "BTagVertexAccessors_v1.h"


/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::NAME:                                                \
   {                                                               \
     static SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {
    
  

} // namespace xAOD
