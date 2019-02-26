/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_IMDT_DIGITIZATIONTOOL_H
#define MDT_DIGITIZATION_IMDT_DIGITIZATIONTOOL_H

#include "MDT_Digitization/MdtDigiToolOutput.h"
#include "GaudiKernel/IAlgTool.h"
/*-----------------------------------------------
  
   Created 7-5-2004 by Niels van Eldik

 Interface for tools which convert MDT digitization input quantities into
 the signal 
-----------------------------------------------*/
namespace CLHEP {
  class HepRandomEngine;
}
class MdtDigiToolInput;

static const InterfaceID IID_IMDT_DigitizationTool("IMDT_DigitizationTool",1,0);

class IMDT_DigitizationTool : virtual public IAlgTool {
 public:

  virtual MdtDigiToolOutput digitize(const MdtDigiToolInput& input, CLHEP::HepRandomEngine *rndmEngine) = 0;

  static const InterfaceID& interfaceID()
    { return IID_IMDT_DigitizationTool; }
};

#endif
