/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H

// This is the interface for the fast simulation dedicated sensitive detector.
#include "GaudiKernel/IAlgTool.h"
class EnergySpot;

class IFastSimDedicatedSD : virtual public IAlgTool {

 public:

  // ProcessHitsMethod
  /** Process a single energy spot from a frozen shower.
      The appropriate region of the sensitive detector is calculated and a LArIdentifier is constructed*/
  virtual void ProcessSpot(const EnergySpot & spot) = 0;

  // End each event (do hit merger here)
  /** End of event collection of hits. Merging is done in this method.*/
  virtual StatusCode EndOfAthenaEvent() = 0;

  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IFastSimDedicatedSD( "IFastSimDedicatedSD" , 1 , 0 ) ;
    return IID_IFastSimDedicatedSD;
  }
};
#endif //LARG4FASTSIMULATION_IFASTSIMDEDICATEDSD_H
