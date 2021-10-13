/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCDETTOOL_H
#define ZDCDETTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

class ZDC_DetTool final : public GeoModelTool
{

public:

  ZDC_DetTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ZDC_DetTool() override final;
  
  virtual StatusCode create() override final;
};

#endif // ZDCDETTOOL_H
