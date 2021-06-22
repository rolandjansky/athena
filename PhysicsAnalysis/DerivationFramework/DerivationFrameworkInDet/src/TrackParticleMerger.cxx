/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   TrackParticleMerger
//   Author: Bingxuan Liu, bingxuan.liu@cern.ch
//   This is a modified version of exisitng TrackCollectionMerger
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Track Particle merger to be used downstream, mainly for 
//          combining LRT and standard tracks 
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/TrackParticleMerger.h"
#include "GaudiKernel/MsgStream.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace DerivationFramework {

  TrackParticleMerger::TrackParticleMerger(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_createViewCollection(true)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    // The default goal of this merger is to create a collection combining standard and LRT tracks
    m_outtrackParticleLocation = "InDetWithLRTTrackParticles"    ;
    declareProperty("TrackParticleLocation",         m_trackParticleLocation);
    declareProperty("OutputTrackParticleLocation",   m_outtrackParticleLocation); 
    declareProperty("CreateViewColllection" ,        m_createViewCollection   );
  }

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////

  StatusCode TrackParticleMerger::initialize()
  {

    ATH_MSG_DEBUG("Initializing TrackParticleMerger");
    ATH_CHECK( m_trackParticleLocation.initialize() );
    ATH_CHECK( m_outtrackParticleLocation.initialize() );
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute
  ///////////////////////////////////////////////////////////////////
  StatusCode TrackParticleMerger::execute()
  {
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////

  StatusCode TrackParticleMerger::finalize()
  {
     return StatusCode::SUCCESS;
  }

 
  StatusCode TrackParticleMerger::addBranches() const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    std::unique_ptr<xAOD::TrackParticleContainer> outputCol = m_createViewCollection ?
    std::make_unique<xAOD::TrackParticleContainer>(SG::VIEW_ELEMENTS) : std::make_unique<xAOD::TrackParticleContainer>();
    ATH_MSG_DEBUG("Number of Track Particle collections " << m_trackParticleLocation.size());

    // pre-loop to reserve enough space in the output collection
    std::vector<const xAOD::TrackParticleContainer*> trackParticleCollections;
    trackParticleCollections.reserve(m_trackParticleLocation.size());
    size_t ttNumber = 0;
    for (auto& tcname : m_trackParticleLocation){
      ///Retrieve track particles from StoreGate
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCol (tcname, ctx);
      if (!trackParticleCol.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve xAOD::TrackParticleContainer, \"" << tcname << "\", returning without running LRT merger!");
        return StatusCode::SUCCESS;
      }
      trackParticleCollections.push_back(trackParticleCol.cptr());
      ttNumber += trackParticleCol->size();
    }
    outputCol->reserve(ttNumber);
    // merging loop
    for(auto& tciter : trackParticleCollections){
      // merge them in
      mergeTrackParticle(tciter,outputCol.get());
    }
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write(m_outtrackParticleLocation, ctx);
    ATH_CHECK(h_write.record(std::move(outputCol)));	     
    ATH_MSG_DEBUG("Done !");  
    return StatusCode::SUCCESS;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Merge track collections and remove duplicates
  ///////////////////////////////////////////////////////////////////

  void TrackParticleMerger::mergeTrackParticle(const xAOD::TrackParticleContainer* trackParticleCol,
                                                  xAOD::TrackParticleContainer* outputCol) const
  {
    // loop over tracks, accept them and add them into association tool
    if(trackParticleCol && trackParticleCol->size()) {
      ATH_MSG_DEBUG("Size of track particle collection " << trackParticleCol->size());
      // loop over tracks
      for(const auto rf: *trackParticleCol){
        // add track into output
        // FIXME: const_cast
        xAOD::TrackParticle* newTrackParticle = m_createViewCollection ? const_cast<xAOD::TrackParticle*>(rf) : new xAOD::TrackParticle(*rf);
        outputCol->push_back(newTrackParticle);
      }
    }
  }
}

