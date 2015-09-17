/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getCaloRingsDecorator.cxx 668862 2015-05-20 20:15:30Z wsfreund $ 
// Local includes:
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"

namespace xAOD {

//==============================================================================
caloRingsDeco_t* getCaloRingsDecorator()
{
  static caloRingsDeco_t caloRingsDeco( "caloRingsELVec" );
  return &caloRingsDeco;
}

//==============================================================================
caloRingsReader_t* getCaloRingsReader()
{
  static caloRingsReader_t caloRingsReader( "caloRingsELVec" );
  return &caloRingsReader;
}

} // namespace xAOD
