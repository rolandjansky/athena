/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EnvelopeTool_H
#define EnvelopeTool_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Members

// STL library
#include <string>
#include <vector>

class EnvelopeTool final : public DetectorGeometryBase 
{
public:
  // Basic constructor and destructor
  EnvelopeTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~EnvelopeTool() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override;
  
  /** virtual methods being implemented here */
  
  virtual void BuildGeometry();
  
private:
  ServiceHandle<IEnvelopeDefSvc> m_envelopeDefSvc;
};

#endif
