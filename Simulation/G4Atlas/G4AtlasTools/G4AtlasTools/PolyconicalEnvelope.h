/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PolyconicalEnvelope_H
#define PolyconicalEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Members

// STL library
#include <string>
#include <vector>

class PolyconicalEnvelope final : public DetectorGeometryBase 
{
public:
  // Basic constructor and destructor
  PolyconicalEnvelope(const std::string& type, const std::string& name, const IInterface *parent);
  ~PolyconicalEnvelope() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override;
  
  /** virtual methods being implemented here */
  
  virtual void BuildGeometry();
  
private:
  unsigned int m_surfaceNr=0;
  std::vector<double> m_innerRadii;
  std::vector<double> m_outerRadii;
  std::vector<double> m_ZSurfaces;
  

};

#endif
