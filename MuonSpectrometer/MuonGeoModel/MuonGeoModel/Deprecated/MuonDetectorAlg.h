/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELEXAMPLES_MUONDETECTORALG_H
#define GEOMODELEXAMPLES_MUONDETECTORALG_H
#include "GaudiKernel/Algorithm.h"


/////////////////////////////////////////////////////////////////////////////

class MuonDetectorAlg:public Algorithm {
public:
  MuonDetectorAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  int m_myInt;
  bool m_myBool;
  double m_myDouble;
  std::vector<std::string> m_myStringVec;

};

#endif // GEOMODELEXAMPLES_MUONDETECTORALG_H
