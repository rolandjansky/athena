/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloLayerCalculator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Component test for CaloLayerCalculator.
 */

#ifndef XAOD_STANDALONE
#undef NDEBUG

//This is a test so no need to be thread safe
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloTester.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "StoreGate/StoreGateSvc.h"
#include "IdDictParser/IdDictParser.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/Bootstrap.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <iostream>
#include <cmath>

using CLHEP::GeV;

bool is_equal (double x1, double x2)
{
  if (std::abs (x1-x2) / (std::abs(x1+x2)+1e-5) < 1e-5)
    return true;
  std::cout << "Match failure: " << x1 << " " << x2 << "\n";
  return false;
}


static const CaloPhiRange range;
static const float deta = 0.025f;
static const float dphi = static_cast<float> (2*M_PI / 256);

struct Testcell
{
  int ieta;
  int iphi;
  double energy;
};

Testcell clust1[] = {
  {  1, -1, 10*GeV },
  { -1, -1, 10*GeV },
  { -1,  2, 20*GeV },
  {  3,  2, 30*GeV },
  {  0,  0, 0 },
};
static const float clust1_eta0 = static_cast<float> (0.3 + deta/2);
static const float clust1_phi0 = static_cast<float> (CaloPhiRange::fix (dphi*128 + dphi/2));

void clust1_check (CaloLayerCalculator& calc, double wt=1)
{
  assert (is_equal (calc.etam(), clust1_eta0 - wt/(1+wt)*deta));
  assert (is_equal (calc.phim(),
                    range.fix (clust1_phi0 + (2*wt-1)/(1+wt)*dphi)));
  assert (is_equal (calc.etas(), sqrt(1-wt*wt/(1+wt)/(1+wt))*deta));
  assert (is_equal (calc.phis(),
                  sqrt((1+4*wt)/(1+wt)-(2*wt-1)*(2*wt-1)/(1+wt)/(1+wt))*dphi));
  assert (is_equal (calc.em(), 20*GeV*(1+wt)));
  assert (is_equal (calc.emax(), 20*GeV*wt));
  assert (is_equal (calc.etamax(), clust1_eta0-deta));
  assert (is_equal (calc.phimax(), range.fix (clust1_phi0 + 2*dphi)));
}


Testcell clust2[] = {
  {  0, -1,  10*GeV },
  {  0,  1, -20*GeV },
  {  0,  0,   0 },
};
static const float clust2_eta0 = static_cast<float> (0.3 + deta/2);
static const float clust2_phi0 = static_cast<float> (CaloPhiRange::fix (dphi*32 + dphi/2));

void clust2_check (CaloLayerCalculator& calc)
{
  assert (is_equal (calc.etam(), clust2_eta0));
  assert (is_equal (calc.phim(), clust2_phi0));
  assert (is_equal (calc.etas(), 0));
  assert (is_equal (calc.phis(), 0));
  assert (is_equal (calc.em(), -10*GeV));
  assert (is_equal (calc.emax(), 0));
  assert (is_equal (calc.etamax(), clust2_eta0));
  assert (is_equal (calc.phimax(), clust2_phi0));
}

class MyDetDescriptor
  : public CaloDetDescriptor
{
public:
  MyDetDescriptor (const Identifier& id,
                   const AtlasDetectorID* helper,
                   const CaloCell_ID* calo_helper);
};



MyDetDescriptor::MyDetDescriptor (const Identifier& id,
                                  const AtlasDetectorID* helper,
                                  const CaloCell_ID* calo_helper)
  : CaloDetDescriptor (id, helper, calo_helper)
{
  m_lar_reg_min = 0;
  m_lar_reg_max = 1.5;
  m_lar_phi_min = 0;
}


const CaloDetDescriptor* make_dd (CaloTester& tester)
{
  Identifier reg_id = tester.caloID().region_id (CaloCell_ID::LAREM,
                                                 1, // + barrel
                                                 2, // sampling 2
                                                 0); // sampling 2
  auto ddp = std::make_unique<MyDetDescriptor> (reg_id,
                                                nullptr,
                                                &tester.caloID());
  CaloDetDescriptor* descr = ddp.get();

  tester.mgr().add (std::move (ddp));
  return descr;
}


CaloCell* make_cell (const CaloDetDescriptor* descr,
                     float energy, float eta, float phi)
{
  int ieta = static_cast<int> (eta * (1./deta));
  int iphi = static_cast<int> (phi * (1./dphi));
  if (iphi < 0) iphi += 256;
  const CaloCell_Base_ID* helper = descr->get_calo_helper();
  Identifier id = helper->cell_id (descr->identify(), ieta, iphi);
  int subcalo;
  IdentifierHash subcalo_hash = helper->subcalo_cell_hash (id, subcalo);
  DummyDetDescrElement* dde = new DummyDetDescrElement (subcalo_hash,
							0, 0, descr);
  dde->set_cylindric ((double)eta, (double)phi, (double)1000);
  dde->set_cylindric_raw (eta, phi, 1000);
  dde->set_cylindric_size (deta, dphi, 1);
  const_cast<CaloDetDescrManager*> (CaloDetDescrManager::instance())->add(dde);
  return new CaloCell (dde, energy, 0, 0, CaloGain::LARHIGHGAIN);
}


void make_cells (CaloCellContainer* cont,
                 const CaloDetDescriptor* descr,
                 const Testcell* cells,
                 double eta0,
                 double phi0)
{
  for (; cells->energy != 0; ++cells) {
    double phi = static_cast<float> (cells->iphi) * dphi + phi0;
    double eta = static_cast<float> (cells->ieta) * deta + eta0;
    cont->push_back (make_cell (descr,
                                cells->energy,
                                static_cast<float> (eta),
                                static_cast<float> (CaloPhiRange::fix (phi))));
  }
}


const CaloCellContainer* fill_cells (CaloTester& tester)
{
  const CaloDetDescriptor* descr = make_dd (tester);
  CaloCellContainer* cells = new CaloCellContainer;
  make_cells (cells,
              descr,
              clust1,
              clust1_eta0,
              clust1_phi0);
  make_cells (cells,
              descr,
              clust2,
              clust2_eta0,
              clust2_phi0);
  return cells;
}


void test1 (const CaloCellContainer* cells)
{
  CaloLayerCalculator calc;
  assert( calc.fill (cells, clust1_eta0, clust1_phi0,
                     5*deta, 5*dphi, CaloSampling::EMB2) );
  clust1_check (calc);

  std::unique_ptr<xAOD::CaloCluster> clust (CaloClusterStoreHelper::makeCluster (cells));
  calc.fill (cells->begin(), cells->end(), clust1_eta0, clust1_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2, clust.get());
  clust1_check (calc);
  assert (clust->size() == 3);
  calc.fill (clust->begin(), clust->end(), clust1_eta0, clust1_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2);
  clust1_check (calc);

  for (auto icell = clust->begin(); icell != clust->end(); ++icell) {
    if ((*icell)->energy() > 15*GeV)
      clust->reweightCell (icell, 0.75);
  }
  calc.fill (clust->begin(), clust->end(), clust1_eta0, clust1_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2);
  clust1_check (calc, 0.75);

  assert( calc.fill (cells, clust2_eta0, clust2_phi0,
                     5*deta, 5*dphi, CaloSampling::EMB2) );
  clust2_check (calc);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloLayerCalculator_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = nullptr;
  assert ( svcLoc->service("StoreGateSvc", sg).isSuccess() );

  CaloTester tester;
  assert ( tester.record_mgr() );
  const CaloCellContainer* cells = fill_cells (tester);
  assert (sg->record (cells, "cells").isSuccess());

  test1(cells);
  return 0;
}

#else // XAOD_STANDALONE
int main(){return 0;}
#endif
