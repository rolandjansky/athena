/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4TBECBeamChSDTool_H__
#define __LArG4TBECBeamChSDTool_H__

#include "G4AtlasTools/SensitiveDetectorBase.h"

class G4VSensitiveDetector;

class LArG4TBECBeamChSDTool : public SensitiveDetectorBase
{
public:
  LArG4TBECBeamChSDTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~LArG4TBECBeamChSDTool() {}

  G4VSensitiveDetector* makeSD() override final;
};

#endif // __LArG4H8CalibSD_H__
