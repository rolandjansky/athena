/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
MC Truth object associated with an TrackParticle object

***************************************************************************

 ***************************************************************************/
#ifndef PARTICLETRUTH_TRACKPARTICLETRUTH_H
#define PARTICLETRUTH_TRACKPARTICLETRUTH_H
#include "GeneratorObjects/HepMcParticleLink.h"

class TrackParticleTruth {

 public:
  TrackParticleTruth();
  TrackParticleTruth(const HepMcParticleLink& particleLink, float probability);
  TrackParticleTruth(const TrackParticleTruth& other);
  TrackParticleTruth & operator=(const TrackParticleTruth& ) = default;
  virtual ~TrackParticleTruth();
  int barcode() const;
  const HepMcParticleLink& particleLink() const;
  float probability() const;

 private:
  HepMcParticleLink m_particleLink;
  float m_probability; //probability of being m_barcode the truth information
};

inline int TrackParticleTruth::barcode() const
{
 // EJWM - I've kept this in for the moment to avoid breaking existing code. In my opinion it should be removed though. EJWM.
  return m_particleLink.barcode();
}

inline const HepMcParticleLink& TrackParticleTruth::particleLink() const
{
  return m_particleLink;
}

inline float TrackParticleTruth::probability() const 
{
  return m_probability;
}

#endif // PARTICLE_TRACKPARTICLETRUTH_H

