/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ITestHypoTool.h"
#include "../TestHypoAlg.h"
#include "../TestHypoTool.h"
#include "../TestRecoAlg.h"
#include "../TestRoRSeqFilter.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestHypoAlg )
DECLARE_NAMESPACE_TOOL_FACTORY      ( HLTTest, TestHypoTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestRecoAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestRoRSeqFilter )

DECLARE_FACTORY_ENTRIES( TrigUpgradeTest )
{
  DECLARE_NAMESPACE_ALGORITHM ( HLTTest, TestHypoAlg )
  DECLARE_NAMESPACE_TOOL      ( HLTTest, TestHypoTool )
  DECLARE_NAMESPACE_ALGORITHM ( HLTTest, TestRecoAlg )
  DECLARE_NAMESPACE_ALGORITHM ( HLTTest, TestRoRSeqFilter )
}
