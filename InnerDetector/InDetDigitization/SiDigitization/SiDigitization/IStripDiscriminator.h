/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * IStripDiscriminator.h
 * Interface class for class StripDiscriminator 
 * (c) ATLAS Detector software 
 */

#ifndef SCT_DIGITIZATION_ISCT_STRIPDISCRIMINATOR_H
#define SCT_DIGITIZATION_ISCT_STRIPDISCRIMINATOR_H

//Inheritance
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"

//methods
class SiChargedDiodeCollection;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_IStripDiscriminator("IStripDiscriminator", 1, 0);

class IStripDiscriminator: virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //** Retrieve interface ID */
  static const InterfaceID & interfaceID() {return IID_IStripDiscriminator; }

  /** Destructor */
  virtual ~IStripDiscriminator() {}

  /** process the collection of pre digits */
  virtual void process(SiChargedDiodeCollection &collection) const =0;
  virtual void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) =0;

};

#endif // SCT_DIGITIZATION_ISCT_STRIPDISCRIMINATOR_H
