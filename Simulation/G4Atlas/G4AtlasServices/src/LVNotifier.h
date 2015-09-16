/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVNotifier_H
#define LVNotifier_H

#include "G4VNotifier.hh"

class DetectorGeometrySvc;

  class LVNotifier: public G4VNotifier {
    friend class DetectorGeometrySvc;
  private:
    LVNotifier(DetectorGeometrySvc *);
	
	DetectorGeometrySvc* m_detGeoSvc;
  public:
    void NotifyRegistration();
    void NotifyDeRegistration();
  };

#endif
