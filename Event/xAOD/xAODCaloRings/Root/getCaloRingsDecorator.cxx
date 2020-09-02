/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"

namespace xAOD {

//==============================================================================
const caloRingsReader_t& getCaloRingsReader()
{
  static const caloRingsReader_t caloRingsReader( caloRingsLinksDecorKey() );
  return caloRingsReader;
}

} // namespace xAOD
