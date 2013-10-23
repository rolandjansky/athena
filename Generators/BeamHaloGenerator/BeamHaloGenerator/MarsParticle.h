/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MARSPARTICLE_H
#define MARSPARTICLE_H

#include "HepMC/SimpleVector.h"
#include <vector>
#include <string>

/** @class MarsShowerPrimary

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to describe the primary shower that produced a
    given MARS particle.
*/
class MarsShowerPrimary {
 public:
  MarsShowerPrimary();
  
  int particleId; //!< IORIG
  int processId; //!< KORIG
  double weight; //!< WORIG
  HepMC::ThreeVector position; //!< XORG, YORG, ZORG
  double kineticEnergy; //!< EORIG
};


/** @class MarsBeamGasInteraction

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to describe the beam gas interaction that produced
    a iven MARS particle.
*/
class MarsBeamGasInteraction {
 public:
  MarsBeamGasInteraction();
  
  int nucleusType; //!< INUC
  int nevve; //!< NEVVE
  double timeOfFlight; //!< ZBGASHIT
};


/** @class MarsParticle

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to describe a MARS particle read from an input
    ASCII file.
*/
class MarsParticle {
 public:
  MarsParticle();

  /** A function to read the values from a single row definition and
      fill the data members. */
  int read(std::vector<std::string> *eventAsStringVector);

  /** A function to print the contents of this particle. If the
      beamGas value is true then the beam gas information is
      printed. */
  void print(bool beamGas);

  /** A function to return the event number of this particle. */
  long eventNumber() const { return m_eventNumber; }

  /** A function to return the particle identify of this particle. */
  int particleId() const { return m_particleId; }

  /** A function to return the relativistic kinetic energy of this
      particle. */
  double kineticEnergy() const { return m_kineticEnergy; }

  /** A function to return the weight associated with this particle in
      the input ASCII file. */
  double weight() const { return m_weight; }

  /** A function to the position of this particle with respect to the
      scoring plane. */
  HepMC::ThreeVector positionAtScoringPlane() const { return m_positionAtScoringPlane; }

  /** A function to return the directional cosines of this
      particle. */
  HepMC::ThreeVector directionalCosines() const { return m_directionalCosines; }

  /** A function to return the time of flight for this particle. */
  double timeOfFlight() const { return m_timeOfFlight; }

  /** A function to return the primary proton Z position for this
      particle. */
  double primaryProtonZ() const { return m_primaryProtonZ; }

  /** A function to return the shower primary infomation for this
      particle. */
  MarsShowerPrimary showerPrimary() const { return m_showerPrimary; }

  /** A function to return the beam gas interaction information for
      this particle. */
  MarsBeamGasInteraction beamGasInteraction() const { return m_beamGasInteraction; }

  /** A function to return the PDG id for this particle. */
  int pdgId();
  
 private:
  long m_eventNumber;
  int m_particleId;
  double m_kineticEnergy;
  double m_weight;
  HepMC::ThreeVector m_positionAtScoringPlane;
  HepMC::ThreeVector m_directionalCosines;
  double m_timeOfFlight;
  double m_primaryProtonZ;
  MarsShowerPrimary m_showerPrimary;
  MarsBeamGasInteraction m_beamGasInteraction;
};

#endif
