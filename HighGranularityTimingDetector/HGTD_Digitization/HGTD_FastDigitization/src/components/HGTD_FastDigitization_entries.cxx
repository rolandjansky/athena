/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "HGTD_FastDigitization/HGTD_SmearedDigitizationTool.h"

DECLARE_TOOL_FACTORY(HGTD_SmearedDigitizationTool)

DECLARE_FACTORY_ENTRIES(HGTD_FastDigitization) {
  DECLARE_TOOL(HGTD_SmearedDigitizationTool)
}
