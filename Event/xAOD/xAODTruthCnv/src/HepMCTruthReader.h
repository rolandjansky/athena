/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPMCTRUTHREADER_H
#define HEPMCTRUTHREADER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"
#include "AtlasHepMC/GenParticle_fwd.h"


/// @short Algorithm demonstrating reading of HepMC truth, and printing to screen
/// @author James Catmore <James.Catmore@cern.ch>
class HepMCTruthReader : public AthAlgorithm {
public:

  /// Regular algorithm constructor
  HepMCTruthReader(const std::string& name, ISvcLocator* svcLoc);

  /// Function initialising the algorithm
  virtual StatusCode initialize();

  /// Function executing the algorithm
  virtual StatusCode execute();


private:

  /// The keys for the input xAOD truth containers
  std::string m_hepMCContainerName;

  void printEvent(const HepMC::GenEvent*);
  void printVertex(HepMC::ConstGenVertexPtr);
  void printParticle(HepMC::ConstGenParticlePtr);

}; // class HepMCTruthReader



#endif // HEPMCTRUTHREADER_H
