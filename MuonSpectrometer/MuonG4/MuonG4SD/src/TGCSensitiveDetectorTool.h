/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSENSITIVEDETECTORTOOL_H
#define TGCSENSITIVEDETECTORTOOL_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class TGCSensitiveDetectorTool : public SensitiveDetectorBase {

 public:
  /** construction/destruction */
  TGCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~TGCSensitiveDetectorTool() {}
protected:
  G4VSensitiveDetector* makeSD() override final;
};

#endif
