/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include <AFP_Raw2Digi/AFP_Raw2Digi.h>
#include "../AFP_Raw2DigiTool.h"
#include "../AFP_Raw2DigiMultiBcXTool.h"

DECLARE_ALGORITHM_FACTORY( AFP_Raw2Digi )
DECLARE_TOOL_FACTORY( AFP_Raw2DigiTool )
DECLARE_TOOL_FACTORY( AFP_Raw2DigiMultiBcXTool )

DECLARE_FACTORY_ENTRIES( AFP_Raw2Digi ) 
{
  DECLARE_ALGORITHM( AFP_Raw2Digi );
}
