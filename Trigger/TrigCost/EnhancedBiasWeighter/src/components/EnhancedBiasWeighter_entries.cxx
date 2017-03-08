/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "EnhancedBiasWeighter/EnhancedBiasWeighter.h"

DECLARE_TOOL_FACTORY( EnhancedBiasWeighter )

DECLARE_FACTORY_ENTRIES( EnhancedBiasWeighter ) 
{
  DECLARE_TOOL( EnhancedBiasWeighter );
}
