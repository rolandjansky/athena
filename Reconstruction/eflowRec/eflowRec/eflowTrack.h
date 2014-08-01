/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWTRACK_H
#define EFLOWREC_EFLOWTRACK_H
/********************************************************************

NAME:     eflowTrack.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  10th November, 2001

********************************************************************/

enum Luminosity {LOW,HIGH};

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <utility>

class eflowTrack {
 public:
  /* Constructor sets the track */
  eflowTrack(const xAOD::TrackParticle* track, int eflowType, double trackExtrapolatedEta);
  ~eflowTrack();

  double sime()    const { return m_e; }
  double simvare() const { return m_vare; }

 private:
  void sim(int eflowType, double trackExtrapolatedEta);

  double getElectronEOverP(double trackEta, double trackPt) const;
  double getPiEOverP(double trackPt, double absEta) const;
  double getElectronEOverPStdDev(double trackPt, double trackEta) const;
  double getPiEOverPStdDev(double trackEta, double trackPt) const;

  const xAOD::TrackParticle* m_track;
  double m_e, m_vare;
};

#endif
