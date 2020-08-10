/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/BeamHaloParticle.h"
#include "BeamHaloGenerator/MarsParticle.h"
#include "BeamHaloGenerator/FlukaParticle.h"
#include "HepPDT/ParticleDataTable.hh"
#include <iostream>
#include <cmath>

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(): m_pdgId(0), 
				      m_fourVector(), 
				      m_positionAtScoringPlane(), 
				      m_weight(0.),
                                      m_positionAtPrimary() {
}

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(long pdgId, 
				   HepMC::FourVector fourVector, 
				   HepMC::FourVector positionAtScoringPlane, 
				   double weight): m_pdgId(pdgId), 
						   m_fourVector(fourVector), 
						   m_positionAtScoringPlane(positionAtScoringPlane), 
						   m_weight(weight) {
}

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(long pdgId,
                                   HepMC::FourVector fourVector,
                                   HepMC::FourVector positionAtScoringPlane,
                                   double weight,
                                   HepMC::FourVector positionAtPrimary): m_pdgId(pdgId),
                                                                         m_fourVector(fourVector),
                                                                         m_positionAtScoringPlane(positionAtScoringPlane),
                                                                         m_weight(weight),
                                                                         m_positionAtPrimary(positionAtPrimary) {
}

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(const BeamHaloParticle& beamHaloParticle) {
  m_pdgId = beamHaloParticle.m_pdgId;
  m_fourVector = beamHaloParticle.m_fourVector;
  m_positionAtScoringPlane = beamHaloParticle.m_positionAtScoringPlane; 
  m_weight = beamHaloParticle.m_weight;
  m_positionAtPrimary = beamHaloParticle.m_positionAtPrimary;
}

//---------------------------------------------------------------------

//BeamHaloParticle::operator(const BeamHaloParticle &rhs) {
//}

//---------------------------------------------------------------------

int BeamHaloParticle::fill(const HepPDT::ParticleDataTable *particleDataTable,
			   MarsParticle *marsParticle) {
  double p_sq, mod_p, pz, mass, energy;
  
  m_pdgId = marsParticle->pdgId();
  if(!m_pdgId) {
    std::cerr << "There is no PDG code for MARS id " << marsParticle->particleId() << std::endl;
    return 1;
  }
  
  // Read mass from pdg table
  const HepPDT::ParticleData* particleData = particleDataTable->particle(HepPDT::ParticleID(std::abs(m_pdgId)));
  mass = 0;
  if(particleData) {
    mass = particleData->mass().value();
  }
  else {
    std::cerr << "PDG code " << m_pdgId << " is not in the particle data table." << std::endl;
    return 1;
  }

  // Mars uses GeV, convert to MeV then calculate |p|
  //   from relativistic kinetic energy (using c=1):
  //     KE = m - m_0            m^2 = p^2 + m_0^2
  p_sq = std::pow(marsParticle->kineticEnergy() * 1000.0 + mass, 2) - std::pow(mass,2);
  mod_p = std::sqrt(p_sq);

  // ATLAS: the x-axis points towards the centre of the LHC ring, the
  // y-axis points upwards, and the z-axis points towards the airport
  // and the Saleve, i.e. towards the A-side.
  //
  // MARS simulation: the x-axis points into the ground, the y-axis
  // points out of the ring, and the z-axis points towards the Jura
  // i.e. towards the C-side.  

  // Calculate px and py from the directional cosines
  m_fourVector.setX(-1.0 * marsParticle->directionalCosines().y() * mod_p);
  m_fourVector.setY(-1.0 * marsParticle->directionalCosines().x() * mod_p);

  // Calculate pz from sqrt(p^2 - p_T^2)
  pz = std::sqrt(p_sq - m_fourVector.perp2());
  m_fourVector.setZ(pz);  // This is always +ve.  Corrected during conversion to HepParticle  

  // Calculate the energy
  energy=std::sqrt(std::pow(mass,2)+std::pow(mod_p,2));
  m_fourVector.setE(energy);

  // Convert the position from cm to mm.
  m_positionAtScoringPlane.setX(-1.0 * marsParticle->positionAtScoringPlane().y() * 10.0);
  m_positionAtScoringPlane.setY(-1.0 * marsParticle->positionAtScoringPlane().x() * 10.0);
  m_positionAtScoringPlane.setZ(0.0); // The information is not provided in the input file.
  m_positionAtScoringPlane.setT(0.0); // Assume that this is t = 0

  // replace this with marsParticle->effectiveWeight();
  m_weight = marsParticle->weight();

  return 0;
}

//-------------------------------------------------------------------------------------------------

int BeamHaloParticle::fill(const HepPDT::ParticleDataTable *particleDataTable,
			   FlukaParticle *flukaParticle) {
  double p_sq, mod_p, pz, mass, energy;

  // Needed for debugging
  //flukaParticle->print();

  m_pdgId = flukaParticle->pdgId();
  if(!m_pdgId) {
    std::cerr << "There is no PDG code for FLUKA id " << flukaParticle->flukaId() << std::endl;
    return 1;
  }

  // Read mass from pdg table
  const HepPDT::ParticleData* particleData = particleDataTable->particle(HepPDT::ParticleID(std::abs(m_pdgId)));
  mass = 0;
  if(particleData) {
    mass = particleData->mass().value();
  }
  else {
    std::cerr << "PDG code " << m_pdgId << " is not in the particle data table." << std::endl;
    return 1;
  }

  // FLUKA uses GeV, convert to MeV then calculate |p|
  //   from relativistic kinetic energy (using c=1):
  //     KE = m - m_0            m^2 = p^2 + m_0^2
  p_sq = std::pow(flukaParticle->kineticEnergy() * 1000.0 + mass, 2) - std::pow(mass,2);
  mod_p = std::sqrt(p_sq);

  // Calculate px, py, pz from the directional cosines
  m_fourVector.setX(flukaParticle->directionalCosines().x() * mod_p);
  m_fourVector.setY(flukaParticle->directionalCosines().y() * mod_p);

  // If the z component of the directional cosine has not been set recalculate it.
  if(std::abs(flukaParticle->directionalCosines().z()) < 0.0001) {
    // Calculate pz from sqrt(p^2 - p_T^2)
    pz = std::sqrt(p_sq - m_fourVector.perp2());
    m_fourVector.setZ(pz);  // This is always +ve.  Corrected during conversion to HepParticle
  }
  else {  
    m_fourVector.setZ(flukaParticle->directionalCosines().z() * mod_p);
  }

  // Calculate the energy
  energy=std::sqrt(std::pow(mass,2)+std::pow(mod_p,2));
  m_fourVector.setE(energy);

  // Convert the position from cm to mm.
  m_positionAtScoringPlane.setX(flukaParticle->positionAtScoringPlane().x()*10.0); // Convert from cm to mm
  m_positionAtScoringPlane.setY(flukaParticle->positionAtScoringPlane().y()*10.0); // Convert from cm to mm
  m_positionAtScoringPlane.setZ(flukaParticle->positionAtScoringPlane().z()*10.0); // Convert from cm to mm
  m_positionAtScoringPlane.setT(flukaParticle->positionAtScoringPlane().t());

  // Get the weight
  m_weight = flukaParticle->weight();

  // Convert the position from cm to mm.
  m_positionAtPrimary.setX(flukaParticle->positionAtPrimary().x()*10.0); // Convert from cm to mm
  m_positionAtPrimary.setY(flukaParticle->positionAtPrimary().y()*10.0); // Convert from cm to mm
  m_positionAtPrimary.setZ(flukaParticle->positionAtPrimary().z()*10.0); // Convert from cm to mm
  m_positionAtPrimary.setT(flukaParticle->positionAtPrimary().t());

  // ATLAS: the x-axis points towards the centre of the LHC ring, the
  // y-axis points upwards, and the z-axis points towards the airport
  // and the Saleve, i.e. towards the A-side.
  //
  // R. Bruce fluka simulation: the x-axis points away from the 
  // center of the LHC ring, the y-axis points upwards, and the z-axis 
  // points along the beam line, but is positive for beam 1 and beam 2 
  // input files. 
  if(flukaParticle->type() == 0) {
    m_fourVector.setX(-m_fourVector.x());
    m_positionAtScoringPlane.setX(-m_positionAtScoringPlane.x());
    m_positionAtPrimary.setX(-m_positionAtPrimary.x());
  }

  return 0;
}

//-------------------------------------------------------------------------------------------------

void BeamHaloParticle::print() {
  std::cout.fill(' ');
  std::cout.precision(6);
  std::cout.width(11); std::cout << m_pdgId << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_fourVector.px() << " "; 
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_fourVector.py() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_fourVector.pz() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_fourVector.e() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.z() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.t() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_weight << std::endl; 
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.x() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.y() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.z() << " ";
  std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.t() << std::endl; 
}

//-------------------------------------------------------------------------------------------------
