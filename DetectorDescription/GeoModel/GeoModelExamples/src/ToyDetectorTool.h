/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELEXAMPLES_TOYDETECTORTOOL_H
#define GEOMODELEXAMPLES_TOYDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

class ToyDetectorTool : public GeoModelTool {
 public:

  // Standard Constructor
  ToyDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );
  
  // Standard Destructor
  virtual ~ToyDetectorTool() override final;
  
  virtual StatusCode create() override final;
};

#endif // GEOMODELEXAMPLES_TOYDETECTORTOOL_H
