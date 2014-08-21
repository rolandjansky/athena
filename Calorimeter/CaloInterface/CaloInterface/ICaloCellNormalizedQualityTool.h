/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOCELLNORMALIZEDQUALITYTOOL_H
#define CALOINTERFACE_ICALOCELLNORMALIZEDQUALITYTOOL_H

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"

class CaloCell;

class ICaloCellNormalizedQualityTool: virtual public IAlgTool
{  
public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ICaloCellNormalizedQualityTool("ICaloCellNormalizedQualityTool", 1 , 0);
    return IID_ICaloCellNormalizedQualityTool;
  }

  virtual StatusCode normalizedQuality(const CaloCell* cell, float& qNorm, unsigned int& type) const=0;

 
};

#endif 
