#ifndef GENERATOR_PYTHIA8_H
#define GENERATOR_PYTHIA8_H

#include "GeneratorModules/GenModule.h"

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

// calls to fortran routines
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <stdexcept>


/**
 *  Author: James Monk (jmonk@cern.ch)
*/

using std::string;

class IAtRndmGenSvc;

namespace Pythia8{
  class Sigma2Process;
}


class customRndm : public Pythia8::RndmEngine {
public:
  
  // Constructor.
  customRndm():
    m_engine(0), m_RndmGenSvc(0) {}
  
  // Routine for generating a random number.
  inline double flat(){    
    m_engine=m_RndmGenSvc->GetEngine(m_stream);
    return CLHEP::RandFlat::shoot(m_engine);
  };
  
  // Initialisation Routine 
  inline void init(IAtRndmGenSvc &RndmGenSvc, std::string RndmStream){m_RndmGenSvc=&RndmGenSvc; m_stream=RndmStream;};
  std::string m_stream; 
  
private:
  CLHEP::HepRandomEngine* m_engine;
  IAtRndmGenSvc* m_RndmGenSvc;
};


//namespace Generator{
class Pythia8_i: public GenModule{

public:
  Pythia8_i(const string &name, ISvcLocator *pSvcLocator);
  
  ~Pythia8_i();

  class CommandException : public std::runtime_error{
  public:
    
  CommandException(const string &cmd): std::runtime_error("Cannot interpret command: " + cmd){
    }
  };
    
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode fillEvt(HepMC::GenEvent *evt);
  virtual StatusCode genFinalize();

  double pythiaVersion()const;
  
  static std::string    pythia_stream;
    
protected:
  
  // make these protected so that Pythia8B can access them
  Pythia8::Pythia m_pythia;
  HepMC::Pythia8ToHepMC m_pythiaToHepMC;

private:
  
  static std::string xmlpath();
  
  // Add the pythia.process, which is the LHE record for external ME events, to the
  // HepMC record
  void addLHEToHepMC(HepMC::GenEvent *evt);
  
  int m_internal_event_number;
  
  double m_version;
  
  std::vector<std::string> m_commands;
  std::vector<std::string> m_userParams;
  std::vector<std::string> m_userModes;
  
  enum PDGID {PROTON=2212, ANTIPROTON=-2212, ELECTRON=11, POSITRON=-11, INVALID=0};
  
  double m_collisionEnergy;
  bool m_useRndmGenSvc;
  customRndm *m_atlasRndmEngine; 

  
  std::string m_beam1;
  std::string m_beam2;

  std::string m_lheFile;
  
  bool m_doCKKWLAcceptance;
  bool m_doFxFxXS;
  double m_nAccepted;
  double m_nMerged;
  double m_sigmaTotal;
  
  unsigned int m_maxFailures;
  unsigned int m_failureCount;
  
  std::map<std::string, PDGID> m_particleIDs;

  std::vector<long int> m_seeds;

  std::string m_userProcess;

  // ptr to possible user process
  Pythia8::Sigma2Process *m_procPtr;
  
  std::vector<std::string> m_userHooks;
  
  Pythia8::UserHooks *m_userHookPtr;
  
  std::string m_userResonances;
  
  std::vector<Pythia8::ResonanceWidths*> m_userResonancePtrs;
  
  bool m_useLHAPDF;

  std::string m_particleDataFile;
  std::string m_outputParticleDataFile;
  
  std::vector<string> m_weightIDs;
  bool m_doLHE3Weights;
  
};

#endif
