/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * IFrontEnd.h
 * Header file for interface class for FrontEnd
 * (c) ATLAS Detector software
 */

#ifndef SIDIGITIZATION_IFRONTEND_H
#define SIDIGITIZATION_IFRONTEND_H

//Inheritance
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "SiDigitization/SiChargedDiode.h"

//methods
#include "Identifier/Identifier.h"
class SiChargedDiodeCollection;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_IFrontEnd("IFrontEnd", 1, 0);

class IFrontEnd : virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_IFrontEnd; }

  //** Destructor: */
  virtual ~IFrontEnd() {}

};

#endif // SIDIGITIZATION_IFRONTEND_H
