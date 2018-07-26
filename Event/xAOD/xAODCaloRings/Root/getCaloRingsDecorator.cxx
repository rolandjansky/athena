/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getCaloRingsDecorator.cxx 707323 2015-11-12 02:45:01Z wsfreund $
// Local includes:
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"

namespace xAOD {

//==============================================================================
caloRingsReader_t* getCaloRingsReader()
{
  static caloRingsReader_t caloRingsReader( caloRingsLinksDecorKey() );
  return &caloRingsReader;
}

} // namespace xAOD
