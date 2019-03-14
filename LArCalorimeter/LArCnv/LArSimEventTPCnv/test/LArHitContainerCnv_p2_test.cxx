/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArSimEventTPCnv/test/LArHitContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArHitContainerCnv_p2.
 */


#undef NDEBUG
#include "LArSimEventTPCnv/LArHitContainerCnv_p2.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const LArHit& p1,
              const LArHit& p2)
{
  assert (p1.cellID() == p2.cellID());
  assert (Athena_test::isEqual (p1.energy(), p2.energy(), 0.0005));
  assert (Athena_test::isEqual (p1.time(), p2.time(), 0.002));
}


void compare (const LArHitContainer& p1,
              const LArHitContainer& p2)
{
  assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArHitContainer& trans1)
{
  MsgStream log (0, "test");
  LArHitContainerCnv_p2 cnv;
  LArHitContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArHitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 (const CaloCell_ID& idhelper)
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LArHitContainer trans1 ("coll");
  for (int i=0; i < 10; i++) {
    Identifier id = idhelper.cell_id (CaloCell_ID::LAREM,
                                      1,
                                      1,
                                      1,
                                      1,
                                      i+10);
    int o = i*100;
    trans1.push_back (new LArHit (id, 10.5+o, 12.5+o));
  }
    
  testit (trans1);
}


template <class T>
T* make_helper (IdDictMgr& idd, bool do_neighbours)
{
  T* h = new T;
  h->set_do_neighbours (do_neighbours);
  assert (h->initialize_from_dictionary (idd) == 0);
  return h;
}


const CaloCell_ID& make_dd (ISvcLocator* svcLoc)
{
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");

  LArEM_ID* em_id = make_helper<LArEM_ID> (idd, false);
  LArHEC_ID* hec_id = make_helper<LArHEC_ID> (idd, false);
  LArFCAL_ID* fcal_id = make_helper<LArFCAL_ID> (idd, false);
  LArMiniFCAL_ID* minifcal_id = make_helper<LArMiniFCAL_ID> (idd, false);
  TileID* tile_id = make_helper<TileID> (idd, false);

  CaloCell_ID* calo_id = new CaloCell_ID (em_id,
                                          hec_id,
                                          fcal_id,
                                          minifcal_id,
                                          tile_id);
  assert (calo_id->initialize_from_dictionary (idd) == 0);

  StoreGateSvc* detStore = nullptr;
  assert (svcLoc->service( "DetectorStore", detStore ).isSuccess());
  assert (detStore->record (calo_id, "CaloCell_ID").isSuccess());
  return *calo_id;
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  (void)Athena_test::initGaudi ("DetStoreTest_jobOptions.txt", svcLoc);
  const CaloCell_ID& idhelper = make_dd (svcLoc);
  
  test1 (idhelper);
  return 0;
}
