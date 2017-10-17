/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/* @file CaloTPCnv/test/CaloClusterContainerCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Regression tests for CaloClusterContainer converters.
 */

#undef NDEBUG

#include "CaloTPCnv/CaloClusterContainerCnv_p5.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p4.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p3.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p2.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p1.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloUtils/CaloClusterSignalState.h"
#include "EventKernel/SignalStateHelper.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <cassert>


#include "AthenaPoolCnvSvc/Compressor.h"
#include "CaloClusterContainerCnvTest_p7.icc"
#include "CaloClusterContainerCnvTest_p6.icc"


using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;


// Dufus-quality RNG, using LCG.  Constants from numerical recipies.
// I don't particularly care about RNG quality here, just about
// getting something that's reproducible.
#include <stdint.h>
uint32_t seed = 1;
uint32_t rngmax = static_cast<uint32_t> (-1);
uint32_t rng()
{
  seed = (1664525*seed + 1013904223);
  return seed;
}

float randf (float rmax, float rmin = 0)
{
  return static_cast<float>(rng()) / rngmax * (rmax-rmin) + rmin;
}
int randi (int rmax, int rmin = 0)
{
  return static_cast<int> (randf(rmax, rmin));
}
struct randi_fn
{
  int operator() (int rmax) { return randi(rmax); }
};


class CaloClusterContainerCnvTestMakeCluster
  : public CaloCluster
{
public:
  static void setStores (CaloCluster& c);
};


void CaloClusterContainerCnvTestMakeCluster::setStores (CaloCluster& c)
{
  c.setLinkStore (new CaloCellLink);
}


typedef CaloClusterContainerCnvTestMakeCluster MakeCluster;


std::vector<CaloSampling::CaloSample> getSamplingList()
{
  std::vector<CaloSampling::CaloSample> v;
  int n = (int)CaloSampling::Unknown;
  v.reserve (n);
  for (int i=0; i < n; i++)
    v.push_back ((CaloSampling::CaloSample) i);
  return v;
}


CaloCluster* make_cluster()
{
  float eta0 = randf (4, -4);
  float phi0 = randf (3, -3);

  int nvars = 10;
  std::vector<CaloVariableType::VariableType> vtypes (nvars);
  unsigned int mask = 0;
  for (int i = 0; i < nvars; i++) {
    vtypes[i] = (CaloVariableType::VariableType)randi
      (CaloVariableType::getNumberOfVariables());
    mask |= CaloVariableType::getVariableBit (vtypes[i]);
  }
  
  CaloCluster* c = new CaloCluster (eta0, phi0, mask);
  MakeCluster::setStores (*c);
  
  std::vector<CaloSampling::CaloSample> vsamp = getSamplingList();
  for (int i = 0; i < nvars; i++) {
    assert (c->setVariable (vtypes[i],
                            vsamp[randi(vsamp.size())],
                            randf(100000)));
  }

  Hep3Vector dir;
  HepLorentzVector hlv;
  dir.setREtaPhi (randf (100000), randf (4, -4), randf (3, -3));
  hlv.setVectM (Hep3Vector(dir), randf (100000));
  c->set4Mom (hlv);

  {
    SignalStateHelper sig (c);
    sig.setSignalState(P4SignalState::UNCALIBRATED);
    dir.setREtaPhi (randf (100000), randf (4, -4), randf (3, -3));
    hlv.setVectM (Hep3Vector(dir), randf (100000));
    c->set4Mom (hlv);
  }

  c->setBasicEnergy (randf (100000, -10000));
  c->setTime (randf (32, -32));
  c->setClusterSize (randi (14));

  c->setBarrel (randi (2));
  c->setEndcap (randi (2));

  c->setAthenaBarCode (randi (1000000) + 10LL*1000*1000*1000);

  c->insertMoment (CaloClusterMoment::SECOND_R,  randf(100000), false);
  c->insertMoment (CaloClusterMoment::ISOLATION, randf(100000));
  c->insertMoment (CaloClusterMoment::DELTA_PHI, randf(100000));

  c->addBadChannel (CaloClusterBadChannelData
                    (randf (4, -4),
                     randf (3, -3),
                     vsamp[randi(vsamp.size())],
                     CaloBadChannel (randi (256))));
  c->addBadChannel (CaloClusterBadChannelData
                    (randf (4, -4),
                     randf (3, -3),
                     vsamp[randi(vsamp.size())],
                     CaloBadChannel (randi (256))));

  c->setRecoStatus (CaloRecoStatus
                    ((CaloRecoStatus::StatusIndicator)randi (100000)));

  return c;
}


std::unique_ptr<const CaloClusterContainer> make_clusters()
{
  auto ccc = CxxUtils::make_unique<CaloClusterContainer>();
  ccc->setROIAuthor ("theauthor");
  ccc->setTowerSeg (CaloTowerSeg (20, 10, -4, 4, -3, 3));
  for (int i=0; i < 5; i++)
    ccc->push_back (make_cluster());
  return std::unique_ptr<const CaloClusterContainer>(ccc.release());
}


void compare_float (double f1, double f2, double thresh = 2e-5)
{
  double num = f1 - f2;
  double den = fabs(f1) + fabs(f2);
  if (den == 0) den = 1;
  assert (num / den < thresh);
}


void compare (const HepLorentzVector& h1, const HepLorentzVector& h2,
              double thresh = 2e-5)
{
  compare_float (h1.px(), h2.px(), thresh);
  compare_float (h1.py(), h2.py(), thresh);
  compare_float (h1.pz(), h2.pz(), thresh);
  compare_float (h1.e(),  h2.e(), thresh);
}


void compare (const CaloTowerSeg& seg1, const CaloTowerSeg& seg2)
{
  assert (seg1.neta() == seg2.neta());
  assert (seg1.nphi() == seg2.nphi());
  assert (seg1.etamin() == seg2.etamin());
  assert (seg1.etamax() == seg2.etamax());
  assert (seg1.phimin() == seg2.phimin());
  assert (seg1.phimax() == seg2.phimax());
}


void compare (const CaloClusterBadChannelData& b1,
              const CaloClusterBadChannelData& b2)
{
  compare_float (b1.getEta(), b2.getEta(), 3e-2);
  compare_float (b1.getPhi(), b2.getPhi(), 3e-2);
  assert (b1.getLayer() == b2.getLayer());
  assert (b1.getFlag().packedData() == b2.getFlag().packedData());
}


void compare (const CaloCluster::badChannelList& l1,
              const CaloCluster::badChannelList& l2)
{
  assert (!l1.empty());
  assert (l1.size() == l2.size());
  for (size_t i=0; i < l1.size(); i++)
    compare (l1[i], l2[i]);
}


void compare (const CaloRecoStatus& s1, const CaloRecoStatus& s2)
{
  assert (s1.getStatusWord() == s2.getStatusWord());
}


void compare (const CaloCluster& c1, const CaloCluster& c2, int version)
{
  assert (c1.eta0() == c2.eta0());
  assert (c1.phi0() == c2.phi0());

  for (unsigned int vtype = 0;
       vtype < CaloVariableType::getNumberOfVariables();
       ++vtype)
  {
    std::vector<double> v1;
    std::vector<double> v2;
    bool stat1 = c1.getVariable ((CaloVariableType::VariableType)vtype, v1);
    bool stat2 = c2.getVariable ((CaloVariableType::VariableType)vtype, v2);
    if (!stat1 && stat2) {
      for (size_t i=0; i < v2.size(); i++)
        assert (v2[i] == 0);
    }
    else if (stat1) {
      assert (v1.size() == v2.size());
      for (size_t i = 0; i < v1.size(); i++)
        compare_float (v1[i], v2[i]);
    }
  }

  compare (c1.hlv(), c2.hlv());
  if (version >= 4) {
    compare (c1.hlv(P4SignalState::UNCALIBRATED),
             c2.hlv(P4SignalState::UNCALIBRATED),
             2e-2);
  }

  compare_float (c1.getBasicEnergy(), c2.getBasicEnergy());
  compare_float (c1.getTime(),        c2.getTime());
  if (version >= 2)
    assert (c1.getClusterSize() == c2.getClusterSize());
  
  bool barrel = false;
  bool endcap = false;
  std::vector<CaloSampling::CaloSample> vsamp = getSamplingList();
  for (size_t i = 0; i < vsamp.size(); i++) {
    assert (c1.hasSampling (vsamp[i]) == c2.hasSampling (vsamp[i]));
    if (c2.hasSampling(vsamp[i]) && CaloSamplingHelper::isBarrelSampling(vsamp[i]))
      barrel = true;
    if (c2.hasSampling(vsamp[i]) && CaloSamplingHelper::isEndCapSampling(vsamp[i]))
      endcap = true;
  }

  if (version >= 4) {
    assert (barrel == c2.inBarrel());
    assert (endcap == c2.inEndcap());
  }
  else {
    assert (c1.inBarrel() == c2.inBarrel());
    assert (c1.inEndcap() == c2.inEndcap());
  }

  assert (c1.getAthenaBarCode() != IAthenaBarCode::UNDEFINEDBARCODE);
  if (version >= 7)
    assert (c1.getAthenaBarCode() == c2.getAthenaBarCode());

  assert (c1.getMoment (CaloClusterMoment::SECOND_R).getValue() != 0);
  compare_float (c1.getMoment (CaloClusterMoment::SECOND_R).getValue(),
                 c2.getMoment (CaloClusterMoment::SECOND_R).getValue(), 2e-3);
  compare_float (c1.getMoment (CaloClusterMoment::ISOLATION).getValue(),
                 c2.getMoment (CaloClusterMoment::ISOLATION).getValue(), 2e-3);
  compare_float (c1.getMoment (CaloClusterMoment::DELTA_PHI).getValue(),
                 c2.getMoment (CaloClusterMoment::DELTA_PHI).getValue(), 2e-3);

  if (version >= 4)
    compare (*c1.getBadChannel(), *c2.getBadChannel());

  compare (c1.getRecoStatus(), c2.getRecoStatus());
}


void compare (const CaloClusterContainer& clust1,
              const CaloClusterContainer& clust2,
              int version)
{
  assert (clust1.size() == clust2.size());

  // NOT SAVED
  //assert (clust1.getROIAuthor() == clust2.getROIAuthor());

  compare (clust1.getTowerSeg(), clust2.getTowerSeg());

  for (size_t i = 0; i < clust1.size(); i++) {
    compare (*clust1[i], *clust2[i], version);
    CaloCluster& cl = *const_cast<CaloCluster*> (clust2[i]);
    assert (cl.cellLink().index() == i);
    assert (cl.cellLink().dataID() == "celllinkkey");
  }
}


class MungeCellLinks
{
public:
  MungeCellLinks (const CaloClusterContainer& cont);
  ~MungeCellLinks();

private:
  const CaloClusterContainer* m_cont;
  std::vector<const CaloCellLink*> m_saved;
};


MungeCellLinks::MungeCellLinks (const CaloClusterContainer& cont)
  : m_cont (&cont)
{
  for (size_t i = 0; i < cont.size(); i++) {
    CaloCluster& cl = *const_cast<CaloCluster*> (cont[i]);
    m_saved.push_back (*cl.cellLink().cptr());
    ElementLink<CaloCellLinkContainer> el ("celllinkkey", i);
    cl.resetCellLink (el);
  }
}


MungeCellLinks::~MungeCellLinks()
{
  assert (m_cont->size() == m_saved.size());
  for (size_t i = 0; i < m_cont->size(); i++) {
    CaloCluster& cl = *const_cast<CaloCluster*> ((*m_cont)[i]);
    ElementLink<CaloCellLinkContainer> el;
    el.setElement (m_saved[i]);
    cl.resetCellLink (el);
  }
}


template <class PERS, class TPCONV>
void testit (const CaloClusterContainer& clust, int version)
{
  MsgStream log (0, "test");

  TPCONV cnv;
  PERS pers;
  {
    MungeCellLinks munge (clust);
    cnv.transToPers (&clust, &pers, log);
  }

  CaloClusterContainer clust2;
  cnv.persToTrans (&pers, &clust2, log);

  compare (clust, clust2, version);
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  SGTest::initTestStore();
  std::unique_ptr<const CaloClusterContainer> clust = make_clusters();
  testit<CaloClusterContainer_p7, CaloClusterContainerCnvTest_p7> (*clust, 7);
  testit<CaloClusterContainer_p6, CaloClusterContainerCnvTest_p6> (*clust, 6);
  testit<CaloClusterContainer_p5, CaloClusterContainerCnv_p5> (*clust, 5);
  testit<CaloClusterContainer_p4, CaloClusterContainerCnv_p4> (*clust, 4);
  testit<CaloClusterContainer_p3, CaloClusterContainerCnv_p3> (*clust, 3);
  testit<CaloClusterContainer_p2, CaloClusterContainerCnv_p2> (*clust, 2);
  testit<CaloClusterContainer_p1, CaloClusterContainerCnv_p1> (*clust, 1);
  return 0;
}
