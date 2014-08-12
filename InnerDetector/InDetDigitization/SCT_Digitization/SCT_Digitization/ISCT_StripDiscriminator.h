/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_StripDiscriminator.h
 * Interface class for class SCT_StripDiscriminator 
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

static const InterfaceID IID_ISCT_StripDiscriminator("ISCT_StripDiscriminator", 1, 0);

class ISCT_StripDiscriminator: virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //** Retrieve interface ID */
  static const InterfaceID & interfaceID() {return IID_ISCT_StripDiscriminator; }

  /** Destructor */
  virtual ~ISCT_StripDiscriminator() {}

  /** process the collection of pre digits */
  virtual void process(SiChargedDiodeCollection &collection) const =0;
  virtual void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) =0;

};

#endif // SCT_DIGITIZATION_ISCT_STRIPDISCRIMINATOR_H
