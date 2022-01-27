/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/TgcRdoCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/TgcRdoCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TgcRawData& p1,
              const TgcRawData& p2)
{
  assert (p1.bcTag() == p2.bcTag());
  assert (p1.subDetectorId() == p2.subDetectorId());
  assert (p1.rodId() == p2.rodId());
  assert (p1.sswId() == p2.sswId());
  assert (p1.slbId() == p2.slbId());
  assert (p1.l1Id() == p2.l1Id());
  assert (p1.bcId() == p2.bcId());
  assert (p1.slbType() == p2.slbType());
  assert (p1.bitpos() == p2.bitpos());
  assert (p1.tracklet() == p2.tracklet());
  assert (p1.isAdjacent() == p2.isAdjacent());
  assert (p1.type() == p2.type());
  assert (p1.isForward() == p2.isForward());
  assert (p1.index() == p2.index());
  assert (p1.position() == p2.position());
  assert (p1.delta() == p2.delta());
  assert (p1.segment() == p2.segment());
  assert (p1.subMatrix() == p2.subMatrix());
  assert (p1.sector() == p2.sector());
  assert (p1.chip() == p2.chip());
  assert (p1.isHipt() == p2.isHipt());
  assert (p1.hitId() == p2.hitId());
  assert (p1.hsub() == p2.hsub());
  assert (p1.isStrip() == p2.isStrip());
  assert (p1.inner() == p2.inner());
  assert (p1.cand3plus() == p2.cand3plus());
  assert (p1.isMuplus() == p2.isMuplus());
  assert (p1.threshold() == p2.threshold());
  assert (p1.isOverlap() == p2.isOverlap());
  assert (p1.isVeto() == p2.isVeto());
  assert (p1.roi() == p2.roi());
}


void compare (const TgcRdo::Errors& p1,
              const TgcRdo::Errors& p2)
{
  assert (p1.badBcID == p2.badBcID);
  assert (p1.badL1Id == p2.badL1Id);
  assert (p1.timedout == p2.timedout);
  assert (p1.badData == p2.badData);
  assert (p1.overflow == p2.overflow);
}


void compare (const TgcRdo::RodStatus& p1,
              const TgcRdo::RodStatus& p2)
{
  assert (p1.EC_RXsend == p2.EC_RXsend);
  assert (p1.EC_FELdown == p2.EC_FELdown);
  assert (p1.EC_frame == p2.EC_frame);
  assert (p1.EC_Glnk == p2.EC_Glnk);
  assert (p1.EC_xor == p2.EC_xor);
  assert (p1.EC_ovfl == p2.EC_ovfl);
  assert (p1.EC_timeout == p2.EC_timeout);
  assert (p1.EC_xormezz == p2.EC_xormezz);
  assert (p1.EC_wc0 == p2.EC_wc0);
  assert (p1.EC_L1ID == p2.EC_L1ID);
  assert (p1.EC_nohdr == p2.EC_nohdr);
  assert (p1.EC_rectype == p2.EC_rectype);
  assert (p1.EC_null == p2.EC_null);
  assert (p1.EC_order == p2.EC_order);
  assert (p1.EC_LDB == p2.EC_LDB);
  assert (p1.EC_RXovfl == p2.EC_RXovfl);
  assert (p1.EC_SSWerr == p2.EC_SSWerr);
  assert (p1.EC_sbid == p2.EC_sbid);
  assert (p1.EC_unxsbid == p2.EC_unxsbid);
  assert (p1.EC_dupsb == p2.EC_dupsb);
  assert (p1.EC_ec4 == p2.EC_ec4);
  assert (p1.EC_bc == p2.EC_bc);
  assert (p1.EC_celladr == p2.EC_celladr);
  assert (p1.EC_hitovfl == p2.EC_hitovfl);
  assert (p1.EC_trgbit == p2.EC_trgbit);
  assert (p1.EC_badEoE == p2.EC_badEoE);
  assert (p1.EC_endWCnot0 == p2.EC_endWCnot0);
  assert (p1.EC_noEoE == p2.EC_noEoE);
}


void compare (const TgcRdo::LocalStatus& p1,
              const TgcRdo::LocalStatus& p2)
{
  assert (p1.mergedHitBCs == p2.mergedHitBCs);
  assert (p1.mergedTrackletBCs == p2.mergedTrackletBCs);
  assert (p1.sortedHits == p2.sortedHits);
  assert (p1.sortedTracklets == p2.sortedTracklets);
  assert (p1.hasRoI == p2.hasRoI);
  assert (p1.fakeSsw == p2.fakeSsw);
  assert (p1.fill1 == p2.fill1);
}


void compare (const TgcRdo& p1,
              const TgcRdo& p2)
{
  assert (p1.version() == p2.version());
  assert (p1.identify() == p2.identify());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.subDetectorId() == p2.subDetectorId());
  assert (p1.rodId() == p2.rodId());
  assert (p1.triggerType() == p2.triggerType());
  assert (p1.bcId() == p2.bcId());
  assert (p1.l1Id() == p2.l1Id());
  compare (p1.errors(), p2.errors());
  compare (p1.rodStatus(), p2.rodStatus());
  compare (p1.localStatus(), p2.localStatus());
  assert (p1.orbit() == p2.orbit());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const TgcRdo& trans1)
{
  MsgStream log (nullptr, "test");
  TgcRdoCnv_p3 cnv;
  TgcRdo_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TgcRdo trans2 (0, 234);
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TgcRdo dum;
  // Creates services --- call it once now to avoid tripping the leak checker.
  dum.setOnlineId (3, 4);

  Athena_test::Leakcheck check;

  TgcRdo trans1 (123, 234);
  trans1.setVersion (345);
  trans1.setOnlineId (456, 567);
  trans1.setTriggerType (678);
  trans1.setBcId (789);
  trans1.setL1Id (987);
  trans1.setOrbit (876);
  trans1.setErrors (0x0c);
  trans1.setRodStatus (0x35);
  trans1.setLocalStatus (0x0a);

  // hit
  trans1.push_back (new TgcRawData (1, 2, 3, 4, 5, 6, 7, 8));

  // trigger coincidence
  trans1.push_back (new TgcRawData (11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21));

  // P2 hits
  trans1.push_back (new TgcRawData (31, 32, 33, 34, 35, 36, 37,
                                    TgcRawData::SLB_TYPE_DOUBLET_STRIP,
                                    true, 38, 39));

  // tracklets
  trans1.push_back (new TgcRawData (41, 42, 43, 44, 45, 46, 47,
                                    TgcRawData::SLB_TYPE_TRIPLET_WIRE,
                                    48, 49, 50, 51));

  // high-pt
  trans1.push_back (new TgcRawData (61, 62, 63, 64, 65, true, false, 66, 67, 68, true,
                                    69, 70, 71, 72));

  // sector logic
  trans1.push_back (new TgcRawData (71, 72, 73, 74, 75, false, true, 76, 77, false,
                                    78, true, false, 79));

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1();
  return 0;
}
