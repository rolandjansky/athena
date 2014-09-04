// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_WRITEDATA_H
#define ATHEXSTOREGATEEXAMPLE_WRITEDATA_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

/////////////////////////////////////////////////////////////////////////////

class WriteData:public AthAlgorithm {
public:
  WriteData (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StatusCode onError();
};


#endif // not ATHEXSTOREGATEEXAMPLE_WRITEDATA_H


