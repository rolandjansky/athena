/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxInfo.cxx 598261 2014-05-21 16:36:05Z krasznaa $

// Local include(s):
#include "xAODCore/ShallowAuxInfo.h"

namespace xAOD {

   ShallowAuxInfo::ShallowAuxInfo()
      : ShallowAuxContainer( true ) {

   }

   ShallowAuxInfo::
   ShallowAuxInfo( const DataLink< SG::IConstAuxStore >& parent )
      : ShallowAuxContainer( parent, true ) {

   }

} // namespace xAOD
