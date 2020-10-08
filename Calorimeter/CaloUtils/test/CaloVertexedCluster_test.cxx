/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloUtils/CaloVertexedCluster_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Component test for CaloVertexedCluster.
 */
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "CaloUtils/CaloVertexedCluster.h"
#include "CaloUtils/CaloVertexedTopoCluster.h"
#include "CaloEvent/CaloTester.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <iostream>
#include <cassert>


using CLHEP::GeV;
using FourMom_t = xAOD::CaloCluster::FourMom_t;


bool is_equal (double x1, double x2)
{
  if (std::abs (x1-x2) / (std::abs(x1+x2)+1e-5) < 1e-5)
    return true;
  std::cout << "Match failure: " << x1 << " " << x2 << "\n";
  return false;
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


void add_dde (CaloDetDescrManager* ddman, CaloDetDescriptor* descr,
              double eta, double phi)
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
}


CaloDetDescriptor* make_dd (CaloTester& tester)
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

  add_dde (&tester.mgr(), descr, 0.11, 1);
  add_dde (&tester.mgr(), descr, 0.21, 2);

  return descr;
}


template<class VCLUS>
void checkit (const VCLUS& cl,
              const FourMom_t& v)
{
  assert (cl.clust().type() == cl.type());
  assert (is_equal (cl.pt(), v.Pt()));
  assert (is_equal (cl.eta(), v.Eta()));
  assert (is_equal (cl.phi(), v.Phi()));
  assert (is_equal (cl.m(), v.M()));
  assert (is_equal (cl.e(), v.E()));
  assert (is_equal (cl.rapidity(), v.Rapidity()));

  assert (is_equal (cl.p4().X(), v.X()));
  assert (is_equal (cl.p4().Y(), v.Y()));
  assert (is_equal (cl.p4().Z(), v.Z()));
  assert (is_equal (cl.p4().E(), v.E()));
}


void test1()
{
  std::cout << "test1\n";
  xAOD::CaloCluster* cl = new xAOD::CaloCluster;
  cl->makePrivateStore();

  FourMom_t v1;
  v1.SetPtEtaPhiM (100/cosh(0.11) * GeV, 0.11, 1, 10*GeV); 
  FourMom_t v2;
  v2.SetPtEtaPhiM (50/cosh(0.21) * GeV, 0.21, 2, 5*GeV);

  cl->setE   (v1.E());
  cl->setEta (v1.Eta());
  cl->setPhi (v1.Phi());
  cl->setM   (v1.M());

  cl->setRawE   (v2.E());
  cl->setRawEta (v2.Eta());
  cl->setRawPhi (v2.Phi());
  cl->setRawM   (v2.M());

  cl->setSamplingPattern (1 << CaloCell_ID::EMB2);

  cl->setClusterSize (xAOD::CaloCluster::SW_35ele);
  cl->setEnergy (CaloCell_ID::EMB2, 50*GeV);

  Amg::Vector3D vx (1, 1, 50);

  FourMom_t v5 (53906.492488,84010.082746,6032.941780,100498.757813);
  checkit (xAOD::CaloVertexedCluster (*cl, vx), v5);

  FourMom_t v6 (-20599.634043,44853.657575,7987.775653,50249.378906);
  checkit (xAOD::CaloVertexedCluster (*cl, xAOD::CaloCluster::UNCALIBRATED, vx),
           v6);

  cl->setClusterSize (xAOD::CaloCluster::Topo_420);
  cl->insertMoment (xAOD::CaloCluster::CENTER_MAG, 1000);

  checkit (xAOD::CaloVertexedTopoCluster (*cl), v1);
  checkit (xAOD::CaloVertexedTopoCluster (*cl, xAOD::CaloCluster::UNCALIBRATED), v2);


  FourMom_t v3 (53907.177467,84011.431644,6007.985731,100498.757813);
  checkit (xAOD::CaloVertexedTopoCluster (*cl, vx), v3);

  FourMom_t v4 (-20604.274536,44860.449042,7937.509874,50249.378906);
  checkit (xAOD::CaloVertexedTopoCluster (*cl, xAOD::CaloCluster::UNCALIBRATED, vx),
           v4);

}

 
int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloLayerCalculator_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  CaloTester tester;
  assert (tester.record_mgr());
  make_dd (tester);
  test1();
  return 0;
}

#else // XAOD_STANDALONE
int main(){return 0;}
#endif
