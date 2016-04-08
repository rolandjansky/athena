/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef ILARCELLHVCORRTOOL
#define ILARCELLHVCORRTOOL

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

static const InterfaceID IID_ILArCellHVCorrTool("ILArCellHVCorrTool", 1 ,0);

class Identifier;
 /**
  * Abstract interface for LArCellHVCorr tool that also derives from CaloCellCorrection
  * Purpose: Allow to used LArCellHVCorr  tool also from the CaloNoiseTool
  */
class ILArCellHVCorrTool: virtual public IAlgTool {
  public: 
  
  virtual ~ILArCellHVCorrTool () { } 

  static const InterfaceID& interfaceID() { return IID_ILArCellHVCorrTool; }

  virtual float getCorrection(const Identifier id)=0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;

  virtual bool updateOnLastCallback() =0;

};

#endif
