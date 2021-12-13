/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MARSHALOGENERATOR_H
#define MARSHALOGENERATOR_H

#include "BeamHaloGenerator/BeamHaloGenerator.h"


/** @class MarsHaloGenerator

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to provide conversion from a MARS format ASCII
    input record into HepMC format, with or without sampling.
*/
class MarsHaloGenerator: public BeamHaloGenerator {
 public:

  MarsHaloGenerator(const HepPDT::ParticleDataTable* particleTable,
		    CLHEP::HepRandomEngine* engine, 
		    const std::string& inputFile,
		    const std::vector<std::string>& generatorSettings);

  virtual ~MarsHaloGenerator();

  /** A function to initialise the generator. */
  virtual int genInitialize();

  /** A function to finalise the generator. */
  virtual int genFinalize();

  /** A function to create one event in HepMC format. */
  virtual int fillEvt(HepMC::GenEvent* evt);
  
 protected:

  /** A function to read one event in a simplified format. */
  virtual int readEvent(std::vector<BeamHaloParticle> *beamHaloEvent);

  /** A function to read one particle from the input ASCII file. */
  virtual int readParticle(BeamHaloParticle *beamHaloParticle);
};

#endif
