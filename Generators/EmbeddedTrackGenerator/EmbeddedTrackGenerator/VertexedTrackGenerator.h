/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXEDTRACKGENERATOR_H
#define VERTEXEDTRACKGENERATOR_H

#include "EmbeddedTrackGenerator/EmbeddedGeneratorCommon.h"
#include "EmbeddedTrackGenerator/GenPDG_Property.h"

/**
@class VertexedTrackGenerator

VertexedTrackGenerator generates particles originating at a reconstructed vertex position,
which is taken from an AOD file.

@author Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008

*/

class VertexedTrackGenerator;

class VertexedTrackGenerator: public EmbeddedGeneratorCommon,
                              private GenPDG_Property
{
public:

  VertexedTrackGenerator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode embeddedGenInitialize();
  virtual StatusCode embeddedFillEvt(HepMC::GenEvent* evt);
  
private:

  /// @name Settable properties
  //@{

  /// StoreGate key  for the vertex collection
  std::string m_vertexCollectionName;

  /// Number of particles to throw for each reconstructed vertex.
  unsigned int m_particlesPerVertex;

  /// Maximum number of vertexes to use in an event.
  unsigned int m_maxVertexes;

  /// Momentum of generated particles is flat between m_ptMin and m_ptMax.
  /// Set m_ptMin==m_ptMax to get a specific pt value.
  double m_ptMin;
  double m_ptMax;

  /// Pseudorapidity of generated particles is flat between m_etaMin and m_etaMax.
  /// Set m_etaMin==m_etaMax to get a specific eta value.
  double m_etaMin;
  double m_etaMax;

  /// Azimuthal angle of generated particles is flat between m_phiMin
  /// and m_phiMax.  Set m_phiMin==m_phiMax to get a specific eta
  /// value.  The default range is (-pi, pi).  (The proper thing would
  /// be e.g. [-pi,pi) but we use HepRandomEngine that always excludes
  /// endpoints of a range...)
  double m_phiMin;
  double m_phiMax;

  //@}

};


#endif /* VERTEXEDTRACKGENERATOR_H*/
