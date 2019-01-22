/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_FrontEnd.h
 * Header file for interface class for SCT_FrontEnd
 * (c) ATLAS Detector software
 */

#ifndef SCT_DIGITIZATION_ISCT_FRONTEND_H
#define SCT_DIGITIZATION_ISCT_FRONTEND_H

//Inheritance
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "SiDigitization/SiChargedDiode.h"

//methods
#include "Identifier/Identifier.h"
class SiChargedDiodeCollection;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ISCT_FrontEnd("ISCT_FrontEnd", 1, 0);

class ISCT_FrontEnd : virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_ISCT_FrontEnd; }

  //** Destructor: */
  virtual ~ISCT_FrontEnd() {}

};

#endif // SCT_DIGITIZATION_ISCT_FRONTEND_H
