/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCREATORALGS_XAODTRUTHCNVALG_H
#define XAODCREATORALGS_XAODTRUTHCNVALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#define private public
#   include "GeneratorObjects/McEventCollection.h"
#undef private

#include "xAODTruth/TruthMetaDataContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"

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
//    std::string m_aodContainerName;
    SG::ReadHandle<McEventCollection> m_mcColl;


    /// The key for the output xAOD truth containers
//    std::string m_xaodTruthEventContainerName;
    //SG::ReadHandle<xAOD::TruthEventContainer> m_xTruthEventContainer;

    SG::WriteHandle<xAOD::TruthEventContainer> m_xTruthEventContainer;
    SG::WriteHandle<xAOD::TruthEventAuxContainer> m_xTruthEventContainerAux;
    
    
//    std::string m_xaodTruthPUEventContainerName;
    
    SG::WriteHandle<xAOD::TruthPileupEventContainer> m_xTruthPileupEventContainer;
    SG::WriteHandle<xAOD::TruthPileupEventAuxContainer> m_xTruthPileupEventAuxContainer;
    
//    std::string m_xaodTruthParticleContainerName;
    SG::WriteHandle<xAOD::TruthParticleContainer> m_xTruthParticleContainer;
    SG::WriteHandle<xAOD::TruthParticleAuxContainer> m_xTruthParticleAuxContainer;
    
//    std::string m_xaodTruthVertexContainerName;
    
    SG::WriteHandle<xAOD::TruthVertexContainer> m_xTruthVertexContainer;
    SG::WriteHandle<xAOD::TruthVertexAuxContainer> m_xTruthVertexContainerAUX;
    
//    std::string m_truthLinkContainerName;
    
    SG::WriteHandle<xAODTruthParticleLinkVector> m_truthLinkVec;
    
    /// Pile-up options
    bool m_doAllPileUp;
    bool m_doInTimePileUp;

    /// Connection to the metadata store
    ServiceHandle< StoreGateSvc > m_metaStore;
    ServiceHandle<StoreGateSvc> inputMetaStore;
    /// The meta data container to be written out
    xAOD::TruthMetaDataContainer* m_tmd;
    /// SG key and name for meta data
    std::string m_metaName;
    /// Set for tracking the mc channels for which we already added meta data 
    std::unordered_set<uint32_t> m_existingMetaDataChan;

    /// a flag to force rerunning (useful for rerunning on ESDs)
    bool m_forceRerun;

  }; // class xAODTruthCnvAlg


} // namespace xAODMaker

#endif // XAODCREATORALGS_XAODTRUTHCNVALG_H
