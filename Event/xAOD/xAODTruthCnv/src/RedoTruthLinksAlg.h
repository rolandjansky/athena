/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCREATORALGS_REDOTRUTHLINKSALG_H
#define XAODCREATORALGS_REDOTRUTHLINKSALG_H

#include "AthenaBaseComps/AthAlgorithm.h"


#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "xAODTruth/TruthEventContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace xAODMaker {

  /// @brief Algorithm creating xAOD truth from HepMC
  ///
  /// This algorithm can be used to translate the HepMC coming from an AOD, and
  /// create xAOD truth objects out of them for an output xAOD.
  ///
  /// @author James Catmore <James.Catmore@cern.ch>
  /// @author Jovan Mitreski <Jovan.Mitreski@cern.ch>
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  class RedoTruthLinksAlg : public AthAlgorithm {
  public:

    /// Regular algorithm constructor
    RedoTruthLinksAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute();


  private:

    /// The key for the output xAOD truth containers
    SG::WriteHandleKey<xAODTruthParticleLinkVector> m_truthLinkContainerKey{
      this, "TruthLinks", "xAODTruthLinks", "Output xAODTruthLinks container"};

    // The key for the input xAOD truth containers
    SG::ReadHandleKey<xAOD::TruthEventContainer> m_linksOnlyTruthEventContainerKey{
      this, "linksOnlyTruthEventContainerName", "TruthEvents", "Input TruthEvents container"};

  }; // class RedoTruthLinksAlg


} // namespace xAODMaker

#endif // XAODCREATORALGS_REDOTRUTHLINKSALG_H
