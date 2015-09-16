/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PVNotifier_H
#define PVNotifier_H

// Notifier class to prepend detector name to all G4 Physical Volumes
// Only to be used by the DetectorGeometrySvc

#include "G4VNotifier.hh"

class DetectorGeometrySvc;

  class PVNotifier: public G4VNotifier {
  friend class DetectorGeometrySvc;
  private:
    PVNotifier(DetectorGeometrySvc*);
	
	DetectorGeometrySvc* m_detGeoSvc;
  public:
    void NotifyRegistration();
    void NotifyDeRegistration();
  };

#endif
