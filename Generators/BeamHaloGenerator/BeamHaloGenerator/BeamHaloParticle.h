/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMHALOPARTICLE_H
#define BEAMHALOPARTICLE_H

#include "AtlasHepMC/SimpleVector.h"

class MarsParticle;
class FlukaParticle;
namespace HepPDT {
  class ParticleDataTable;
}

/** @class BeamHaloParticle

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to describe a generic beam halo particle.
*/
class BeamHaloParticle {
 public:
  
  BeamHaloParticle();
  
  BeamHaloParticle(long pdgId, 
		   HepMC::FourVector fourVector, 
		   HepMC::FourVector positionAtScoringPlane, 
		   double weight);
 
  BeamHaloParticle(long pdgId,
                   HepMC::FourVector fourVector,
                   HepMC::FourVector positionAtScoringPlane,
                   double weight,
                   HepMC::FourVector positionAtPrimary);


  BeamHaloParticle(const BeamHaloParticle& beamHaloParticle) = default;

  /** Assignment operator */
  BeamHaloParticle & operator=(const BeamHaloParticle &rhs) = default;

  /** A function to fill the data members from an input MarsParticle
      object. */
  int fill(const HepPDT::ParticleDataTable *particleDataTable,
	   MarsParticle *marsParticle);
  
  /** A function to fill the data members from an input FlukaParticle
      object. */
  int fill(const HepPDT::ParticleDataTable *particleDataTable,
	   FlukaParticle *flukaParticle);
  
  /** A function to print the contents of this particle. */
  void print();
  
  /** A function to return the PDG id of this particle. */
  long pdgId() const { return m_pdgId; }

  /** A function to return the momentum fourvector of this
      particle. */
  HepMC::FourVector fourVector() const { return m_fourVector; }

  /** A function to return the position fourvector of this particle
      with respect to the scoring plane. */
  HepMC::FourVector positionAtScoringPlane() const { return m_positionAtScoringPlane; } 

  /** A function to return the weight of this particle within the
      input beam background simulation file. */
  double weight() const { return m_weight; }

  /** A function to return the position fourvector of this particle
      with respect to the primary interaction. */
  HepMC::FourVector positionAtPrimary() const { return m_positionAtPrimary; }

 private:

  /** The PDG Id of the particle */
  long m_pdgId;

  /** A four vector describing this particle at the scoring plane. */
  HepMC::FourVector m_fourVector;
  
  /** Position of the particle at the scoring plane (x,y,z,t). */
  HepMC::FourVector m_positionAtScoringPlane;
  
  /** The resultant particle weight after normalisation and rescaling. */
  double m_weight;

  /** Position of the particle at the primary interaction point (x,y,z,y). */
  HepMC::FourVector m_positionAtPrimary;
};

#endif
