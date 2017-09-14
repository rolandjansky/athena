/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_RandomDisabledCellGenerator.h
 * Header file for interface class for SCT_RandomDisabledCellGenerator
 * (c) ATLAS Detector software
 */

#ifndef SCT_DIGITIZATION_ISCT_RANDOMDISABLEDCELLGENERATOR_H
#define SCT_DIGITIZATION_ISCT_RANDOMDISABLEDCELLGENERATOR_H

//Inheritance
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"

//methods
class SiChargedDiodeCollection;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ISCT_RandomDisabledCellGenerator("ISCT_RandomDisabledCellGenerator",1,0);

class ISCT_RandomDisabledCellGenerator : virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISCT_RandomDisabledCellGenerator; }

  // Destructor:
  virtual ~ISCT_RandomDisabledCellGenerator() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // process the collection of charged diodes
  virtual void process(SiChargedDiodeCollection& collection) const =0;
  virtual void setRandomEngine(CLHEP::HepRandomEngine* rndmEngine) =0;
};

#endif // SCT_DIGITIZATION_ISCT_RANDOMDISABLEDCELLGENERATOR_H
