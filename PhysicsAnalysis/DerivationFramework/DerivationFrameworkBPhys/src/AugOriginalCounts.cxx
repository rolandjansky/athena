/* 
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AugOriginalCounts.cxx
 *
 */

#include "DerivationFrameworkBPhys/AugOriginalCounts.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <string>
#include "xAODEventInfo/EventInfo.h"
using namespace xAOD;
namespace DerivationFramework {
 
  AugOriginalCounts::AugOriginalCounts(const std::string& t,
                                       const std::string& n,
                                       const IInterface* p) :
    AthAlgTool(t,n,p),
    m_TrackContainername("InDetTrackParticles"),
    m_PVContainername("PrimaryVertices")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("TrackContainer", m_TrackContainername);
    declareProperty("VertexContainer", m_PVContainername);
    declareProperty("AddPVCountsByType", m_addPVCountsByType = false);
    // decorate PVs with track counts and/or sqrt(sum(pt^2))
    // (needed if track collection will be thinned)
    declareProperty("AddNTracksToPVs", m_addNTracksToPVs = false);
    declareProperty("AddSqrtPt2SumToPVs", m_addSqrtPt2SumToPVs = false);
  }
 
  StatusCode AugOriginalCounts::addBranches() const
  {
    const xAOD::EventInfo* Info = nullptr;
    
    if(evtStore()->retrieve( Info ).isFailure()){
      ATH_MSG_ERROR ("Cannot retrieve event info");
      return StatusCode::FAILURE;
    }
    
    if(!m_PVContainername.empty()){
      
      std::string pvstring = "OriginalCount_";
      pvstring+= m_PVContainername;
      static EventInfo::Decorator< int > PV_count(pvstring);
      // CALCULATION OF THE NEW VARIABLE
      // Get Primary vertex
      const xAOD::VertexContainer* vertices =
        evtStore()->retrieve< const xAOD::VertexContainer >(m_PVContainername);
      if(!vertices) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key "
                       << m_PVContainername);
        return StatusCode::FAILURE;
      }
      PV_count(*Info) = vertices->size();
      
      if ( m_addPVCountsByType ) {
        std::string pv0string = "OriginalCount_type0_"+m_PVContainername;
        std::string pv1string = "OriginalCount_type1_"+m_PVContainername;
        std::string pv2string = "OriginalCount_type2_"+m_PVContainername;
        std::string pv3string = "OriginalCount_type3_"+m_PVContainername;
        std::string pvUstring = "OriginalCount_typeUnknown_"+m_PVContainername;
        static EventInfo::Decorator< int > PV0_count(pv0string);
        static EventInfo::Decorator< int > PV1_count(pv1string);
        static EventInfo::Decorator< int > PV2_count(pv2string);
        static EventInfo::Decorator< int > PV3_count(pv3string);
        static EventInfo::Decorator< int > PVU_count(pvUstring);
	
        // now count
        int nvtypes = 5;
        int nvtc[] = {0, 0, 0, 0, 0};
        for (auto vtx : *vertices) {
          VxType::VertexType vt = vtx->vertexType();
          if ( vt >=0 && vt < nvtypes ) {
            nvtc[vt]++; // vertex types 0 - 3
          } else {
            nvtc[nvtypes-1]++; // unknown
          }
        }
        PV0_count(*Info) = nvtc[0];
        PV1_count(*Info) = nvtc[1];
        PV2_count(*Info) = nvtc[2];
        PV3_count(*Info) = nvtc[3];
        PVU_count(*Info) = nvtc[4];
      } // m_addPVCountsByType

      // decorate PVs with track counts
      // (needed if track collection will be thinned)
      if ( m_addNTracksToPVs ) {
    
        static SG::AuxElement::Decorator< int > d_nPVTracks("OrigNTracks");
        for (auto vtx : *vertices) {
          d_nPVTracks(*vtx) = (int)vtx->nTrackParticles();
        }
        
      } // m_addNTracksToPVs
      
      // decorate PVs with sqrt(sum(pt^2)) of tracks
      // (needed if track collection will be thinned)
      if ( m_addSqrtPt2SumToPVs ) {
        static SG::AuxElement::Decorator< float >
          d_pvSqrtPt2Sum("OrigSqrtPt2Sum");
        for (auto vtx : *vertices) {
          float sqrtPt2Sum(0.);
          for (auto tp : vtx->trackParticleLinks()) {
            sqrtPt2Sum += sqrt(pow((*tp)->pt(),2));
          }
          d_pvSqrtPt2Sum(*vtx) = sqrtPt2Sum;
        }
      } // m_addSqrtPt2SumToPVs
    }
    
    if(!m_TrackContainername.empty()){
      
      std::string pvstring = "OriginalCount_";
      pvstring+= m_TrackContainername;
      static EventInfo::Decorator< int > track_count(pvstring);
      const xAOD::TrackParticleContainer* tracks =
        evtStore()->retrieve<
          const xAOD::TrackParticleContainer >(m_TrackContainername);
      if(!tracks) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key "
                       << m_TrackContainername);
        return StatusCode::FAILURE;
      }
      track_count(*Info) = tracks->size();
    }

    return StatusCode::SUCCESS;
  }
}
