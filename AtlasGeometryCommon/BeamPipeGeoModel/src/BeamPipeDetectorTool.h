/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMPIPEDETECTORTOOL_H
#define BEAMPIPEDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
class BeamPipeDetectorManager;

class BeamPipeDetectorTool : public GeoModelTool 
{
 public:

  // Standard Constructor
  BeamPipeDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~BeamPipeDetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
 private:
  const BeamPipeDetectorManager* m_manager;
};

#endif 
