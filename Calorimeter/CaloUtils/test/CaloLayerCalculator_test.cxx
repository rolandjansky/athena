/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloLayerCalculator_test.cxx,v 1.3 2008-08-22 04:05:04 ssnyder Exp $
/**
 * @file  CaloLayerCalculator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Component test for CaloLayerCalculator.
 */
#ifndef XAOD_STANDALONE
#undef NDEBUG

#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
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
#include "StoreGate/StoreGate.h"
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


static CaloPhiRange range;
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
static const float clust1_phi0 = static_cast<float> (range.fix (dphi*128 + dphi/2));

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
static const float clust2_phi0 = static_cast<float> (range.fix (dphi*32 + dphi/2));

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


CaloDetDescriptor* make_dd ()
{
  LArEM_ID*   em_id   = new LArEM_ID;
  LArHEC_ID*  hec_id  = new LArHEC_ID;
  LArFCAL_ID* fcal_id = new LArFCAL_ID;
  LArMiniFCAL_ID* minifcal_id = new LArMiniFCAL_ID;
  TileID*     tile_id = new TileID;

  IdDictParser parser;
  parser.register_external_entity ("LArCalorimeter",
                                   "IdDictLArCalorimeter.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  em_id->set_do_neighbours (false);
  em_id->initialize_from_dictionary (idd);
  hec_id->initialize_from_dictionary (idd);
  fcal_id->set_do_neighbours (false);
  fcal_id->initialize_from_dictionary (idd);
  minifcal_id->set_do_neighbours (false);
  minifcal_id->initialize_from_dictionary (idd);
  tile_id->set_do_neighbours (false);
  tile_id->initialize_from_dictionary (idd);

  CaloCell_ID* calo_helper = new CaloCell_ID (em_id,
                                              hec_id,
                                              fcal_id,
                                              minifcal_id,
                                              tile_id);
  calo_helper->initialize_from_dictionary (idd);
  Identifier reg_id = calo_helper->region_id (CaloCell_ID::LAREM,
                                              1, // + barrel
                                              2, // sampling 2
                                              0); // sampling 2
  CaloDetDescriptor* descr = new MyDetDescriptor (reg_id,
                                                  0,
                                                  calo_helper);

  CaloDetDescrManager* ddman =
    const_cast<CaloDetDescrManager*> (CaloDetDescrManager::instance());
  if (!ddman) {
    ddman = new CaloDetDescrManager;
    ISvcLocator* svcLoc = Gaudi::svcLocator();
    StoreGateSvc* detStore = 0;
    StatusCode status = svcLoc->service("DetectorStore",detStore);
    if (!status.isSuccess()) abort();
    status = detStore->record (ddman, "CaloMgr");
    if (!status.isSuccess()) abort();
  }
  ddman->set_helper (calo_helper);
  ddman->initialize();
  ddman->add (descr);
  return descr;
}


CaloCell* make_cell (CaloDetDescriptor* descr,
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
                 CaloDetDescriptor* descr,
                 Testcell* cells,
                 double eta0,
                 double phi0)
{
  for (; cells->energy != 0; ++cells) {
    double phi = static_cast<float> (cells->iphi) * dphi + phi0;
    double eta = static_cast<float> (cells->ieta) * deta + eta0;
    cont->push_back (make_cell (descr,
                                cells->energy,
                                static_cast<float> (eta),
                                static_cast<float> (range.fix (phi))));
  }
}


const CaloCellContainer* fill_cells()
{
  CaloDetDescriptor* descr = make_dd();
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
  calc.fill (cells, clust1_eta0, clust1_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2);
  clust1_check (calc);

  xAOD::CaloCluster* clust = CaloClusterStoreHelper::makeCluster (cells);
  calc.fill (cells->begin(), cells->end(), clust1_eta0, clust1_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2, clust);
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

  calc.fill (cells, clust2_eta0, clust2_phi0,
             5*deta, 5*dphi, CaloSampling::EMB2);
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
  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("StoreGateSvc", sg).isSuccess() );

  const CaloCellContainer* cells = fill_cells();
  assert (sg->record (cells, "cells").isSuccess());

  test1(cells);
  return 0;
}

#else // XAOD_STANDALONE
int main(){return 0;}
#endif
