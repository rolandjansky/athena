// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEXAMPLE_SGREAD_H
#define ATHEXSTOREGATEXAMPLE_SGREAD_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

/*

  Header file for algorithm SGRead
  reads data objects from StoreGate

*/

class SGRead  :  public AthAlgorithm  {  
public:
      
  // Standard Algorithm Constructor:
      
  SGRead (const std::string& name, ISvcLocator* pSvcLocator);
  
  // Algorithm methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  std::string m_DataObjKey;          // Property: key of DataObj in TDS
  
};


#endif








