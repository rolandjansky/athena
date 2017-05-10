/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GDMLDetectorTool_H
#define GDMLDetectorTool_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

#include "G4Transform3D.hh"

// Members

// STL library
#include <string>
#include <vector>

/** @class GeoDetectorTool GeoDetectorTool.h "G4AtlasTools/GeoDetectorTool.h"
*
*  Tool for building detectors out of a GDML description. 
*
*  @author Andrea Dell'Acqua
*  @date   2017-02-21
*/

class GDMLDetectorTool final : public DetectorGeometryBase
{
public:
  // Basic constructor and destructor
  GDMLDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~GDMLDetectorTool() {}

  /** Athena method. called at initialization time, being customized here */
  virtual StatusCode initialize() override final;

  /** virtual methods being implemented here */

  void BuildGeometry() override final;
  
//  void PositionInParent() override final;

private:

  std::string m_GDMLFileName;
  std::string m_builderName;
  bool m_blParamOn;
  bool m_blGetTopTransform;
  G4Transform3D m_topTransform;
  std::string m_geoDetectorName;
  bool IsTopTransform();
  void SetInitialTransformation();
};

#endif
