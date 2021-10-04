/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTLARTT_H
#define TESTLARDETDESCR_TESTLARTT_H

/**
 * @file   TestLArTT.h 
 * @author Claire Adam-Bourdarios
 * @date   Feb 2003
 * @brief  provide example to retreive CaloTTDetDescr manager and
 *         TTregions from StoreGate and test them.
 */

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"

class CaloTTDescrManager;

class TestLArTT : public AthAlgorithm
{
 public:

  // constructor 
  TestLArTT(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestLArTT();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;

  void print_reg();

 protected:
  const CaloTTDescrManager*  m_tt_man;
};

#endif

//end of TESTLARDETDESCR_TESTLARTT 







