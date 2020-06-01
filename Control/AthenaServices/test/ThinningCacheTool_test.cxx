/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  AthenaServices/test/ThinningCacheTool_test.cxx
 * @author scott snyder
 * @date Oct 2019
 * @brief Test for ThinningCacheTool.
 */


#undef NDEBUG
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ThinningDecision.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"
#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <iostream>
#include <cassert>


class Test {};
CLASS_DEF (DataVector<Test>, 29384894, 0)


class TestTNThinningSvc
  : public extends<AthService, ITrigNavigationThinningSvc>
{
public:
  using base_class::base_class;


  virtual
  StatusCode doSlimming (const EventContext& /*ctx*/,
                         std::vector<uint32_t>& /*slimmed_and_serialized*/) const override
  {
    return StatusCode::SUCCESS;
  }
};
DECLARE_COMPONENT (TestTNThinningSvc)


void thin (SG::ThinningDecisionBase& dec, unsigned int mask)
{
  for (size_t i = 0; mask != 0; i++, mask>>=1) {
    if ((mask & 1) != 0) {
      dec.thin (i);
    }
  }
}


unsigned int thinning (const SG::ThinningDecisionBase& dec)
{
  unsigned int mask = 0;
  for (size_t i = 0; i < dec.size(); i++) {
    if (dec.thinned(i)) {
      mask |= (1<<i);
    }
  }
  return mask;
}


void test1()
{
  std::cout << "test1\n";

  ToolHandle<IAthenaOutputTool> tool ("Athena::ThinningCacheTool/TestCacheTool");
  assert (tool.retrieve().isSuccess());

  ServiceHandle<StoreGateSvc> sg ("StoreGateSvc", "test");
  assert (sg.retrieve().isSuccess());

  assert (tool->preExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);
  assert (tool->postExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);

  assert( sg->record (std::make_unique<DataVector<Test> >(16), "v1", false).isSuccess() );
  assert( sg->record (std::make_unique<DataVector<Test> >(16), "v2", false).isSuccess() );

  auto d1 = std::make_unique<SG::ThinningDecision> ("v1");
  auto d2 = std::make_unique<SG::ThinningDecision> ("v2");
  auto d3 = std::make_unique<SG::ThinningDecision> ("v2");

  thin (*d1, 0x5555);
  thin (*d2, 0x6753);
  thin (*d3, 0x7234);

  const SG::ThinningDecisionBase* d1p = d1.get();
  const SG::ThinningDecisionBase* d2p = d2.get();
  const SG::ThinningDecisionBase* d3p = d3.get();

  assert( sg->record (std::move (d1), "v1_THINNED_MyStream",   false).isSuccess() );
  assert( sg->record (std::move (d2), "v2_THINNED_MyStream.a", false).isSuccess() );
  assert( sg->record (std::move (d3), "v2_THINNED_MyStream.b", false).isSuccess() );

  assert (tool->preExecute().isSuccess());
  assert (SG::getThinningCache() != nullptr);

  assert (SG::getThinningDecision ("v1") == d1p);
  assert (SG::getThinningDecision ("v2") != nullptr);
  assert (SG::getThinningDecision ("v2") != d2p);
  assert (SG::getThinningDecision ("v2") != d3p);

  assert (thinning (*SG::getThinningDecision ("v1")) == 0x5555);
  assert (thinning (*SG::getThinningDecision ("v2")) == 0x6210);
  assert (SG::getThinningDecision ("v3") == nullptr);

  assert (SG::getThinningDecision ("v1")->index (7) == 3);
  assert (SG::getThinningDecision ("v2")->index (7) == 6);

  assert (SG::getThinningCache()->trigNavigationThinningSvc() == nullptr);

  assert (tool->postExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);
}


void test2()
{
  std::cout << "test2\n";

  ToolHandle<IAthenaOutputTool> tool ("Athena::ThinningCacheTool/TestCacheTool2");
  assert (tool.retrieve().isSuccess());

  ServiceHandle<ITrigNavigationThinningSvc> tsvc ("TestTNThinningSvc", "test");
  assert (tsvc.retrieve().isSuccess());

  assert (tool->preExecute().isSuccess());
  assert (SG::getThinningCache() != nullptr);
  assert (SG::getThinningCache()->trigNavigationThinningSvc() == tsvc.get());
  assert (tool->postExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);
}



int main()
{
  std::cout << "AthenaServices/ThinningCacheTool_test\n";
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("ThinningCacheTool_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }  
  assert(svcloc);

  EventContext ctx;
  Atlas::setExtendedEventContext (ctx, Atlas::ExtendedEventContext());
  Gaudi::Hive::setCurrentContext (ctx);

  test1();
  test2();
  return 0;
}
