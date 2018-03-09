/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H6WarmTCSDTool_H__
#define __LArG4H6WarmTCSDTool_H__

#include "G4AtlasTools/SensitiveDetectorBase.h"

class LArG4H6WarmTCSDTool : public SensitiveDetectorBase
{
public:
  LArG4H6WarmTCSDTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~LArG4H6WarmTCSDTool() {}

  G4VSensitiveDetector* makeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;
};

#endif // __LArG4H6WarmTCSDTool_h__
