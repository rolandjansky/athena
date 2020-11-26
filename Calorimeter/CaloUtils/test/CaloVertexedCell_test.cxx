/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloUtils/CaloVertexedCell_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Component test for CaloVertexedCell.
 */
#ifndef XAOD_STANDALONE
#undef NDEBUG
#include "CaloUtils/CaloVertexedCell.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloTester.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <iostream>
#include <cassert>


using CLHEP::GeV;
using CLHEP::HepLorentzVector;


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


CaloDetDescrElement* add_dde (CaloDetDescrManager* ddman,
                              CaloDetDescriptor* descr,
                              double eta,
                              double phi)
{
  double deta = 0.1;
  double dphi = 0.1;

  int ieta = descr->eta_channel (eta);
  int iphi = descr->phi_channel (phi);
  Identifier regId = descr->identify();
  const CaloCell_Base_ID* idhelper = ddman->getCaloCell_ID();
  Identifier cellID = idhelper->cell_id(regId, ieta, iphi);  
  int subcalo = 0;
  IdentifierHash subcalo_hash = idhelper->subcalo_cell_hash (cellID, subcalo);

  DummyDetDescrElement* dde = new DummyDetDescrElement (subcalo_hash,
                                                        0, 0, descr);
  dde->set_cylindric (eta, phi, (double)1000);
  dde->set_cylindric_raw (eta, phi, 1000);
  dde->set_cylindric_size (deta, dphi, 1);

  ddman->add (dde);
  return dde;
}


CaloDetDescrElement* make_dd (CaloTester& tester)
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

  return add_dde (&tester.mgr(), descr, 0.11, 1);
}


bool is_equal (double x1, double x2)
{
  if (std::abs (x1-x2) / (std::abs(x1+x2)+1e-5) < 1e-5)
    return true;
  if (std::abs (x1-x2) < 2e-3)
    return true;
  std::cout << "Match failure: " << x1 << " " << x2 << " "
            << std::abs (x1-x2) / (std::abs(x1+x2)+1e-5) << "\n";
  return false;
}


void checkit (const CaloVertexedCell& cell, const HepLorentzVector& v)
{
  assert (cell.cell().caloDDE()->identify() == cell.cell().ID());
  assert (is_equal (cell.eta(), v.eta()));
  assert (is_equal (cell.phi(), v.phi()));
  assert (is_equal (cell.e(), v.e()));
  assert (is_equal (cell.energy(), v.e()));
  assert (is_equal (cell.m(), v.m()));
  assert (is_equal (cell.sinTh(), sin(v.theta())));
  assert (is_equal (cell.cosTh(), cos(v.theta())));
  assert (is_equal (cell.sinPhi(), sin(v.phi())));
  assert (is_equal (cell.cosPhi(), cos(v.phi())));
}


void test1 (CaloDetDescrElement* dde)
{
  std::cout << "test1\n";

  CaloCell cell (dde, 100*GeV, 0, 0, 0,
                 CaloGain::LARMEDIUMGAIN);

  HepLorentzVector v1;
  v1.setREtaPhi (100*GeV, 0.11, 1);
  v1.setE (100*GeV);
  checkit (CaloVertexedCell (cell), v1);

  Amg::Vector3D vx (1, 1, 50);
  HepLorentzVector v2;
  v2.setREtaPhi (100*GeV, 0.0604524, 1.0003012);
  v2.setE (100*GeV);

  checkit (CaloVertexedCell (cell, vx), v2);

  CaloVertexedCell::compE comp;
  CaloCell cell2 (dde, 50*GeV, 0, 0, 0,
                  CaloGain::LARMEDIUMGAIN);
  assert (comp (CaloVertexedCell (cell),
                CaloVertexedCell (cell2)));
  assert (!comp (CaloVertexedCell (cell2),
                 CaloVertexedCell (cell)));
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloLayerCalculator_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  CaloTester tester;
  assert(tester.record_mgr());
  CaloDetDescrElement* dde = make_dd (tester);
  test1 (dde);
  return 0;
}

#else // XAOD_STANDALONE
int main(){return 0;}
#endif
