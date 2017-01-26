/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H

#include "LArG4Code/LArG4SimpleSD.h"
class EnergySpot;
class StoreGateSvc;

/// This is the interface for the fast simulation dedicated sensitive detector.
class IFastSimDedicatedSD : public LArG4SimpleSD {

 public:

  /// Simple constructor and destructor
  IFastSimDedicatedSD( std::string name , StoreGateSvc * detStore ) : LArG4SimpleSD( name , detStore ) {}

  ~IFastSimDedicatedSD() {}

  /// ProcessHitsMethod
  /** Process a single energy spot from a frozen shower.
      The appropriate region of the sensitive detector is calculated and a LArIdentifier is constructed*/
  virtual void ProcessSpot(const EnergySpot & spot) = 0;

};
#endif //LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H
