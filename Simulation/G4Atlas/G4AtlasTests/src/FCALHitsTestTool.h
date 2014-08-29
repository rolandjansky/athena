/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_FCALHITSTESTTOOL
#define G4AT_FCALHITSTESTTOOL

#include "LArHitsTestTool.h"

class FCALHitsTestTool : public LArHitsTestTool 
{
 public:
  FCALHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FCALHitsTestTool() {}
  
  StatusCode initialize();
};

#endif // G4AT_FCALHITSTESTTOOL
