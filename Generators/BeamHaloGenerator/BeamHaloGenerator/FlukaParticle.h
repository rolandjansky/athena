/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FLUKAPARTICLE_H
#define FLUKAPARTICLE_H

#include "AtlasHepMC/SimpleVector.h"
#include <string>
#include <vector>

/** @class FlukaParticle

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to describe a FLUKA particle read from an input
    ASCII file.
*/
class FlukaParticle {
 public:
  FlukaParticle(int type = 0);
  FlukaParticle(const FlukaParticle& flukaParticle) = default;
  FlukaParticle& operator=(const FlukaParticle& flukaParticle) = default;

  /** A function to read the values from a single row definition and
      fill the data members. */
  int read(std::vector<std::string> *eventAsStringVector);

  /** A function to print the contents of this particle. */
  void print(void);

  /** A function to clear the values of this particle. */
  void clear(void);

  /** A function to return the type of this FlukaParticle. */ 
  int type(void) const { return m_type; }

  /** A function to return the FLUKA event Id of this particle. */
  long eventId(void) const { return m_eventId; }

  /** A function to return the FLUKA particle Id of this particle. */
  int flukaId(void) const { return m_flukaId; }

  /** A function to return the particle generation number of this
      particle. */
  int partGenNum(void) const { return m_partGenNum; }

  /** A function to return the relativistic kinetic energy of this
      particle. */
  double kineticEnergy(void) const { return m_kineticEnergy; }

  /** A function to return the fourvector position with respect to the
      scoring plane of this particle. */
  HepMC::FourVector positionAtScoringPlane(void) const { return m_positionAtScoringPlane; }

  /** A function to return the directional cosines of this
      particle. The fourth component is 0 */
  HepMC::FourVector directionalCosines(void) const { return m_directionalCosines; }  

  /** A function to return the particle or event weight. */
  double weight(void) const { return m_weight; }

  /** A function to return the total energy. */
  double totalEnergy(void) const { return m_totalEnergy; }

  /** A function to return the position of the primary interaction. */
  HepMC::FourVector positionAtPrimary(void) const { return m_positionAtPrimary; }

  /** A function to return the PDG id of this particle. */
  int pdgId();

  enum flukaParticleType {
    BRUCE,
    TALANOV};

 private:
  int m_type;

  long m_eventId;
  int m_flukaId;
  int m_partGenNum;
  double m_kineticEnergy;
  HepMC::FourVector m_positionAtScoringPlane;
  HepMC::FourVector m_directionalCosines;
  double m_weight;
  double m_totalEnergy;
  HepMC::FourVector m_positionAtPrimary;

};

#endif
