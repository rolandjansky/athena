/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/FlukaHaloGenerator.h"

#include "BeamHaloGenerator/AsciiInput.h"
#include "BeamHaloGenerator/BeamHaloGeneratorAlg.h"
#include "BeamHaloGenerator/BeamHaloGeneratorSettings.h"

#include "AtlasHepMC/GenEvent.h"
#include "CLHEP/Random/RandFlat.h"
#include <cmath>

FlukaHaloGenerator::FlukaHaloGenerator(int type,
                                       const HepPDT::ParticleDataTable* particleTable,
                                       CLHEP::HepRandomEngine* engine,
                                       std::string inputFile,
                                       std::vector<std::string> generatorSettings):
  BeamHaloGenerator(particleTable, engine, inputFile, generatorSettings),
  m_sameEvent(true),
  m_firstEvent(true),
  m_flukaParticle(type),
  m_lastFlukaParticle() {
}

//--------------------------------------------------------------------------


FlukaHaloGenerator::~FlukaHaloGenerator() {
}

//--------------------------------------------------------------------------


int FlukaHaloGenerator::genInitialize() {
  int ret_val;

  // Initialise base class
  if((ret_val = BeamHaloGenerator::genInitialize()) != 0) return ret_val;

  if(m_asciiInput->open() != 0) {
    std::cout << "Error: Could not open ascii input file " << m_inputFile << std::endl;
    return 1;
  }

  std::cout << "Info: Reading ascii input file " << m_inputFile << std::endl;

  return 0;
}

//--------------------------------------------------------------------------


int FlukaHaloGenerator::fillEvt(HepMC::GenEvent* evt) {
  std::vector<BeamHaloParticle> beamHaloEvent;
  int ret_val;

  // Read one FLUKA event which passes the generator settings.
  if((ret_val = readEvent(&beamHaloEvent)) != 0) return ret_val;

  // Convert the particles to GenParticles and attach them to the
  // event.  Flip the event if needed.
  if((ret_val = BeamHaloGenerator::convertEvent(&beamHaloEvent, evt)) != 0) return ret_val;

  // Set the event number
  evt->set_event_number(m_eventNumber);

  // Set the signal process
  HepMC::set_signal_process_id(evt,BeamHaloGenerator::FLUKA_SHOWER);

  // Increment the event number
  m_eventNumber++;

  return 0;
}

//--------------------------------------------------------------------------

int FlukaHaloGenerator::genFinalize() {
  int ret_val;

  m_asciiInput->close();

  // Finalise base class
  if((ret_val = BeamHaloGenerator::genFinalize()) != 0) return ret_val;

  return 0;
}

//--------------------------------------------------------------------------

int FlukaHaloGenerator::readParticle(BeamHaloParticle *beamHaloParticle) {

  std::cout << "Warning: FlukaHaloGenerator::readParticle is not yet available: "<< beamHaloParticle  << std::endl;
  return 0;
}

//--------------------------------------------------------------------------

int FlukaHaloGenerator::readEvent(std::vector<BeamHaloParticle> *beamHaloEvent) {
  BeamHaloParticle beamHaloParticle;

  // Clear the event
  beamHaloEvent->clear();

  // If there was a last event.
  if(!m_firstEvent) {

    // If the last event caused the same event flag to be set to false
    // copy the last particle into the vector of those in this event.
    if(!m_sameEvent) {
      // Fill the BeamHaloParticle with the data in the FlukaParticle
      if(beamHaloParticle.fill(m_particleTable, &m_lastFlukaParticle)) {
        std::cout << "Error: Conversion from FlukaParticle to BeamHaloParticle failed." << std::endl;
        return 1;
      }
      // Append the BeamHalo particle to the event.
      beamHaloEvent->push_back(beamHaloParticle);

      // Set the same event flag to enter the while loop to read the
      // rest of this event.
      m_sameEvent = true;
    }
  }

  // Loop over the ascii input and read each particle until a new
  // event is found or there are no more particles.
  std::vector<std::string> row;
  bool endOfFile = false;
  while(m_sameEvent && !endOfFile) {
    row = m_asciiInput->readRow(); // Read one line of the ascii file.11

    if(row.size() == 0) {
      endOfFile = true;
      continue;
    }

    if(m_flukaParticle.read(&row)) { // Fill the particle from the string vector
      continue;
    }

    // Check if there was a last particle.
    if(!m_firstEvent) {

      m_sameEvent = false;

      // Check if the event id of the last particle is the same as this particle.
      if((m_lastFlukaParticle.type() == FlukaParticle::BRUCE)
         && (m_lastFlukaParticle.eventId() == m_flukaParticle.eventId())
         && (m_lastFlukaParticle.partGenNum() == m_flukaParticle.partGenNum())) m_sameEvent = true;

      if((m_lastFlukaParticle.type() == FlukaParticle::TALANOV)
         && (m_lastFlukaParticle.eventId() == m_flukaParticle.eventId())) m_sameEvent = true;
    }
    else {
      // For the first event.
      m_firstEvent = false;
      m_sameEvent = true;
    }

    // If this is the same event copy the particle into the vector for
    // this event.
    if(m_sameEvent) {

      // Fill the BeamHaloParticle with the data in the FlukaParticle
      if(beamHaloParticle.fill(m_particleTable, &m_flukaParticle)) {
        std::cout << "Error: Conversion from FlukaParticle to BeamHaloParticle failed." << std::endl;
        return 1;
      }

      // Append the BeamHalo particle to the event.
      beamHaloEvent->push_back(beamHaloParticle);
    }

    // Copy this particle into the last particle.
    m_lastFlukaParticle = m_flukaParticle;

  }

  if(beamHaloEvent->size() == 0) {
    std::cout << "Error: No particles read from " << m_inputFile << std::endl;
    return 1;
  }

  m_counters[TOT_READ]++;
  m_wsums[TOT_READ]+= 1.0;

  // Check if one of the particles in the event passes the generator settings.
  std::vector<BeamHaloParticle>::iterator itr = beamHaloEvent->begin();
  std::vector<BeamHaloParticle>::iterator itr_end = beamHaloEvent->end();
  bool passed = false;
  for(;itr!=itr_end;++itr) {
    beamHaloParticle = *itr;
    // Check the generator settings for this particle.
    if(m_beamHaloGeneratorSettings->checkParticle(&beamHaloParticle)) {
      passed = true;
    }
    else {
      if(m_debug) std::cout << "Debug: Particle fails generator settings cuts." << std::endl;
    }
  }

  m_counters[TOT_AFTER]++;
  m_wsums[TOT_AFTER]+= 1.0;
  m_counters[TOT_GEN]++;
  m_wsums[TOT_GEN]+= 1.0;

  // If all of the particles from this event fail read another event.
  // If there are no more events this function will exit with a
  // WARNING.
  if(!passed) {
    int ret_val;
    if((ret_val = readEvent(beamHaloEvent)) != 0) return ret_val;
  }

  return 0;
}
