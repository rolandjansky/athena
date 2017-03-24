/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHALG_H
#define TRACKPARTICLETRUTHALG_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMCTruthClassifier;

class TrackParticleTruthAlg: public AthAlgorithm {
public:
  TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  
  std::string m_truthName;          /// Track(Particle)TruthCollection input name
  std::string m_trackParticleName;  /// TrackParticle input name
  std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth
  
  ToolHandle<IMCTruthClassifier> m_truthClassifier;
};

#endif/*TRACKTRUTHSELECTOR_H*/
