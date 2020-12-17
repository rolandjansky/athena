/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ThinningToolExample.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkBPhys/BPhysPVThinningTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "StoreGate/ThinningHandle.h"
#include "xAODBPhys/BPhysHelper.h"
#include "StoreGate/ThinningHandle.h"
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

using namespace std;
using namespace xAOD;
// Constructor
DerivationFramework::BPhysPVThinningTool::BPhysPVThinningTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p),
    m_TrackContainerName("InDetTrackParticles"),
    m_PVContainerName("PrimaryVertices"),
    m_ntot(0),
    m_npass(0), m_tracks_kept(0), m_keepTracks(false)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("CandidateCollections" , m_BPhyCandList);
    declareProperty("KeepPVTracks", m_keepTracks);
    declareProperty("TrackParticleContainerName", m_TrackContainerName);
    declareProperty("PrimaryVertexContainerName", m_PVContainerName);
}

// Destructor
DerivationFramework::BPhysPVThinningTool::~BPhysPVThinningTool() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::BPhysPVThinningTool::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK(m_BPhyCandList.initialize());
    if(not m_TrackContainerName.key().empty()) ATH_CHECK(m_TrackContainerName.initialize(m_streamName));
    ATH_CHECK(m_PVContainerName.initialize(m_streamName));

    return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::BPhysPVThinningTool::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" PV, "<< m_npass<< " were retained ");
    if(m_keepTracks) ATH_MSG_INFO("Additional tracks kept " << m_tracks_kept);
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::BPhysPVThinningTool::doThinning() const
{

    // Get the track container
    SG::ThinningHandle<xAOD::VertexContainer> PV_col(m_PVContainerName);
    if(!PV_col.isValid()) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key PrimaryVertices");
        return StatusCode::FAILURE;
    }
    m_ntot+=PV_col->size();
    // Loop over tracks, see if they pass, set mask
    std::vector<bool> mask(PV_col->size(), false);

    BPhysHelper::pv_type pvtypes[] = {BPhysHelper::PV_MAX_SUM_PT2,
				      BPhysHelper::PV_MIN_A0,
				      BPhysHelper::PV_MIN_Z0,
				      BPhysHelper::PV_MIN_Z0_BA};
 
    

    for(auto &str : m_BPhyCandList) {
        SG::ReadHandle<xAOD::VertexContainer> Container(str);
        ATH_CHECK(Container.isValid());
        size_t s = Container->size();
        for(size_t i = 0; i<s; i++) {
            xAOD::BPhysHelper vtx(Container->at(i));

            for(size_t i =0; i < 4; i++) {
                 const xAOD::Vertex* origPv = vtx.origPv(pvtypes[i]);
                 if(origPv==nullptr) continue;
                 auto pvit = std::find (PV_col->begin(), PV_col->end(), origPv);
                 if(pvit == PV_col->end()) {
                    ATH_MSG_WARNING("PV not found in container");
                    continue;
                 }
                size_t x = std::distance(PV_col->begin(), pvit);
                mask.at(x) = true;
            }

        }
    }

    m_npass += std::accumulate(mask.begin(), mask.end(), 0);

    if(m_keepTracks){
         SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles(m_TrackContainerName);
         std::vector<bool> trackmask(importedTrackParticles->size(), false);
         size_t pvnum = mask.size();
         for(size_t i =0; i<pvnum;i++){
            if(mask[i] == false) continue;
            auto vtx =  PV_col->at(i);
            size_t s = vtx->nTrackParticles();
            for(size_t j=0;j<s;j++){
                auto trackit = std::find(importedTrackParticles->begin(), importedTrackParticles->end(), vtx->trackParticle(j));
                if(trackit == importedTrackParticles->end()){
                    ATH_MSG_WARNING("track not found in container");
                    continue;
                }
                size_t x = std::distance(importedTrackParticles->begin(), trackit);
                trackmask.at(x) = true;
            }
         }
      importedTrackParticles.keep(trackmask, SG::ThinningHandleBase::Op::Or);
      m_tracks_kept += std::accumulate(trackmask.begin(), trackmask.end(), 0);
    }
    PV_col.keep(mask, SG::ThinningHandleBase::Op::Or);

    return StatusCode::SUCCESS;
}

