/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/BeamHaloGenerator.h"
#include "BeamHaloGenerator/BeamHaloParticleBuffer.h"
#include "BeamHaloGenerator/BeamHaloGeneratorAlg.h"
#include "BeamHaloGenerator/AsciiInput.h"
#include "BeamHaloGenerator/BeamHaloGeneratorSettings.h"
#include "HepMC/GenEvent.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TMath.h"
#include <cmath>

BeamHaloGenerator::BeamHaloGenerator(const HepPDT::ParticleDataTable* particleTable,
				     CLHEP::HepRandomEngine* engine, 
				     std::string inputFile,
				     std::vector<std::string> generatorSettings):
  p_particleTable(particleTable),
  p_engine(engine),
  p_inputFile(inputFile),
  p_interfacePlane(0.),
  p_enableFlip(false),
  p_flipProbability(0.),
  p_enableSampling(false),
  p_bufferFileName("buffer.bin"),
  p_beamHaloParticleBuffer(0),
  p_asciiInput(0),
  p_beamHaloGeneratorSettings(0),
  p_eventNumber(0),
  p_debug(false),
  m_generatorSettings(generatorSettings) {
  for(int i=0;i<NUM_COUNTERS;i++) {
    p_counters[i] = 0;
    p_wsums[i] = 0.;
  }
}

//------------------------------------------------------------------

BeamHaloGenerator::~BeamHaloGenerator() {
  if(p_asciiInput) delete p_asciiInput;
  if(p_beamHaloGeneratorSettings) delete p_beamHaloGeneratorSettings;
  if(p_beamHaloParticleBuffer) delete p_beamHaloParticleBuffer;
}

//------------------------------------------------------------------

int BeamHaloGenerator::genInitialize() {
  p_asciiInput = new AsciiInput(p_inputFile);
  p_beamHaloGeneratorSettings = new BeamHaloGeneratorSettings(m_generatorSettings);

  return 0;
}

//------------------------------------------------------------------

int BeamHaloGenerator::genFinalize() {

  std::cout << "=================================================================" << std::endl;
  std::cout << "|                                 | Total Number | Total Weight |" << std::endl;
  std::cout << "|---------------------------------------------------------------|" << std::endl;
  std::cout << "| Particles read from input file  | ";
  std::cout << std::setw(12) << p_counters[TOT_READ] << " | ";
  std::cout << std::setw(12) << std::setprecision(8) << p_wsums[TOT_READ] << " |" << std::endl;
  std::cout << "| Particles after cuts            | ";
  std::cout << std::setw(12) << p_counters[TOT_AFTER] << " | ";
  std::cout << std::setw(12) << std::setprecision(8) << p_wsums[TOT_AFTER] << " |" << std::endl;
  std::cout << "| Particles generated             | ";
  std::cout << std::setw(12) << p_counters[TOT_GEN] << " | ";
  std::cout << std::setw(12) << std::setprecision(8) << p_wsums[TOT_GEN] << " |" << std::endl;
  std::cout << "=================================================================" << std::endl;
  
  std::cout << std::setprecision(6);

  return 0;
}

//------------------------------------------------------------------

bool BeamHaloGenerator::flipEvent() {
  if(!p_enableFlip) return false;

  // Check to see if the event should be flipped or not
  if(CLHEP::RandFlat::shoot(p_engine) <= p_flipProbability) {
    if(p_debug) std::cout << "Debug: Flipping event" << std::endl;
    return true;
  }

  return false;
}

//------------------------------------------------------------------

int BeamHaloGenerator::convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent,
				    HepMC::GenEvent* evt) {
  double pz;
  bool flipFlag = flipEvent();
  HepMC::GenParticle* genParticle;
  HepMC::GenVertex* genVertex;
  const double c = 2.99792458E+11; // speed of light in mm/s

  std::vector<BeamHaloParticle>::iterator itr = beamHaloEvent->begin();
  std::vector<BeamHaloParticle>::iterator itr_end = beamHaloEvent->end();

  if(itr != itr_end) {
    // Take the primary information from the first particle in the event.  If the event is more than one
    // particle, the particles come from the same primary proton.
    evt->weights().push_back((*itr).weight()); // event weight
    HepMC::FourVector primaryPosition = (*itr).positionAtPrimary();
    evt->weights().push_back(primaryPosition.x()); // Starting position of primary particle: x position (cm)
    evt->weights().push_back(primaryPosition.y()); // Starting position of primary particle: y position (cm)
    evt->weights().push_back(primaryPosition.z()); // Starting position of primary particle: z position (cm)
    evt->weights().push_back(primaryPosition.t()); // Starting position of primary particle: t (s)
  } 

  // Append each particle to the GenEvent
  for(;itr!=itr_end;++itr) {
    HepMC::FourVector position = (*itr).positionAtScoringPlane();
    HepMC::FourVector fourVector = (*itr).fourVector();

    if(!flipFlag) {
      genParticle = new HepMC::GenParticle(fourVector,
					   (*itr).pdgId(),
					   1);

      genVertex = new HepMC::GenVertex(HepMC::FourVector(position.x(),
							 position.y(),
							 (position.z() + p_interfacePlane),
							 c*position.t() -1.0*std::fabs(p_interfacePlane)),
				       1);
    }
    else {
      pz = fourVector.pz();
      fourVector.setPz(-pz);

      genParticle = new HepMC::GenParticle(fourVector,
					   (*itr).pdgId(),
					   1);

      genVertex = new HepMC::GenVertex(HepMC::FourVector(position.x(),
							 position.y(),
							 -1.0*(position.z() + p_interfacePlane),
							 c*position.t() -1.0*std::fabs(p_interfacePlane)),
				       1);
    }

    genVertex->add_particle_out(genParticle);
    evt->add_vertex(genVertex);
  }

  return 0;
}

//------------------------------------------------------------------
