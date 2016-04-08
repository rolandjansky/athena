/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CAVERNINFRADETECTORTOOL_H
#define CAVERNINFRADETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
class CavernInfraDetectorManager;

class CavernInfraDetectorTool : public GeoModelTool 
{
 public:

  // Standard Constructor
  CavernInfraDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~CavernInfraDetectorTool();

  virtual StatusCode create( StoreGateSvc* detStore );
  virtual StatusCode clear(StoreGateSvc* detStore);
  
 private:
  const CavernInfraDetectorManager* m_manager;
};

#endif 
