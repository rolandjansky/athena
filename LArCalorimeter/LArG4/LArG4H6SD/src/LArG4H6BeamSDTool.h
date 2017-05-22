/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H6BeamSDTool_H__
#define __LArG4H6BeamSDTool_H__

#include "G4AtlasTools/SensitiveDetectorBase.h"

class LArG4H6BeamSDTool : public SensitiveDetectorBase
{
public:
  LArG4H6BeamSDTool(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~LArG4H6BeamSDTool() {}

  G4VSensitiveDetector* makeSD() override final;
};

#endif // __LArG4H6BeamSDTool_h__
