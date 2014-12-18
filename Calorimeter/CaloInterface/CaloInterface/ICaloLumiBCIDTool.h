/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICaloLumiBCIDTool_H
#define CALOINTERFACE_ICaloLumiBCIDTool_H

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include "Identifier/Identifier.h"

class CaloDetDescrElement;
class CaloCell;

class ICaloLumiBCIDTool: virtual public IAlgTool
{  
public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ICaloLumiBCIDTool("ICaloLumiBCIDTool", 1 , 0);
    return IID_ICaloLumiBCIDTool; 
  }


  virtual float average(const CaloCell* caloCell, unsigned int bcid)=0;

  virtual float average(const CaloDetDescrElement* caloDDE, unsigned int bcid)=0;

  virtual float average(const Identifier cellID, unsigned int bcid)=0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;
 
};

#endif 
