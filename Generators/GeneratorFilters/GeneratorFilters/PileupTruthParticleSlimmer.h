/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_PILEUPTRUTHPARTICLESLIMMER_H
#define GENERATORFILTERS_PILEUPTRUTHPARTICLESLIMMER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "StoreGate/StoreGateSvc.h"

#include <unordered_set>

/// @brief Algorithm to skim the xAOD truth particle container for pileup minbias samples
///
/// This algorithm is used to copy and skim the the particles from the xAOD TruthParticles container,
/// keeping just relevant photons, electrons, and muons from the event.  These particles are made 
/// available in the output EVNT file so they can later be combined with other pileup overlay events
/// into a pileup TruthParticles container.
/// The design of this class heavily mirrors the DerivationFramework::TruthCollectionMaker.
///
/// @author Jeff Dandoy <Jeff.Dandoy@cern.ch>
class PileupTruthParticleSlimmer : public AthAlgorithm {
public:

  /// Regular algorithm constructor
  PileupTruthParticleSlimmer( const std::string& name, ISvcLocator* svcLoc );

  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();


private:

  /// The key for the output xAOD truth containers
  std::string m_xaodTruthPileupParticleContainerName;
  std::string m_xaodTruthParticleContainerName;

  /// Selection values for keeping photons and leptons
  double m_photon_pt_selection; //in GeV
  double m_lepton_pt_selection; //in GeV
  double m_abseta_selection;

  /// a flag to force rerunning (useful for rerunning on ESDs)
  bool m_forceRerun;

}; // class PileupTruthParticleSlimmer



#endif // GENERATORFILTERS_PILEUPTRUTHPARTICLESLIMMER_H
