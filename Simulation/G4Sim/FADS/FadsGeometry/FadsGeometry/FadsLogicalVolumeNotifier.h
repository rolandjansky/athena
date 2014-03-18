/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsLogicalVolumeNotifier_H
#define FadsLogicalVolumeNotifier_H

#include "G4VNotifier.hh"

namespace FADS {

  class FadsLogicalVolumeNotifier: public G4VNotifier {
    friend class GeometryManager;
  private:
    FadsLogicalVolumeNotifier();
  public:
    void NotifyRegistration();
    void NotifyDeRegistration();
  };

}

#endif
