/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTCALODDE_H
#define TESTLARDETDESCR_TESTCALODDE_H

/**
 * @file   TestCaloDDE.h 
 *
 * @author Claire Adam-Bourdarios
 *
 * @date   Feb 2003
 *
 * @brief  Example of retreiving CaloDetDescr manager either from
 *           DetStore or from CondStore
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloIdManager;
class ICaloRecoMaterialTool;
class ICaloRecoSimpleGeomTool;
class CaloPhiRange;

class TestCaloDDE : public AthAlgorithm
{
 public:
  TestCaloDDE(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TestCaloDDE();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;

 private:
  void print_subcalo(CaloCell_ID::CaloSample sample, const CaloDetDescrManager* caloDDMan);
  void print_eta_line(int phi_num, bool em, bool hec, bool fcal, const CaloDetDescrManager* caloDDMan);
  void print_phi_line(int eta_num, bool em, bool hec, bool fcal, const CaloDetDescrManager* caloDDMan);
  void print_elt_HW(bool em, bool hec, bool fcal, const CaloDetDescrManager* caloDDMan);
  void try_zone(const CaloDetDescrManager* caloDDMan);
  void try_each_descr_zone(const CaloDetDescrManager* caloDDMan);
  void try_zone(double eta, double deta, double phi, double dphi, int sampling_or_module, const CaloDetDescrManager* caloDDMan);
  void read_volumes(const CaloDetDescrManager* caloDDMan);
  void where_am_I(double eta, double phi, const CaloDetDescrManager* caloDDMan);
  void print_edges_via_CaloSample(CaloCell_ID::CaloSample sample, double eta, double phi, const CaloDetDescrManager* caloDDMan);
  void print_edges_via_SubCalo(CaloCell_ID::CaloSample sample, double eta, double phi, const CaloDetDescrManager* caloDDMan);
  void print_edges_via_CaloSample(const CaloDetDescrManager* caloDDMan);
  void print_edges_via_SubCalo(const CaloDetDescrManager* caloDDMan);
  
  const CaloIdManager*  m_calo_id_man{nullptr};
  const CaloDetDescrManager*  m_calo_dd_man{nullptr};

  BooleanProperty m_useCondStore { this
      , "UseCondStore"
      , true
      , "Get CaloDetDescrManage from Condition Store. If FALSE, then use Detector Store"};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };
};

#endif
