/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESEPOS_H
#define GENERATORMODULESEPOS_H

#include "GeneratorModules/GenModule.h"

/**
@class Epos
@brief This code is used to get an Epos Monte Carlo event.

  genInitialize() is used to read parameters
  callGenerator() makes the event
  genFinalize() writes log files etc
  fillEvt(GeneratorEvent* evt) passes the event to HepMC

  The output will be stored in the transient event store so it can be
  passed to the simulation.

@author Sami Kama
  Adapted from Ian Hinchliffe's code
*/

class Epos: public GenModule {
public:
  Epos(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Epos();
  
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(GenEvent* evt);

protected:
    // event counter
  int m_events;

    // setable properties
  double      m_beamMomentum;
  double      m_targetMomentum;
  int         m_model;
  int         m_primaryParticle;
  int         m_targetParticle;
  std::string m_paramFile;

    // max number of particles MUST BE EQUAL TO THE NUMBER SET IN crmc-aaa.f
  static const size_t kMaxParticles = 9990; 
  int   m_partID[ kMaxParticles ];
  float m_partPx[ kMaxParticles ];
  float m_partPy[ kMaxParticles ];
  float m_partPz[ kMaxParticles ];
  float m_partEnergy[ kMaxParticles ];
  float m_partMass[ kMaxParticles ];

  std::vector<long int> m_seeds;
};

#endif

