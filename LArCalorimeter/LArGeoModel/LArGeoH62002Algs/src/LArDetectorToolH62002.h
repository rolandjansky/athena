/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDETECTORTOOLH62002_H
#define LARDETECTORTOOLH62002_H

#include "GeoModelUtilities/GeoModelTool.h"

class LArDetectorToolH62002 final : public GeoModelTool {

public:

    // Standard Constructor
  LArDetectorToolH62002( const std::string& type, const std::string& name, const IInterface* parent );
  
  // Standard Destructor
  virtual ~LArDetectorToolH62002() override final;
  
  virtual StatusCode create() override final;
  
};

#endif 
