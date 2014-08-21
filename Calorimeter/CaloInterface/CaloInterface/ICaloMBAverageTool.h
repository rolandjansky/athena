/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOMBAVERAGETOOL_H
#define CALOINTERFACE_ICALOMBAVERAGETOOL_H

#include "CaloIdentifier/CaloGain.h"

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

class Identifier;
class CaloDetDescrElement;
class CaloCell;

class ICaloMBAverageTool: virtual public IAlgTool
{  
public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ICaloMBAverageTool("ICaloMBAverageTool", 1 , 0);
    return IID_ICaloMBAverageTool; 
  }


  virtual float average(const CaloCell* caloCell)=0;

  virtual float average(const CaloDetDescrElement* caloDDE, const CaloGain::CaloGain gain)=0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;
 
};

#endif 
