/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkEventTPCnv/test/CompetingRIOsOnTrackCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


class TestCompetingRIOsOnTrack
  : public Trk::CompetingRIOsOnTrack
{
public:
  TestCompetingRIOsOnTrack() {}
  TestCompetingRIOsOnTrack (const Trk::LocalParameters& pars,
                            const Amg::MatrixX& cov,
                            const std::vector<double>& probs);
  
  virtual unsigned int numberOfContainedROTs() const override
  { return m_assignProb.load()->size(); }

  virtual const Trk::Surface& associatedSurface() const override
  { std::abort(); }
  virtual const Amg::Vector3D& globalPosition() const override
  { std::abort(); }

  virtual Trk::CompetingRIOsOnTrack* clone() const override
  { std::abort(); }
  virtual const Trk::RIO_OnTrack& rioOnTrack(unsigned int) const override
  { std::abort(); }
  virtual bool ROTsHaveCommonSurface(const bool /*withNonVanishingAssignProb=true*/) const override
  { std::abort(); }
};


TestCompetingRIOsOnTrack::TestCompetingRIOsOnTrack (const Trk::LocalParameters& pars,
                                                    const Amg::MatrixX& cov,
                                                    const std::vector<double>& probs)
  : Trk::CompetingRIOsOnTrack (new std::vector<double> (probs))
{
  m_localParams = pars;
  m_localCovariance = cov;
}


void compare (const Trk::CompetingRIOsOnTrack& p1,
              const Trk::CompetingRIOsOnTrack& p2)
{
  assert (p1.localParameters().size() == p2.localParameters().size());
  for (int i = 0; i < p1.localParameters().size(); i++) {
    Trk::ParamDefs ii = static_cast<Trk::ParamDefs>(i);
    assert (Athena_test::isEqual (p1.localParameters()[ii], p2.localParameters()[ii]));
  }
  assert (p1.localCovariance() == p2.localCovariance());

  assert (p1.indexOfMaxAssignProb() == p2.indexOfMaxAssignProb());
  for (int i=0; i < 5; i++)
    assert (p1.assignmentProbability(i) == p2.assignmentProbability(i));
}


void testit (const Trk::CompetingRIOsOnTrack& trans1)
{
  MsgStream log (0, "test");
  CompetingRIOsOnTrackCnv_p1 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::CompetingRIOsOnTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TestCompetingRIOsOnTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::MatrixX cov(5,5);
  for (int i=0; i < 5; i++)
    for (int j=0; j < 5; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  Trk::LocalParameters parms1 (1.5, 2.5, 3.5, 4.5, 5.5);
  std::vector<double> probs { 5, 3, 10.5, 3, 1 };

  TestCompetingRIOsOnTrack trans (parms1, cov, probs);
  testit (trans);
}


int main()
{
  test1();
  return 0;
}
