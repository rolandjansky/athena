/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHALG_H
#define TRACKPARTICLETRUTHALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"


class IMCTruthClassifier;

class TrackParticleTruthAlg: public AthAlgorithm {
public:
  TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_particlesLinkKey;
  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_particlesTypeKey;
  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_particlesOriginKey;
  std::string m_particleName; /// TrackParticle input name
  SG::ReadHandleKey<xAODTruthParticleLinkVector>  m_truthParticleLinkVecKey;//std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth
  SG::ReadHandleKey<TrackTruthCollection> m_truthTracksKey;//std::string m_truthName;          /// Track(Particle)TruthCollection input name
  
  
  ToolHandle<IMCTruthClassifier> m_truthClassifier;
};

#endif/*TRACKTRUTHSELECTOR_H*/
