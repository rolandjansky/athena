/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCREATORALGS_XAODTRUTHCNVALG_H
#define XAODCREATORALGS_XAODTRUTHCNVALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "StoreGate/StoreGateSvc.h"

#include <unordered_set>

namespace HepMC {
  class GenVertex;
  class GenParticle;
}

namespace xAODMaker {

  /// @brief Algorithm creating xAOD truth from HepMC
  ///
  /// This algorithm can be used to translate the HepMC coming from an AOD, and
  /// create xAOD truth objects out of them for an output xAOD.
  ///
  /// @author James Catmore <James.Catmore@cern.ch>
  /// @author Jovan Mitreski <Jovan.Mitreski@cern.ch>
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  class xAODTruthCnvAlg : public AthAlgorithm {
  public:

    /// Regular algorithm constructor
    xAODTruthCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute();


  private:

    /// Type for tracking particles connected to a single vertex
    struct VertexParticles {
      std::vector<xAOD::TruthParticle*> incoming;
      std::vector<ElementLink<xAOD::TruthParticleContainer> > incomingEL;
      std::vector<xAOD::TruthParticle*> outgoing;
      std::vector<ElementLink<xAOD::TruthParticleContainer> > outgoingEL;
    };
    /// Convenience handle for a map of vtx ptrs -> connected particles
    typedef std::map<const HepMC::GenVertex*, VertexParticles> VertexMap;

    /// These functions do not set up ELs, just the other variables
    static void fillVertex(xAOD::TruthVertex *tv, const HepMC::GenVertex *gv);
    static void fillParticle(xAOD::TruthParticle *tp, const HepMC::GenParticle *gp);

    /// The key of the input AOD truth container
    std::string m_aodContainerName;

    /// The key for the output xAOD truth containers
    std::string m_xaodTruthEventContainerName;
    std::string m_xaodTruthPUEventContainerName;
    std::string m_xaodTruthParticleContainerName;
    std::string m_xaodTruthVertexContainerName;
    std::string m_truthLinkContainerName;
    /// Pile-up options
    bool m_doAllPileUp;
    bool m_doInTimePileUp;

    /// Connection to the metadata store
    ServiceHandle< StoreGateSvc > m_metaStore;
    ServiceHandle<StoreGateSvc> m_inputMetaStore;
    /// The meta data container to be written out
    xAOD::TruthMetaDataContainer* m_tmd;
    /// SG key and name for meta data
    std::string m_metaName;
    /// Set for tracking the mc channels for which we already added meta data 
    std::unordered_set<uint32_t> m_existingMetaDataChan;

    /// a flag to force rerunning (useful for rerunning on ESDs)
    bool m_forceRerun;

    /// option to disable writing of metadata (e.g. if running a filter on xAOD in generators)
    bool m_writeMetaData;

  }; // class xAODTruthCnvAlg


} // namespace xAODMaker

#endif // XAODCREATORALGS_XAODTRUTHCNVALG_H
