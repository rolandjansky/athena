/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCREATORALGS_XAODTRUTHCNVALG_H
#define XAODCREATORALGS_XAODTRUTHCNVALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#ifdef HEPMC3
// This form of ifdef is kept for convenience
#else
// The lines below I don't like. We should fix them when we update the
// the metadata to handles (ATLASRECTS-4162).
// Needs changes in HepMC to resolve.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#include "AtlasHepMC/WeightContainer.h"
#undef private
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "GeneratorObjects/McEventCollection.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthParticleContainerFwd.h"
#include "xAODTruth/TruthVertexContainerFwd.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CxxUtils/checker_macros.h"

#include <unordered_set>


#include "AtlasHepMC/WeightContainer.h"
#include "AtlasHepMC/Polarization.h"
#include "AtlasHepMC/GenVertex_fwd.h"
#include "AtlasHepMC/GenParticle_fwd.h"


namespace xAODMaker {

  /// @brief Algorithm creating xAOD truth from HepMC
  ///
  /// This algorithm can be used to translate the HepMC coming from an AOD, and
  /// create xAOD truth objects out of them for an output xAOD.
  ///
  /// @author James Catmore <James.Catmore@cern.ch>
  /// @author Jovan Mitreski <Jovan.Mitreski@cern.ch>
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  class xAODTruthCnvAlg : public AthReentrantAlgorithm {
  public:

    /// Regular algorithm constructor
    xAODTruthCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize() override;
    /// Function executing the algorithm
    virtual StatusCode execute (const EventContext& ctx) const override;


  private:
    /// Factor out the pieces dealing with writing to meta data.
    /// This will be non-const, so need to protect with a mutex.
    class MetaDataWriter
    {
    public:
      StatusCode initialize (ServiceHandle<StoreGateSvc>& metaStore,
                             const std::string& metaName);
      StatusCode maybeWrite (uint32_t mcChannelNumber,
                             const HepMC::GenEvent& genEvt);


    private:
      /// Mutex to control access to meta data writing.
      std::mutex m_mutex;
      typedef std::lock_guard<std::mutex> lock_t;

      /// The meta data container to be written out
      xAOD::TruthMetaDataContainer* m_tmd = nullptr;

      /// Set for tracking the mc channels for which we already added meta data 
      std::unordered_set<uint32_t> m_existingMetaDataChan;
    };

    /// Type for tracking particles connected to a single vertex
    struct VertexParticles {
      std::vector<xAOD::TruthParticle*> incoming;
      std::vector<ElementLink<xAOD::TruthParticleContainer> > incomingEL;
      std::vector<xAOD::TruthParticle*> outgoing;
      std::vector<ElementLink<xAOD::TruthParticleContainer> > outgoingEL;
    };
    /// Convenience handle for a map of vtx ptrs -> connected particles
    typedef std::map<HepMC::ConstGenVertexPtr, VertexParticles> VertexMap;

    /// These functions do not set up ELs, just the other variables
    static void fillVertex(xAOD::TruthVertex *tv, HepMC::ConstGenVertexPtr gv);
    static void fillParticle(xAOD::TruthParticle *tp, HepMC::ConstGenParticlePtr gp);

    /// The key of the input AOD truth container
    SG::ReadHandleKey<McEventCollection> m_aodContainerKey{ 
      this, "AODContainerName", "GEN_AOD", "The input McEvenCollection"};
    
    /// The key for the output xAOD truth containers
    SG::WriteHandleKey<xAOD::TruthEventContainer> m_xaodTruthEventContainerKey{
      this, "xAODTruthEventContainerName", "TruthEvents", "Output TruthEvents container"};
    SG::WriteHandleKey<xAOD::TruthPileupEventContainer> m_xaodTruthPUEventContainerKey{
      this, "xAODTruthPileupEventContainerName", "TruthPileupEvents", "Output TruthPileupEvents container"};
    SG::WriteHandleKey<xAOD::TruthParticleContainer> m_xaodTruthParticleContainerKey{
      this, "xAODTruthParticleContainerName", "TruthParticles", "Output TruthParticles container"};
    SG::WriteHandleKey<xAOD::TruthVertexContainer> m_xaodTruthVertexContainerKey{
      this, "xAODTruthVertexContainerName", "TruthVertices", "Output TruthVertices container"};
    SG::WriteHandleKey<xAODTruthParticleLinkVector> m_truthLinkContainerKey{
      this, "TruthLinks", "xAODTruthLinks", "Output xAODTruthLinks container"};

    /// Pile-up options
    Gaudi::Property<bool> m_doAllPileUp{this, "WriteAllPileUpTruth", false};
    Gaudi::Property<bool> m_doInTimePileUp{this, "WriteInTimePileUpTruth", false};

    /// Helper for writing to the meta data store.
    /// This factors out the pieces that are non-const.
    /// It's declared mutable and protected with a mutex.
    mutable MetaDataWriter m_meta ATLAS_THREAD_SAFE;

    /// Connection to the metadata store
    ServiceHandle< StoreGateSvc > m_metaStore;
    ServiceHandle<StoreGateSvc> m_inputMetaStore;
    /// SG key and name for meta data
    std::string m_metaName;

    /// option to disable writing of metadata (e.g. if running a filter on xAOD in generators)
    Gaudi::Property<bool> m_writeMetaData{this, "WriteTruthMetaData", true};

  }; // class xAODTruthCnvAlg


} // namespace xAODMaker

#endif // XAODCREATORALGS_XAODTRUTHCNVALG_H
