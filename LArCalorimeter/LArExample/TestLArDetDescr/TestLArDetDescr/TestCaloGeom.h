/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTCALOGEOM_H
#define TESTLARDETDESCR_TESTCALOGEOM_H

/********************************************************************

NAME:     TestCaloGeom.h 
PACKAGE:  offline/LArCalorimeter/LArExample/TestLArDetDescr

AUTHORS:  Claire Adam-Bourdarios
CREATED:  Sept 2004

PURPOSE:  provide example to play with the H8 CTB geometry.

********************************************************************/
// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class CaloDetDescrManager;
class ICaloCoordinateTool;

class TestCaloGeom : public AthAlgorithm
{
 public:

  // constructor 
  TestCaloGeom(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestCaloGeom();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  void print_elt(bool em, bool hec, bool fcal, bool tile);
  void print_beam();

 protected:
  const CaloDetDescrManager*  m_calo_dd_man;
  ToolHandle<ICaloCoordinateTool> m_coord;
};

#endif

//end of TESTLARDETDESCR_TESTCALOGEOM 
