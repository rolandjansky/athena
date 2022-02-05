/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTLARMATERIAL_H
#define TESTLARDETDESCR_TESTLARMATERIAL_H

/**
 * @file   TestLArMaterial.h
 * @author Claire Adam-Bourdarios
 * @date   Sept 2004
 * 
 * @brief  provide example to play with the H8 CTB geometry.
 */

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloSubdetNames.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class ICaloRecoMaterialTool;
class ICaloSurfaceBuilder;

class TestLArMaterial : public AthAlgorithm
{
 public:

  // constructor 
  TestLArMaterial(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestLArMaterial();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;

  void print_elt(bool em, bool hec, bool fcal, bool tile);

  void print_Layers();

 protected:
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  ICaloSurfaceBuilder*   m_surfbuild{nullptr};
  CaloSubdetNames* m_lar_names{nullptr};
  ICaloRecoMaterialTool* m_lar_mat{nullptr};
};

#endif

//end of TESTLARDETDESCR_TESTLARMATERIAL 







