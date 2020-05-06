/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTruthEventTPCnv/test/TrigInDetTrackTruthMapCnv_tlp4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for TrigInDetTrackTruthMapCnv_p1.
 */


#undef NDEBUG
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp4.h"
#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigIDHitStats& p1,
              const TrigIDHitStats& p2)
{
  assert (p1.pixhits() == p2.pixhits());
  assert (p1.scthits() == p2.scthits());
  assert (p1.trthits() == p2.trthits());
}


class TrigInDetTrackTruthCnv_p1_test
{
public:
  static void compare (const TrigInDetTrackTruth& p1,
                       const TrigInDetTrackTruth& p2)
  {
    assert (p1.m_best_match_hits == p2.m_best_match_hits);
    assert (p1.m_best_Si_match_hits == p2.m_best_Si_match_hits);
    assert (p1.m_best_TRT_match_hits == p2.m_best_TRT_match_hits);
    assert (p1.m_true_part_vec == p2.m_true_part_vec);
    assert (p1.m_nr_common_hits.size() == p2.m_nr_common_hits.size());
    for (size_t i = 0; i < p1.m_nr_common_hits.size(); i++)
      ::compare (p1.m_nr_common_hits[i], p2.m_nr_common_hits[i]);
    assert (p1.m_family_tree == p2.m_family_tree);
    
  }

  static void set (TrigInDetTrackTruth& p, std::vector<HepMC::GenParticle*>& genPartVector, int o)
  {
    HepMcParticleLink trkLink1(genPartVector.at(0+o)->barcode(),genPartVector.at(0+o)->parent_event()->event_number());
    HepMcParticleLink trkLink2(genPartVector.at(1+o)->barcode(),genPartVector.at(1+o)->parent_event()->event_number());
    HepMcParticleLink trkLink3(genPartVector.at(2+o)->barcode(),genPartVector.at(2+o)->parent_event()->event_number());

    p.m_true_part_vec.emplace_back (trkLink1, 10);
    p.m_true_part_vec.emplace_back (trkLink2, 11);
    p.m_true_part_vec.emplace_back (trkLink3, 12);

    int nstat = 4;
    p.m_nr_common_hits.resize (nstat);
    for (int i=0; i < nstat; i++) {
      p.m_nr_common_hits[i][TrigIDHitStats::PIX] = 12 + i*10 + o*10;
      p.m_nr_common_hits[i][TrigIDHitStats::SCT] = 13 + i*10 + o*10;
      p.m_nr_common_hits[i][TrigIDHitStats::TRT] = 14 + i*10 + o*10;
    }

    p.m_family_tree.emplace_back (0, 1);
    p.m_family_tree.emplace_back (0, 2);

    p.m_best_match_hits = 0;
    p.m_best_Si_match_hits = 1;
    p.m_best_TRT_match_hits = 2;
  }
};


void compare (const TrigInDetTrackTruth& p1,
              const TrigInDetTrackTruth& p2)
{
  TrigInDetTrackTruthCnv_p1_test::compare (p1, p2);
}


void compare (const TrigInDetTrackTruthMap& p1,
              const TrigInDetTrackTruthMap& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1.truthi(i), *p2.truthi(i));
    assert (p1.trackiLink(i) == p2.trackiLink(i));
  }
}


void testit (const TrigInDetTrackTruthMap& trans1)
{
  MsgStream log (0, "test");
  TrigInDetTrackTruthMapCnv_p1 cnv;
  TrigInDetTrackTruthMapCnv_tlp4 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigInDetTrackTruthMap_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigInDetTrackTruthMap trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

const TrigInDetTrackCollection* createTrigInDetTrackCollection()
{

  SG::WriteHandle<TrigInDetTrackCollection> coll{"coll"};
  coll = std::make_unique<TrigInDetTrackCollection>();
  for (int i=0; i<10; i++) {
    int o = i*10;
    auto param = std::make_unique<TrigInDetTrackFitPar>
      (2.5+o, 3.5+o, 4.5+o, 5.5+o, 6.5+o, TrigInDetTrackFitPar::PERIGEE, 7.5+o);
    auto trk = std::make_unique<TrigInDetTrack> (param.release());
    trk->algorithmId (static_cast<TrigInDetTrack::AlgoId>(i+1));
    coll->push_back (std::move (trk));
  }
  return &*coll;
}

void test1(std::vector<HepMC::GenParticle*>& genPartVector)
{
  std::cout << "test1\n";

  TrigInDetTrackTruthMap trans1;
  const TrigInDetTrackCollection *collp = createTrigInDetTrackCollection();

  TrigInDetTrackTruth t1;
  TrigInDetTrackTruthCnv_p1_test::set (t1, genPartVector, 0);
  trans1.addMatch (collp, 1, t1);

  TrigInDetTrackTruth t2;
  TrigInDetTrackTruthCnv_p1_test::set (t2, genPartVector, 3);
  trans1.addMatch (collp, 2, t2);

  TrigInDetTrackTruth t3;
  TrigInDetTrackTruthCnv_p1_test::set (t3, genPartVector ,6);
  trans1.addMatch (collp, 3, t3);

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  std::vector<HepMC::GenParticle*> genPartVector;
  if (!Athena_test::initMcEventCollection(pSvcLoc,genPartVector)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }
  test1(genPartVector);
  return 0;
}
