// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBUTILS_L1CALODUMPRAMPDATA_H
#define TRIGT1CALOCALIBUTILS_L1CALODUMPRAMPDATA_H 1

#include "AthenaBaseComps/AthHistogramAlgorithm.h"

class L1CaloDumpRampData: public ::AthHistogramAlgorithm
{ 
 public: 
  L1CaloDumpRampData(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~L1CaloDumpRampData(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private: 

}; 

#endif //> !TRIGT1CALOCALIBUTILS_L1CALODUMPRAMPDATA_H
