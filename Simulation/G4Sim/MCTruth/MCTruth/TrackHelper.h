/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackHelper_H
#define TrackHelper_H

#include "G4Track.hh"
#include "MCTruth/TrackInformation.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class TrackHelper {
public:
  TrackHelper(const G4Track* t);
  bool IsPrimary() const ;
  bool IsRegeneratedPrimary() const;
  bool IsRegisteredSecondary() const ;
  bool IsSecondary() const ;
  int GetBarcode() const ;
  TrackInformation * GetTrackInformation() {return m_trackInfo;}
  HepMcParticleLink GetParticleLink();
private:
  TrackInformation *m_trackInfo;
};

#endif
