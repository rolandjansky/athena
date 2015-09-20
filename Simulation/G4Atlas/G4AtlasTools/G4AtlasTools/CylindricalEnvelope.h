/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CylindricalEnvelope_H
#define CylindricalEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Members

// STL library
#include <string>
#include <vector>

class CylindricalEnvelope final : public DetectorGeometryBase 
{
public:
  // Basic constructor and destructor
  CylindricalEnvelope(const std::string& type, const std::string& name, const IInterface *parent);
  ~CylindricalEnvelope() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override;
  
  /** virtual methods being implemented here */
  
  virtual void BuildGeometry();
  
private:
  double m_innerRadius=0.;
  double m_outerRadius=0.;
  double m_dZ=0;
  

};

#endif
