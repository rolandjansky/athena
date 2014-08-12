/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_TimeWalkGenerator.h
 * Header file for abstract base class ISCT_TimeWalkGeneratorTool
 * (c) ATLAS Detector software
 * Interface for the relative bunch calculator classes
 * Version 1.0 23/08/2007 Kondo Gnanvo
*/

#ifndef SCT_DIGITIZATION_ISCT_TIMEWALKGENERATOR_H
#define SCT_DIGITIZATION_ISCT_TIMEWALKGENERATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetSimEvent/SiTotalCharge.h"

namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ISCT_TimeWalkGenerator("ISCT_TimeWalkGenerator",1,0);

class ISCT_TimeWalkGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //** Retrieve interface ID */
  static const InterfaceID & interfaceID() {return IID_ISCT_TimeWalkGenerator; }

  // Destructor:
  virtual ~ISCT_TimeWalkGenerator() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // compute the relative bunch for a given threshold and charge
  virtual int relativeBunch(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const =0;
  virtual void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) =0;
};

#endif // SCT_DIGITIZATION_ISCT_TIMEWALKGENERATOR_H
