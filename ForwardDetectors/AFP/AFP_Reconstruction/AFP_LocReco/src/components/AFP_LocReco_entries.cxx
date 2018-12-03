/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"
#include "AFP_LocReco/AFP_SIDLocReco.h"
#include "AFP_LocReco/AFPSiDBasicKalmanTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(AFP_TDLocReco)
DECLARE_ALGORITHM_FACTORY(AFP_SIDLocReco)
DECLARE_TOOL_FACTORY(AFPSiDBasicKalmanTool)
DECLARE_TOOL_FACTORY(AFP_SIDLocRecoTool)

DECLARE_FACTORY_ENTRIES(AFP_LocReco) {
	DECLARE_ALGORITHM  (AFP_TDLocReco)
	DECLARE_ALGORITHM  (AFP_SIDLocReco)
}
