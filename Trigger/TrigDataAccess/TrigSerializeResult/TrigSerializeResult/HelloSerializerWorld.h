// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Algorithm.h"
class StoreGateSvc; 

/////////////////////////////////////////////////////////////////////////////

class HelloSerializerWorld: public Algorithm  {

public:
  HelloSerializerWorld (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  int RunTest;

};
