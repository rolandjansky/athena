/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBEDDEDTRACKGENERATOR_H
#define EMBEDDEDTRACKGENERATOR_H

#include "EmbeddedTrackGenerator/EmbeddedGeneratorCommon.h"
#include "EmbeddedTrackGenerator/GenPDG_Property.h"

/// EmbeddedTrackGenerator generates particles "embedded" in
/// reconstructed jets. The jets are taken from an AOD file.
/// 
/// @author Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008
/// 

class EmbeddedTrackGenerator : public EmbeddedGeneratorCommon,
                               private GenPDG_Property
{
public:
 
  EmbeddedTrackGenerator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode embeddedGenInitialize();
  virtual StatusCode embeddedFillEvt(HepMC::GenEvent* evt);
  
private:

  /// @name Settable properties
  //@{
  
  /// StoreGate key  for the jet collection 
  std::string m_jetName;

  /// Maximum number of particles in an event.  The actual number will be min(m_numberOfParticles, num_jets)
  unsigned int m_maxParticles;
  
  /// Momentum of generated particles
  double m_ptot;
  
  /// deltaR between the jet axis and the corresponding particle. 
  /// Particle direction is sampled from a flat 2D distribution in (eta,phi).
  double m_dR_max;
  
  /// maximum  displacement of generated particle vertex from the origin of its respective jet - flat 3D distribution.
  double m_displacementMax;

  //@}

};

#endif /* EMBEDDEDTRACKGENERATOR_H*/
