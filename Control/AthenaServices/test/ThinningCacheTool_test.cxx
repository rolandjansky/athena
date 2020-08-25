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
#include "../src/SelectionVetoes.h"
#include "../src/CompressionInfo.h"
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

  assert (tool->preStream().isSuccess());
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

  assert (tool->preStream().isSuccess());
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

  auto vetoes = std::make_unique<SG::SelectionVetoes>();
  SG::auxid_set_t s1;
  s1.set (10);
  s1.set (11);
  SG::auxid_set_t s3;
  s3.set (11);
  s3.set (12);
  (*vetoes).emplace ("v1", s1);
  (*vetoes).emplace ("v3", s3);
  assert (sg->record (std::move (vetoes), "SelectionVetoes_MyStream").isSuccess());

  assert (tool->preStream().isSuccess());
  assert (SG::getThinningCache() != nullptr);

  assert (SG::getThinningInfo ("v1") != nullptr);
  assert (SG::getThinningInfo ("v2") != nullptr);
  assert (SG::getThinningInfo ("v3") != nullptr);
  assert (SG::getThinningInfo ("xx") == nullptr);

  assert (SG::getThinningInfo ("v1")->m_vetoed.size() == 2);
  assert (SG::getThinningInfo ("v2")->m_vetoed.size() == 0);
  assert (SG::getThinningInfo ("v3")->m_vetoed.size() == 2);

  assert (SG::getThinningInfo ("v1")->vetoed (10));
  assert (SG::getThinningInfo ("v1")->vetoed (11));
  assert (!SG::getThinningInfo ("v1")->vetoed (12));
  assert (!SG::getThinningInfo ("v3")->vetoed (10));
  assert (SG::getThinningInfo ("v3")->vetoed (11));
  assert (SG::getThinningInfo ("v3")->vetoed (12));

  // Set dummy lossy float compression information
  auto compInfo = std::make_unique<SG::CompressionInfo>();

  SG::ThinningInfo::compression_map_t compression1;
  SG::auxid_set_t s4; s4.set(10); s4.set(11);
  compression1[7] = s4;
  (*compInfo).emplace ("f1", compression1);

  SG::ThinningInfo::compression_map_t compression2;
  SG::auxid_set_t s5; s5.set(10); s5.set(11); s5.set(33);
  compression2[15] = s5;
  compression2[16] = s5;
  (*compInfo).emplace ("f2", compression2);

  // Record the compression information in the SG
  assert (sg->record (std::move (compInfo), "CompressionInfo_MyStream").isSuccess());

  assert (tool->preStream().isSuccess());
  assert (SG::getThinningCache() != nullptr);

  // Test that we can get the thinning information for the relevant keys
  assert (SG::getThinningInfo ("f1") != nullptr);
  assert (SG::getThinningInfo ("f2") != nullptr);

  // Test that we have the correct size of the compression maps for the relevant keys
  assert (SG::getThinningInfo ("f1")->m_compression.size() == 1); // 1 level
  assert (SG::getThinningInfo ("f2")->m_compression.size() == 2); // 2 levels
  // Test that we have the correct size of the AuxIDs for the relevant compression levels
  assert (SG::getThinningInfo ("f1")->m_compression.at(7).size() == 2); // 2 AuxIDs
  assert (SG::getThinningInfo ("f2")->m_compression.at(15).size() == 3); // 3 AuxIDs
  assert (SG::getThinningInfo ("f2")->m_compression.at(16).size() == 3);
  // Test that we can get the correct compression information overall
  assert (SG::getThinningInfo ("f1")->compression(10) == 7);
  assert (SG::getThinningInfo ("f1")->compression(11) == 7);
  assert (SG::getThinningInfo ("f1")->compression(12) == 0);
  assert (SG::getThinningInfo ("f2")->compression(10) == 15);
  assert (SG::getThinningInfo ("f2")->compression(11) == 15);
  assert (SG::getThinningInfo ("f2")->compression(10) != 16);
  assert (SG::getThinningInfo ("f2")->compression(33) != 16);

  assert (tool->postExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);

  assert (sg->clearStore().isSuccess());
}


void test2()
{
  std::cout << "test2\n";

  ToolHandle<IAthenaOutputTool> tool ("Athena::ThinningCacheTool/TestCacheTool2");
  assert (tool.retrieve().isSuccess());

  ServiceHandle<ITrigNavigationThinningSvc> tsvc ("TestTNThinningSvc", "test");
  assert (tsvc.retrieve().isSuccess());

  assert (tool->preStream().isSuccess());
  assert (SG::getThinningCache() != nullptr);
  assert (SG::getThinningCache()->trigNavigationThinningSvc() == tsvc.get());
  assert (tool->postExecute().isSuccess());
  assert (SG::getThinningCache() == nullptr);
}



int main()
{
  std::cout << "AthenaServices/ThinningCacheTool_test\n";
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("AthenaServices/ThinningCacheTool_test.txt", svcloc)) {
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
