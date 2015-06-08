/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSensitiveDetectorCosmicsTool_H
#define TGCSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class TGCSensitiveDetectorCosmicsTool: public SensitiveDetectorBase {

 public:
  /** construction/destruction */
  TGCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~TGCSensitiveDetectorCosmicsTool() {}
protected:
  G4VSensitiveDetector* makeSD() override final;
};

#endif
