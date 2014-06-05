/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARDETDESCR_TESTLARTT_H
#define TESTLARDETDESCR_TESTLARTT_H

/********************************************************************

NAME:     TestLArTT.h 
PACKAGE:  offline/LArCalorimeter/LArExample/TestLArDetDescr

AUTHORS:  Claire Adam-Bourdarios
CREATED:  Feb 2003

PURPOSE:  provide example to retreive CaloTTDetDescr manager and
          TTregions from StoreGate and test them.

********************************************************************/
// INCLUDE HEADER FILES:
#include "GaudiKernel/Algorithm.h"

class StoreGateSvc;
class CaloTTDescrManager;

class TestLArTT : public Algorithm
{
 public:

  // constructor 
  TestLArTT(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  virtual ~TestLArTT();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  void print_reg(bool em, bool hec, bool fcal, bool tile);

 protected:
  
  StoreGateSvc* m_storeGate;

  const CaloTTDescrManager*  m_tt_man;

};

#endif

//end of TESTLARDETDESCR_TESTLARTT 







