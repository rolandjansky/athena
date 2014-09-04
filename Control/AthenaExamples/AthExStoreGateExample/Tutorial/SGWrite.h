// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEXAMPLE_SGWRITE_H
#define ATHEXSTOREGATEXAMPLE_SGWRITE_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

/*

This is the HEADER file for the Algorithm SGWrite.
It illustrates how to write a data object into StoreGate

*/

class SGWrite  :  public AthAlgorithm {
  
public:
  
  // Standard Algorithm Constructor:
  
  SGWrite (const std::string& name, ISvcLocator* pSvcLocator);
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  std::string m_DataObjKey;         // Property: key of DataObj in SG
  
};
#endif



