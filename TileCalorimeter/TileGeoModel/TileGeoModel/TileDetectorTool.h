/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDetectorTool_H
#define TileDetectorTool_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "TileGeoModel/TileSwitches.h"

#include <string>

class StoreGateSvc;
class TileDetDescrManager;

class TileDetectorTool final : public GeoModelTool 
{
 public:

  TileDetectorTool(const std::string& type, 
           const std::string& name, 
           const IInterface* parent);

  virtual ~TileDetectorTool() override final;
  
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  bool isTestBeam() const { return m_switches.testBeam; }
  bool isAddPlates() const { return m_switches.addPlatesToCell; }
  int uShape() const { return m_switches.uShape; }

 private:

  StatusCode initIds();
  StatusCode createElements();
  void setSwitch(int & param, int value, const char * name);

  TileSwitches m_switches;

  bool m_not_locked;
  bool m_useNewFactory;
  std::string m_geometryConfig; // FULL, SIMU, RECO
  TileDetDescrManager* m_manager;
};

#endif 
