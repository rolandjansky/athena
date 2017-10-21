/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCIDDETECTORTOOL_H
#define LUCIDDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
class LUCID_DetectorManager;

class LUCID_DetectorTool: public GeoModelTool  {

 public:
  
  LUCID_DetectorTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~LUCID_DetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

 private:
  const LUCID_DetectorManager* m_manager;
};

#endif 
