/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_IMM_DIGITIZATIONTOOL_H
#define MM_DIGITIZATION_IMM_DIGITIZATIONTOOL_H

#include "MM_Digitization/MM_DigitToolOutput.h"
#include "GaudiKernel/IAlgTool.h"
/*-----------------------------------------------

Created March 2013 by Nektarios Chr. Benekos

Interface for tools which convert MicroMegas digitization input quantities into the signal
-----------------------------------------------*/

class MM_DigitToolInput;

static const InterfaceID IID_IMM_DigitizationTool("IMM_DigitizationTool",1,0);

class IMM_DigitizationTool : virtual public IAlgTool {
 public:

  virtual MM_DigitToolOutput digitize(/*const MmDigitToolInput& input*/) = 0;

  static const InterfaceID& interfaceID()
    { return IID_IMM_DigitizationTool; }
};

#endif
