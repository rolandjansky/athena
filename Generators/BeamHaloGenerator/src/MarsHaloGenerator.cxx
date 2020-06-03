/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/MarsHaloGenerator.h"

#include "BeamHaloGenerator/AsciiInput.h"
#include "BeamHaloGenerator/BeamHaloGeneratorAlg.h"
#include "BeamHaloGenerator/BeamHaloGeneratorSettings.h"
#include "BeamHaloGenerator/BeamHaloParticle.h"
#include "BeamHaloGenerator/BeamHaloParticleBuffer.h"
#include "BeamHaloGenerator/MarsParticle.h"

#include "CLHEP/Units/PhysicalConstants.h"

MarsHaloGenerator::MarsHaloGenerator(const HepPDT::ParticleDataTable* particleTable,
				     CLHEP::HepRandomEngine* engine, 
				     std::string inputFile,
				     std::vector<std::string> generatorSettings):
  BeamHaloGenerator(particleTable, engine, inputFile, generatorSettings) {
}

//------------------------------------------------------------------

MarsHaloGenerator::~MarsHaloGenerator() {
}

//------------------------------------------------------------------

int MarsHaloGenerator::genInitialize() {
  int ret_val;

  // Initialise base class
  if((ret_val = BeamHaloGenerator::genInitialize()) != 0) return ret_val;

  if(m_asciiInput->open()) {
    std::cout << "Error: Could not open ascii input file " << m_inputFile << std::endl;
    return 1;
  }

  // Check if sampling is required if sampling is required leave 
  // the ASCII file open for the event loop. 
  if(!m_enableSampling) return 0;

  m_beamHaloParticleBuffer = new BeamHaloParticleBuffer(m_bufferFileName, m_engine);
  if(m_beamHaloParticleBuffer->openForWriting()) {
    std::cout << "Error: Could not open binary buffer file " << m_bufferFileName << " for writing." << std::endl;
    return 1;
  }

  std::cout << "Info: Reading ascii input file " << m_inputFile << " into buffer" << std::endl;

  // Loop over all lines in the text file.
  BeamHaloParticle beamHaloParticle;
  bool eof = false;
  while(!eof) {
    ret_val = readParticle(&beamHaloParticle); 
    if(ret_val < 0) { eof = true; continue; }
    else if(ret_val > 0) { continue; }

    // Write the BeamHaloParticle into the binary buffer.
    if(m_beamHaloParticleBuffer->writeParticle(&beamHaloParticle)) {
      std::cout << "Warning: writeParticle failed." << std::endl;
    }
  } 

  m_asciiInput->close();
  m_beamHaloParticleBuffer->close();

  m_beamHaloParticleBuffer->openForReading();

  return 0;
}

//------------------------------------------------------------------

int MarsHaloGenerator::fillEvt(HepMC::GenEvent* evt) {
  std::vector<BeamHaloParticle> beamHaloEvent;
  int ret_val;

  // Read one MARS event which passes the generator settings.
  if((ret_val = readEvent(&beamHaloEvent)) != 0) return ret_val;

  // Convert the particles to GenParticles and attach them to the
  // event.  Flip the event if needed.
  if((ret_val = BeamHaloGenerator::convertEvent(&beamHaloEvent, evt)) != 0) return ret_val;

  // Set the event number
  evt->set_event_number(m_eventNumber);

  // Clear the values set by the base class convertEvent method
  evt->weights().clear();

  // Check if sampling has been enabled or not.
  if(!m_enableSampling) {
    HepMC::set_signal_process_id(evt,BeamHaloGenerator::MARS_READ);
    evt->weights().push_back(beamHaloEvent[0].weight());
  }
  else {
    HepMC::set_signal_process_id(evt,BeamHaloGenerator::MARS_SINGLE);
    evt->weights().push_back(1.0);
  }

  m_eventNumber++;

  return 0;
}

//------------------------------------------------------------------

int MarsHaloGenerator::genFinalize() {
  int ret_val;

  // Check if sampling has been enabled or not.
  if(!m_enableSampling) {
    m_asciiInput->close();
  }
  else {
    m_beamHaloParticleBuffer->close();
  }

  // Finalise base class
  if((ret_val = BeamHaloGenerator::genFinalize()) != 0) return ret_val;

  return 0;
}

//------------------------------------------------------------------

int MarsHaloGenerator::readParticle(BeamHaloParticle *beamHaloParticle) {
  std::vector<std::string> row;
  MarsParticle marsParticle;

  // Read one row of the ASCII file.
  row = m_asciiInput->readRow();
  if(row.size() == 0) return -1; // EOF

  // Fill a MarsParticle with values from the string vector.
  if(marsParticle.read(&row)) {
    return 1;  
  }

  // Fill the BeamHaloParticle with the data in the MarsParticle
  if(beamHaloParticle->fill(m_particleTable, &marsParticle)) {
    std::cout << "Warning: Conversion from MarsParticle to BeamHaloParticle failed." << std::endl;
    return 1;
  }

  // Increment the particles read from file information.
  m_counters[TOT_READ]++;
  m_wsums[TOT_READ]+= beamHaloParticle->weight();

  // Check the generator settings.  If this particle fails skip it.
  if(!m_beamHaloGeneratorSettings->checkParticle(beamHaloParticle)) {
    if(m_debug) std::cout << "Debug: Particle fails generator settings cuts." << std::endl;
    return 1;
  }

  // Increment the particles after cuts information.
  m_counters[TOT_AFTER]++;
  m_wsums[TOT_AFTER]+= beamHaloParticle->weight();

  return 0;
}

//------------------------------------------------------------------

int MarsHaloGenerator::readEvent(std::vector<BeamHaloParticle> *beamHaloEvent) {
  BeamHaloParticle *beamHaloParticle;
  int ret_val;

  // Clear the event
  beamHaloEvent->clear();

  // Check if sampling is required  
  if(!m_enableSampling) {
    beamHaloParticle = new BeamHaloParticle();
    bool eof = false, gotParticle = false; 
    while(!eof && !gotParticle) {
      ret_val = readParticle(beamHaloParticle);
      if(ret_val < 0) { eof = true; }
      else if(ret_val == 0) { gotParticle = true; } 
    }
  } 
  else {  
    // Read one particle at random from the binary buffer.  This uses
    // the particle weights to produce a flat distribution rather than a
    // weighted one, but may generate the same particle twice.
    beamHaloParticle = m_beamHaloParticleBuffer->readRandomParticle();
    if(!beamHaloParticle) {
      std::cout << "Error: readRandomParticle failed." << std::endl;
      return 1;
    }
  }

  // Increment generated particles information.
  m_counters[TOT_GEN]++;
  m_wsums[TOT_GEN]+= beamHaloParticle->weight();


  // Copy the BeamHaloParticle into this event
  beamHaloEvent->push_back(*beamHaloParticle);

  // Delete the pointer to the generated particle.
  delete beamHaloParticle;

  return 0;
}

//------------------------------------------------------------------
