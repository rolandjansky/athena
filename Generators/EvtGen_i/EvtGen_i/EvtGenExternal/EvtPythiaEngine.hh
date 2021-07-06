#ifdef EVTGEN_PYTHIA
//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2011      University of Warwick, UK
//
// Module: EvtPythiaEngine
//
// Description: Interface to the Pytha 8 external generator
//
// Modification history:
//
//    John Back       April 2011            Module created
//
//------------------------------------------------------------------------

#ifndef EVT_I_PYTHIAENGINE_HH
#define EVT_I_PYTHIAENGINE_HH

#include "EvtGenModels/EvtAbsExternalGen.hh"
#include "EvtGen_i/EvtGenExternal/EvtPythiaRandom.hh"

#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"

#include "Pythia8/Pythia.h"
#include "Pythia8/ParticleData.h"
//#include "Pythia.h"
//#include "ParticleData.h"

#include <string>
#include <vector>
#include <map>

class EvtPythiaEngine : public EvtAbsExternalGen {

public:

  EvtPythiaEngine(std::string xmlDir = "./xmldoc", 
		  bool convertPhysCodes = false,
		  bool useEvtGenRandom = true);
  virtual ~EvtPythiaEngine();

  virtual bool doDecay(EvtParticle* theMother);

  virtual void initialise();

protected:

private:

  void updateParticleLists();
  void updatePhysicsParameters();

  void createPythiaParticle(EvtId& particleId, int PDGCode);
  void updatePythiaDecayTable(EvtId& particleId, int aliasInt, int PDGCode);
  void storeDaughterInfo(EvtParticle* theParticle, int startInt);

  void clearDaughterVectors();
  void clearPythiaModeMap();

  void createDaughterEvtParticles(EvtParticle* theParent);

  int getModeInt(EvtDecayBase* decayModel);

  Pythia8::Pythia* m_genericPythiaGen;
  Pythia8::Pythia* m_aliasPythiaGen;
  Pythia8::Pythia* m_thePythiaGenerator;

  Pythia8::ParticleData m_genericPartData, m_aliasPartData;  
  Pythia8::ParticleData m_theParticleData;

  std::vector<int> m_daugPDGVector;
  std::vector<EvtVector4R> m_daugP4Vector;

  typedef std::map<int, std::vector<int> > PythiaModeMap;
  PythiaModeMap m_pythiaModeMap;

  bool m_convertPhysCodes, m_initialised, m_useEvtGenRandom;

  EvtPythiaRandom* m_evtgenRandom;

  std::map<int, int> m_addedPDGCodes;

};

#endif

#endif
