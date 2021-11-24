/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"
#include "AFP_LocReco/AFP_SIDLocReco.h"
#include "AFP_LocReco/AFPSiDBasicKalmanTool.h"
#include "AFP_LocReco/AFPTDBasicTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(AFP_TDLocReco)
DECLARE_ALGORITHM_FACTORY(AFP_SIDLocReco)
DECLARE_TOOL_FACTORY(AFPSiDBasicKalmanTool)
DECLARE_TOOL_FACTORY(AFP_SIDLocRecoTool)
DECLARE_TOOL_FACTORY(AFPTDBasicTool)
DECLARE_TOOL_FACTORY(AFP_TDLocRecoTool)

DECLARE_FACTORY_ENTRIES(AFP_LocReco) {
	DECLARE_ALGORITHM  (AFP_TDLocReco)
	DECLARE_ALGORITHM  (AFP_SIDLocReco)
}
