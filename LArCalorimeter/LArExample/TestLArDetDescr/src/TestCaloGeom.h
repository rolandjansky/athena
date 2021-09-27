/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTCALOGEOM_H
#define TESTLARDETDESCR_TESTCALOGEOM_H

/**
 *
 * @file   TestCaloGeom.h 
 * @author Claire Adam-Bourdarios
 * @date   Sept 2004
 *
 * @brief  provide example to play with the H8 CTB geometry.
 *
 */

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class ICaloCoordinateTool;

class TestCaloGeom : public AthAlgorithm
{
 public:

  // constructor 
  TestCaloGeom(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestCaloGeom();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;

  void print_elt(bool em, bool hec, bool fcal, bool tile);
  void print_beam();

 protected:
  ToolHandle<ICaloCoordinateTool> m_coord;
};

#endif

//end of TESTLARDETDESCR_TESTCALOGEOM 
