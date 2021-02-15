/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESQGSJET_H
#define GENERATORMODULESQGSJET_H

#include "GeneratorModules/GenModule.h"
#include "AtlasHepMC/HEPEVT_Wrapper.h"

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

class QGSJet: public GenModule {
public:
  QGSJet(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~QGSJet();
  
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

protected:
    // event counter
  int m_events;
  int m_ievent; //event counter in Epos
  int m_iout; //output type

    // setable properties
  double      m_beamMomentum;
  double      m_targetMomentum;
  int         m_model;
  int         m_primaryParticle;
  int         m_targetParticle;
  std::string m_paramFile;
  std::string m_lheout;
  int         m_itab;
  int         m_ilheout;
  int         m_nEvents;

  static const size_t kMaxParticles = HEPEVT_EntriesAllocation; 
  int   m_partID[ kMaxParticles ];
  double m_partPx[ kMaxParticles ];
  double m_partPy[ kMaxParticles ];
  double m_partPz[ kMaxParticles ];
  double m_partEnergy[ kMaxParticles ];
  double m_partMass[ kMaxParticles ];
  int   m_partStat[ kMaxParticles ];

  std::vector<long int> m_seeds;
};

#endif

