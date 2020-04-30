/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GenParticleJet.h
//   Header file for GenParticleJet helper class
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKVALEVENT_GENPARTICLEJET_H
#define TRK_TRKVALEVENT_GENPARTICLEJET_H

#include <vector>
#include <utility>
#include "CLHEP/Geometry/Vector3D.h"
#include "AtlasHepMC/GenParticle.h"

namespace Trk {

  /** @brief short class to organise MC generated particles
      as a jet.

      Needed at the level of tracking validation to be independent
      of reconstruction inefficiency inside the core of jets.

      @author Loek Hooft van Huysduynen
  */

class GenParticleJet
{
  
 public:
  GenParticleJet(): m_particles(), m_indices(), m_energy(0.), m_momentum(0.0, 0.0, 0.0), m_totalPt2(0.), m_totalMom2(0.), m_pDotJetAxis(0.), m_magP(0.)
	{
		
	}; // constructor

  ~GenParticleJet(){}; // destructor
  
  void setEnergy(double energy){m_energy = energy;}

  double getEnergy() const {return m_energy;}
  
  const HepMC::GenParticle* getParticle (unsigned int i) const
    {
      return m_particles.at(i);
    }
  std::pair<const HepMC::GenParticle*,int> getIndexedParticle(unsigned int i)
    {
      return std::make_pair(m_particles.at(i),m_indices.at(i));
    }
  
  std::vector<const HepMC::GenParticle* > getParticles() const {return m_particles;}

  std::vector<int> getIndicesInEvent() const {return m_indices;}

  int getNumParticles() const {return m_particles.size();}
  
  void addParticle(const HepMC::GenParticle* part,
                   int indexInEvent = -1 ){
    m_particles.push_back(part);
    m_indices.push_back(indexInEvent);
    m_energy = m_energy + part->momentum().e();
    HepGeom::Vector3D<double> moment(part->momentum().px(), part->momentum().py(), part->momentum().pz());
    m_momentum = m_momentum + moment;

    m_totalMom2 = m_totalMom2 + pow((moment.mag()), 2);
    m_magP = m_magP + moment.mag();
    HepGeom::Vector3D<double> partMom(part->momentum().px(), part->momentum().py(), part->momentum().pz());
    double longMoment = partMom.dot((m_momentum)/(m_momentum.mag()));
    m_pDotJetAxis = m_pDotJetAxis + longMoment;
    m_totalPt2 = m_totalPt2 + pow(partMom.mag(), 2) - pow(longMoment, 2);
  }

  HepGeom::Vector3D<double> getMomentum() const {return m_momentum;}

  double getThrust() const {return m_pDotJetAxis/m_magP;}
  double getSphericity() const {return m_totalPt2/m_totalMom2;}
  

 private:
  std::vector<const HepMC::GenParticle* > m_particles;
  std::vector< int > m_indices;
  double m_energy;
  HepGeom::Vector3D<double>  m_momentum;
  double m_totalPt2;
  double m_totalMom2;
  double m_pDotJetAxis;
  double m_magP;
};

} // end of namespace

#endif // TRK_TRKVALEVENT_GENPARTICLEJET_H
