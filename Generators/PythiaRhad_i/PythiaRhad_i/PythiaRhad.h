/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESPYTHIARHAD_H
#define GENERATORMODULESPYTHIARHAD_H

#include "Pythia_i/Pythia.h"

#include "GeneratorModules/GenModule.h"
#include "Tauola_i/Atlas_HEPEVT.h"

/// @todo Move into class: currently this is GLOBAL
using std::string;
typedef std::vector<std::string> PygiveCommandVector;
typedef std::vector<std::string> CommandVector;

class IAtRndmGenSvc;

/**
@brief This code is used to get a PYTHIA Monte Carlo event.

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

    Ewelina Lobodzinska (Jan. 2008) doxygen doc added

    Spited from Pythia_i Jan Kotanski July 2011
*/
class PythiaRhad : public Pythia {
public:

  PythiaRhad(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PythiaRhad();

  virtual StatusCode genuserInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);


 protected:

 private:

  bool m_run_gluino_hadrons;
  bool m_intermediate;
  bool m_regge;
  bool m_run_stop_hadrons;
  bool m_run_sbottom_hadrons;
  double m_gluino_ball_prob;
  bool m_enable_after_initialize;

  /// @name For StoppedGluino simulation ...

  double m_xshift, m_yshift, m_zshift, m_tshift, m_randomtshift;
  bool m_translate_rh_to_stopping_position;
  bool m_rotate_rh;
  bool m_boost_rh_to_rest_frame;
  bool m_strip_out_rh;
  bool m_rh_decay;
  std::vector<std::vector<double> > m_StoppingInput;
  int m_doubleid;
  std::vector<int> m_rhs;

  void strip_out_rh(HepMC::GenEvent* evt);


};


#endif
