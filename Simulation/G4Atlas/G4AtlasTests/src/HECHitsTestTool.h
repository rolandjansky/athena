/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_HECHITSTESTTOOL
#define G4AT_HECHITSTESTTOOL

#include "LArHitsTestTool.h"

class HECHitsTestTool : public LArHitsTestTool 
{
 public:
  HECHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~HECHitsTestTool() {}
  
  StatusCode initialize();
};

#endif // G4AT_HECHITSTESTTOOL
