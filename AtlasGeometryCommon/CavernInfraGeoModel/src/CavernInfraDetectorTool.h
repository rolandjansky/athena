/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CAVERNINFRADETECTORTOOL_H
#define CAVERNINFRADETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
class CavernInfraDetectorManager;

class CavernInfraDetectorTool final : public GeoModelTool 
{
 public:

  // Standard Constructor
  CavernInfraDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~CavernInfraDetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  
 private:
  const CavernInfraDetectorManager* m_manager;
};

#endif 
