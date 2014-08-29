/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_EMBHITSTESTTOOL
#define G4AT_EMBHITSTESTTOOL

#include "LArHitsTestTool.h"

class EMBHitsTestTool : public LArHitsTestTool 
{
 public:
  EMBHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EMBHitsTestTool() {}
  
  StatusCode initialize();
};

#endif // G4AT_EMBHITSTESTTOOL
