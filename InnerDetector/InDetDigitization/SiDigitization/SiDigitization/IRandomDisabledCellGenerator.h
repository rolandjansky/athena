// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * IRandomDisabledCellGenerator.h
 * Header file for interface class for RandomDisabledCellGenerator
 * (c) ATLAS Detector software
 */

#ifndef SIDIGITIZATION_IRANDOMDISABLEDCELLGENERATOR_H
#define SIDIGITIZATION_IRANDOMDISABLEDCELLGENERATOR_H

//Inheritance
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"

//methods
class SiChargedDiodeCollection;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_IRandomDisabledCellGenerator("IRandomDisabledCellGenerator",1,0);

class IRandomDisabledCellGenerator : virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRandomDisabledCellGenerator; }

  // Destructor:
  virtual ~IRandomDisabledCellGenerator() {}

};

#endif // SIDIGITIZATION_IRANDOMDISABLEDCELLGENERATOR_H
