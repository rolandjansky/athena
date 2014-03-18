/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsPhysicalVolumeNotifier_H
#define FadsPhysicalVolumeNotifier_H

#include "G4VNotifier.hh"

namespace FADS {

  class GeometryManager;

  class FadsPhysicalVolumeNotifier: public G4VNotifier {
    friend class GeometryManager;
  private:
    FadsPhysicalVolumeNotifier();
  public:
    void NotifyRegistration();
    void NotifyDeRegistration();
  };

}

#endif
