/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELEXAMPLES_TOYDETECTORTOOL_H
#define GEOMODELEXAMPLES_TOYDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GeoModelKernel/GeoPVConstLink.h"

class ToyDetectorTool : public GeoModelTool 
{
 public:
  ToyDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );
  virtual ~ToyDetectorTool() override final;
  
  virtual StatusCode create() override final;
 private:
  void printVolume(GeoPVConstLink volime);
};

#endif // GEOMODELEXAMPLES_TOYDETECTORTOOL_H
