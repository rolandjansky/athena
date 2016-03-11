/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYTHIAEXO_I_PYTHIALLP_H
#define PYTHIAEXO_I_PYTHIALLP_H

#include "GeneratorModules/GenModule.h"
#include "PythiaExo_i/Pydat1.h"
#include "PythiaExo_i/Pydat2.h"
#include "PythiaExo_i/Pydat3.h"
#include "PythiaExo_i/Pysubs.h"
#include "PythiaExo_i/Pypars.h"
#include "PythiaExo_i/Pydatr.h"
#include "PythiaExo_i/Pymssm.h"
#include "PythiaExo_i/Pyint1.h"
#include "PythiaExo_i/Pyint2.h"
#include "PythiaExo_i/Pyint5.h"
#include "PythiaExo_i/Pytcsm.h"
#include "AcerMC_i/AcerMC_acset.h"
#include "Tauola_i/Atlas_HEPEVT.h"

using std::string; // @todo ...and then it _isn't_ used without std::!
typedef std::vector<std::string> CommandVector;

class IAtRndmGenSvc;


class PythiaLLP : public GenModule {
public:
  PythiaLLP(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PythiaLLP();
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

  // Accessors to PYTHIA COMMONS
  // reference to Pydat1, Pysubs and Pypars
  Pydat1& pydat1();
  Pydat2& pydat2();
  Pydat3& pydat3();
  Pysubs& pysubs();
  Pypars& pypars();
  Pydatr& pydatr();
  Pymssm& pymssm();
  Pyint1& pyint1();
  Pyint2& pyint2();
  Pyint5& pyint5();
  Pytcsm& pytcsm();
  // accessor for external processes
  AcerMC_acset& acermc_acset();

  static IAtRndmGenSvc* 	p_AtRndmGenSvc;
  static std::string	pythia_stream;

protected:

  // inputs to pyinit
  std::string m_frame;
  std::string m_external; //to specify the specific process when m_frame is set to user
  std::string m_beam;
  std::string m_target;
  double  m_win;

  // various input parameters
  int  m_msel;
  int  m_firstlistevent;
  int  m_lastlistevent;
  int  m_eventlistlevel;
  std::vector<int>  m_pystatlistlevel;
  int  m_initlistlevel;
  int  m_randomseed;
  std::string  m_envval1;
  int  m_fortout;

  std::string  m_RndmFileName;
  int	m_RndmFileNumber;
  int	m_RndmSwitch;
  int	m_RndmFirstEvent;
  int	m_RndmSkipEvents;
  int	m_RndmMOVE;
  int	m_RndmFileLength;

  std::vector<long int> m_seeds;

  // event counter
  int m_events ;
  Pysubs m_pysubs;
  // Decay table handling
  //  AbsParmEnum _decayFileMode;
  // AbsParmFilename _decayFile;
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
  // SUSY access
  Pymssm m_pymssm;
  // PYTCSM access
  Pytcsm m_pytcsm;
  // external proces
  int m_ExternalProcess;
  AcerMC_acset m_acermc_acset;

  // Private helper method to initialize talk-to
  //	void _initializeTalkTo();

  // Commands to setup pythia
  CommandVector m_pythiaCommandVector;

  // Control to set the ATLAS defaults
  bool m_AtlasDefaults;

  // The susy file name
  std::string m_read_Filesusy;

  //
  std::string m_LLP_PYUPDA_file;

  // Method to Manage the random seed IO from/to a file
  void	RandomNumberIO(void);

  // I/O to HEPEVT
  void store_Atlas_HEPEVT(void);

  static Atlas_HEPEVT* s_atlas_HEPEVT;
};


#endif
