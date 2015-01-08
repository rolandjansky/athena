/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTCALODDE_H
#define TESTLARDETDESCR_TESTCALODDE_H

/********************************************************************

NAME:     TestCaloDDE.h 
PACKAGE:  offline/LArCalorimeter/LArExample/TestLArDetDescr

AUTHORS:  Claire Adam-Bourdarios
CREATED:  Feb 2003

PURPOSE:  provide example to retreive CaloDetDescr manager and
          elements from StoreGate and test them.

********************************************************************/
// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"

class CaloIdManager;
class LArIdManager;
class CaloDetDescrManager;
#include "CaloIdentifier/CaloCell_ID.h"
class ICaloRecoMaterialTool;
class ICaloRecoSimpleGeomTool;
class CaloPhiRange;

class TestCaloDDE : public AthAlgorithm
{
 public:

  // constructor 
  TestCaloDDE(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestCaloDDE();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  void print_subcalo(CaloCell_ID::CaloSample sample);
  void print_eta_line(int phi_num, bool em, bool hec, bool fcal);
  void print_phi_line(int eta_num, bool em, bool hec, bool fcal);
  void print_elt_HW(bool em, bool hec, bool fcal);
  void try_zone();
  void try_each_descr_zone();
  void try_zone(double eta, double deta, double phi, double dphi, int sampling_or_module);
  void read_volumes();
  void where_am_I(double eta, double phi);
  void update();
  void print_edges_via_CaloSample(CaloCell_ID::CaloSample sample, double eta, double phi );
  void print_edges_via_SubCalo(CaloCell_ID::CaloSample sample, double eta, double phi );
  void print_edges_via_CaloSample();
  void print_edges_via_SubCalo();

 protected:
  
  const CaloIdManager*  m_calo_id_man;
  const CaloDetDescrManager*  m_calo_dd_man;
  const LArIdManager* m_lar_id_man;

  // Simplified geometry :
  ICaloRecoMaterialTool* m_lar_mat;
  ICaloRecoSimpleGeomTool* m_lar_simplegeom;
};

#endif

//end of TESTLARDETDESCR_TESTCALODDE 







