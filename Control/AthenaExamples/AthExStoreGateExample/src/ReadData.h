// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_READDATA_H
#define ATHEXSTOREGATEEXAMPLE_READDATA_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class ReadData:public AthAlgorithm {
public:
  ReadData (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  //Properties
  std::string m_DataProducer;     
};

#endif // not ATHEXSTOREGATEEXAMPLE_READDATA_H








