/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESPYTHIACHL_H
#define GENERATORMODULESPYTHIACHL_H

#include "Pythia_i/Pythia.h"

#include "GeneratorModules/GenModule.h"
#include "Tauola_i/Atlas_HEPEVT.h"

class IAtRndmGenSvc;

/// @todo Put inside class (this is a global declaration)
typedef std::vector<std::string> CommandVector;

/**
@brief This code is used to get a PYTHIA-ChL Monte Carlo event.

    genInitialize() is used to read parameters

    callGenerator() makes the event

    genFinalize() writes log files etc

    fillEvt(GeneratorEvent* evt) passes the event to HepMC

    The output will be stored in the transient event store so it can be
    passed to the simulation.

@author 

    Ian Hinchliffe June 2000

    Modelled after the CDF code by Stan Thompson

    Added access to pyint2 common Feb 2001

    Added access to pyint5 common Feb 2002

    Added access to lesatlas common (not part of pythia but needed for external process May 2002

    Added access to pyint1 common Sept 2002
    
    Spited from Pythia_i by Jan Kotanski July 2011

*/

class PythiaChL : public Pythia {
public:
  
  PythiaChL(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PythiaChL();

  virtual StatusCode genuserInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

};


#endif

/*  LocalWords:  ifndef
 */
