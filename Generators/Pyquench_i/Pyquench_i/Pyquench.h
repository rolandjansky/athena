/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorModules/Pythia.h
// Description:
//    This code is used to get a PYTHIA Monte Carlo event.
//    genInitialize() is used to read parameters
//    callGenerator() makes the event
//    genFinalize() writes log files etc
//    fillEvt(GeneratorEvent* evt) passes the event to HepMC
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
//
// AuthorList:
//         Brian Cole, Nathan Grau 2006
//         Modelled after the Pythia_i interface code by Ian Hinchclife
//
//
//

#ifndef GENERATORMODULESPYQUENCH_H
#define GENERATORMODULESPYQUENCH_H
#include "GeneratorModules/GenModule.h"
#include "Pythia_i/Pydat1.h"
#include "Pythia_i/Pydat2.h"
#include "Pythia_i/Pydat3.h"
#include "Pythia_i/Pysubs.h"
#include "Pythia_i/Pypars.h"
#include "Pythia_i/Pydatr.h"
#include "Pythia_i/Pyint1.h"
#include "Pythia_i/Pyint2.h"
#include "Pythia_i/Pyint5.h"

#include "Pyquench_i/Pyqpar.h"

#include "Tauola_i/Atlas_HEPEVT.h"
#include <vector>
#include <string>

class IAtRndmGenSvc;

// Pyquench is a concrete instance of a generator (interface) module "GenModule"
//
class Pyquench : public GenModule 
{
public:
  Pyquench(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Pyquench(); 

  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

  // Accessors to PYTHIA COMMONS
  // reference to Pydat1, Pysubs and Pypars 
  //
  Pydat1& pydat1() { return m_pydat1; }  
  Pydat2& pydat2() { return m_pydat2; }
  Pydat3& pydat3() { return m_pydat3; }
  Pysubs& pysubs() { return m_pysubs; }
  Pypars& pypars() { return m_pypars; }
  Pydatr& pydatr() { return m_pydatr; }
  Pyint1& pyint1() { return m_pyint1; }
  Pyint2& pyint2() { return m_pyint2; }
  Pyint5& pyint5() { return m_pyint5; }

  Pyqpar& pyqpar() {return m_pyqpar;};
  // Pointer On AtRndmGenSvc
  //
  static IAtRndmGenSvc* m_AtRndmGenSvc_p;
  static std::string	m_pythia_stream;

protected:

  void setPythiaParameters();

  //  User configurable parametera
  //
  int    m_nuclearA;  // A of the nuclke
  bool   m_minBias;
  double m_bimpact;
  double m_rootsNN;
  int    m_randomseed;
  int m_tune_index;
  // event counter
  int m_events ;
  std::vector<std::string>  m_pythiaCommandVector;
  //  Vector for storing seeds
  //
  std::vector<long int> m_seeds;

  // members that provide access to Pythia common blocks
  //
  Pysubs m_pysubs;

  // Pydat1 data, methods 
  Pydat1 m_pydat1;  
  // Pypars data, methods 
  Pypars m_pypars;
  // Pydat2 data, methods 
  Pydat2 m_pydat2;
  // Pydat3 data, methods 
  Pydat3 m_pydat3;  
  // Pydatr access for random numbers (only set the 1st seed just now
  Pydatr m_pydatr;
  // Pyint1 access for data
  Pyint1 m_pyint1;
  // Pyint2 access for data
  Pyint2 m_pyint2;	
  // Pyint5 access for data
  Pyint5 m_pyint5;	

  Pyqpar m_pyqpar;
  // Method to Manage the random seed IO from/to a file
  void	RandomNumberIO(void);

  // I/O to HEPEVT
  void store_Atlas_HEPEVT(void);

  static Atlas_HEPEVT* s_atlas_HEPEVT;
};


#endif










