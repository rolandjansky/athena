/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERTAU_H
#define GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERTAU_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

#include <unordered_set>


/// @brief Algorithm to skim the xAOD truth particle container for tau filter 
///
/// This algorithm is used to copy and skim the particles from the xAOD TruthParticles container,
/// keeping just relevant taus from the event.  
/// The design of this class heavily mirrors the DerivationFramework::TruthCollectionMaker.
///
/// @author Jeff Dandoy <Jeff.Dandoy@cern.ch>
class xAODTruthParticleSlimmerTau : public AthAlgorithm {
public:

  /// Regular algorithm constructor
  xAODTruthParticleSlimmerTau( const std::string& name, ISvcLocator* svcLoc );
//  xAODTruthParticleSlimmerTau( const std::string& t, const std::string& n, const IInterface* p);
  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();

  CLHEP::HepLorentzVector sumDaughterNeutrinos( const xAOD::TruthParticle* tau );

private:

  /// The key for the output xAOD truth containers
  std::string m_xaodTruthTauParticleContainerName;
  std::string m_xaodTruthParticleContainerName;

  /// Selection values for keeping taus and leptons
  double m_tau_pt_selection; //in GeV
  double m_abseta_selection;

  /// a flag to force rerunning (useful for rerunning on ESDs)
  bool m_forceRerun;

  ToolHandle<IMCTruthClassifier> m_classifier;

}; // class xAODTruthParticleSlimmerTau



#endif // GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERTAU_H
