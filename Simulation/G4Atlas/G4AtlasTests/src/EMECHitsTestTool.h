/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_EMECHITSTESTTOOL
#define G4AT_EMECHITSTESTTOOL

#include "LArHitsTestTool.h"

class EMECHitsTestTool : public LArHitsTestTool 
{
 public:
  EMECHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EMECHitsTestTool() {}
  
  StatusCode initialize();
};

#endif // G4AT_EMECHITSTESTTOOL
