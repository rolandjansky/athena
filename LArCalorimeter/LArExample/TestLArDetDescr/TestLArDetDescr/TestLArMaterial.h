/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTLARMATERIAL_H
#define TESTLARDETDESCR_TESTLARMATERIAL_H

/********************************************************************

NAME:     TestLArMaterial.h 
PACKAGE:  offline/LArCalorimeter/LArExample/TestLArDetDescr

AUTHORS:  Claire Adam-Bourdarios
CREATED:  Sept 2004

PURPOSE:  provide example to play with the H8 CTB geometry.

********************************************************************/
// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloSubdetNames.h"

class CaloDetDescrManager;
class ICaloRecoMaterialTool;
class ICaloSurfaceBuilder;

class TestLArMaterial : public AthAlgorithm
{
 public:

  // constructor 
  TestLArMaterial(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestLArMaterial();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  void print_elt(bool em, bool hec, bool fcal, bool tile);

  void print_Layers();

 protected:
  const CaloDetDescrManager*  m_calo_dd_man;
  ICaloSurfaceBuilder*   m_surfbuild;
  CaloSubdetNames* m_lar_names;
  ICaloRecoMaterialTool* m_lar_mat;
};

#endif

//end of TESTLARDETDESCR_TESTLARMATERIAL 







