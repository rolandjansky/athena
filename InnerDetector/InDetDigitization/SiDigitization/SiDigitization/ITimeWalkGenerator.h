/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ITimeWalkGenerator.h
 * Header file for abstract base class ITimeWalkGeneratorTool
 * (c) ATLAS Detector software
 * Interface for the relative bunch calculator classes
 * Version 1.0 23/08/2007 Kondo Gnanvo
*/

#ifndef SIDIGITIZATION_ITIMEWALKGENERATOR_H
#define SIDIGITIZATION_ITIMEWALKGENERATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetSimEvent/SiTotalCharge.h"

namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ITimeWalkGenerator("ITimeWalkGenerator",1,0);

class ITimeWalkGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //** Retrieve interface ID */
  static const InterfaceID & interfaceID() {return IID_ITimeWalkGenerator; }

  // Destructor:
  virtual ~ITimeWalkGenerator() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // compute the relative bunch for a given threshold and charge
  virtual int relativeBunch(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const =0;
  virtual void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) =0;
};

#endif // SIDIGITIZATION_ITIMEWALKGENERATOR_H
