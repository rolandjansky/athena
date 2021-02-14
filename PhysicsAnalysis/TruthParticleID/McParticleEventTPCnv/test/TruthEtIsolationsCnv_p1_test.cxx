/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file McParticleEventTPCnv/test/TruthEtIsolationsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TruthEtIsolationsCnv_p1.
 */


#undef NDEBUG
#include "McParticleEventTPCnv/TruthEtIsolationsCnv_p1.h"
#include "McParticleEvent/TruthEtIsolations.h"
#include "GeneratorObjects/McEventCollection.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TruthEtIsolations& p1,
              const TruthEtIsolations& p2)
{
  assert (p1.genEventName() == p2.genEventName());
  assert (p1.genEventIdx() == p2.genEventIdx());

  assert (p1.size() == p2.size());
  for (const auto& p : p1) {
    const TruthEtIsolations::EtIsol_t* isol2 = p2.etIsolations (p.first);
    assert (isol2 != nullptr);
    assert (p.second == *isol2);
  }
}


void testit (const TruthEtIsolations& trans1)
{
  MsgStream log (0, "test");
  TruthEtIsolationsCnv_p1 cnv;
  TruthEtIsolations_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TruthEtIsolations trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();

  auto evcoll = std::make_unique<McEventCollection>();
  evcoll->push_back (std::make_unique<HepMC::GenEvent>());
  evcoll->push_back (std::make_unique<HepMC::GenEvent>());

  auto ge = std::make_unique<HepMC::GenEvent>();
#ifdef HEPMC3
  auto gv=HepMC::newGenVertexPtr() ;
  std::vector<HepMC::GenParticlePtr> parts;
  for (size_t i = 0; i < 5; i++) {
    auto gp = HepMC::newGenParticlePtr(HepMC::FourVector(i*10 + 1.5,i*10 + 2.5,i*10 + 3.5,i*10 + 4.5),i+20);
    HepMC::suggest_barcode(gp,1000+i);
    parts.push_back (gp);
    gv->add_particle_out(gp);
  }
  ge->add_vertex (gv);
#else
  auto gv = std::make_unique<HepMC::GenVertex>();
  std::vector<HepMC::GenParticle*> parts;
  for (size_t i = 0; i < 5; i++) {
    auto gp = std::make_unique<HepMC::GenParticle>
      (HepMC::FourVector (i*10 + 1.5,
                          i*10 + 2.5,
                          i*10 + 3.5,
                          i*10 + 4.5),
       i+20);
    gp->suggest_barcode (1000+i);
    parts.push_back (gp.get());
    gv->add_particle_out (gp.release());
  }
  ge->add_vertex (gv.release());
#endif
  evcoll->push_back (std::move(ge));
  store.record (std::move(evcoll), "mcevt");
  ElementLink<McEventCollection> evlink ("mcevt", 2);

  TruthEtIsolations trans1 (evlink);
  for (size_t i = 0; i < 5; i++) {
    int offs = 10*i;
    trans1.setEtIsol (parts[i],
                      std::vector<double> { 101.5 + offs,
                                            102.5 + offs,
                                            103.5 + offs,
                                            104.5 + offs,
                                            105.5 + offs,
                                            106.5 + offs,
                                            107.5 + offs,
                                            108.5 + offs } );
  }

  Athena_test::Leakcheck check;
  testit (trans1);
}


int main()
{
  std::cout << "McParticleEventTPCnv/TruthEtIsolationsCnv_p1_test\n";
  std::unique_ptr<SGTest::TestStore> store = SGTest::getTestStore();
  test1 (*store);
  return 0;
}
