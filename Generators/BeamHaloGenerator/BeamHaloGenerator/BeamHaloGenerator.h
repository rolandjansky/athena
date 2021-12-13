/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMHALOGENERATOR_H
#define BEAMHALOGENERATOR_H

#include "BeamHaloGenerator/BeamHaloParticle.h"
#include <string>
#include <vector>
#include "AtlasHepMC/GenEvent.h"

namespace HepPDT {
  class ParticleDataTable;
}
namespace CLHEP {
  class HepRandomEngine;
}
class BeamHaloGeneratorAlg;
class AsciiInput;
class BeamHaloGeneratorSettings;
class BeamHaloParticleBuffer;

/** @class BeamHaloGenerator

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief An abstract base class to provide generic beam halo
    generator functionality.
*/
class BeamHaloGenerator {
 public:

  BeamHaloGenerator(const HepPDT::ParticleDataTable* particleTable,
		    CLHEP::HepRandomEngine* engine, 
		    const std::string& inputFile,
		    const std::vector<std::string>& generatorSettings);

  virtual ~BeamHaloGenerator();

  /** A function to initialise the generator. */
  virtual int genInitialize();

  /** A function to finalise the generator. */
  virtual int genFinalize();

  /** A function to create one event in HepMC format. */
  virtual int fillEvt(HepMC::GenEvent* evt) = 0;
  
 protected:

  /** A function to read one event in a simplified format. */
  virtual int readEvent(std::vector<BeamHaloParticle> *beamHaloEvent) = 0;

  /** A function to read one particle from the input ASCII file. */
  virtual int readParticle(BeamHaloParticle *beamHaloParticle) = 0;

 public:


  /** Set the position of the interface plane in mm. */
  void setInterfacePlane(double interfacePlane) { m_interfacePlane = interfacePlane; }

  /** Turn on or off the event flipping code. */
  void setEnableFlip(bool enableFlip) { m_enableFlip = enableFlip; }

  /** Set probability of flipping an event about Z=0. */
  void setFlipProbability(float flipProbability) { m_flipProbability = flipProbability; }

  /** Turn on or off sampling from the input ASCII file. */
  void setEnableSampling(bool enableSampling) { m_enableSampling = enableSampling; }

  /** Set the name of the binary buffer file, needed for sampling from a
      converted file. */
  void setBufferFileName(const std::string& bufferFileName) { m_bufferFileName = bufferFileName; }

  /** A function to turn on or off debug print out. */
  void setDebugEnable(bool debug) { m_debug = debug; }

 protected:

  /** An enum of generator process identifiers.  _READ corresponds to
      no sampling and therefore simple conversion from input file to
      HepMC record. */
  enum enumGeneratorProcessIds {
    MARS_READ,
    MARS_SINGLE,
    MARS_SHOWER,
    FLUKA_READ,
    FLUKA_SINGLE,
    FLUKA_SHOWER };


  /** A member function to check if the event should be flipped */
  bool flipEvent();

  /** A member function to convert a vector of beam halo particles
      into a GenEvent */
  int convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent,
		   HepMC::GenEvent* evt);

  /** A pointer to the particle data table. */
  const HepPDT::ParticleDataTable* m_particleTable;

  /** Random number engine */
  CLHEP::HepRandomEngine *m_engine;
  
  /** Input file name */
  std::string m_inputFile;

  /** The position of the interface plane in mm. */
  double m_interfacePlane;
  
   /** Flag for flipping event */
  bool m_enableFlip;

  /** Flip probability */
  float m_flipProbability;
  
  /** Flag to enable or disable sampling. */
  bool m_enableSampling;

  /** The name of the binary buffer file, needed for sampling from a
      converted file. */
  std::string m_bufferFileName;

  /** Binary particle buffer for caching converted events. */
  BeamHaloParticleBuffer *m_beamHaloParticleBuffer;

  /** A pointer to an AsciiInput object, used to read data from the
      Ascii input file. */
  AsciiInput *m_asciiInput;

  /** A pointer to a BeamHaloGeneratorSettings object used to filter
      particles. */
  BeamHaloGeneratorSettings *m_beamHaloGeneratorSettings;

  /** A data member to count the event number. */
  long m_eventNumber;

  /** An enum of particle counter indices. */
  enum enumCounters {
    TOT_READ, 
    TOT_AFTER,
    TOT_GEN,
    NUM_COUNTERS};

  /** Particle or event counters of dimension enumCounters. */
  long m_counters[NUM_COUNTERS];

  /** Sum of weights of particles or events of dimension enumCounters. */
  double m_wsums[NUM_COUNTERS];

  /** A flat to turn on or off debug print out. */
  bool m_debug;

 private:

  /** A vector of generator settings in string command form. */
  std::vector<std::string> m_generatorSettings;

};

#endif

