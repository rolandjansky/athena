/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCREATORALGS_XAODTRUTHREADER_H
#define XAODCREATORALGS_XAODTRUTHREADER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEvent.h"

namespace xAODReader {


  /// @short Algorithm demonstrating reading of xAOD truth, and printing to screen
  /// @author James Catmore <James.Catmore@cern.ch>
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  class xAODTruthReader : public AthAlgorithm {
  public:

    /// Regular algorithm constructor
    xAODTruthReader(const std::string& name, ISvcLocator* svcLoc);

    /// Function initialising the algorithm
    virtual StatusCode initialize();

    /// Function executing the algorithm
    virtual StatusCode execute();


  private:

    /// The keys for the input xAOD truth containers
    std::string m_xaodTruthEventContainerName;
    std::string m_xaodTruthPUEventContainerName;
    std::string m_xaodTruthParticleContainerName;
    std::string m_xaodTruthVertexContainerName;

    void printEvent(const xAOD::TruthEventBase*);
    void printVertex(const xAOD::TruthVertex*);
    void printParticle(const xAOD::TruthParticle*);

  }; // class xAODTruthReader


} // namespace xAODReader

#endif // XAODCREATORALGS_XAODTRUTHREADER_H
