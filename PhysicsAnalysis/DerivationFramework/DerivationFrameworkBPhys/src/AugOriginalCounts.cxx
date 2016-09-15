/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  }
 
  StatusCode AugOriginalCounts::addBranches() const
  {
      const xAOD::EventInfo* Info =0;

      if(evtStore()->retrieve( Info ).isFailure()){
        ATH_MSG_ERROR ("Cannot retrieve event info");
        return StatusCode::FAILURE;
      }


      if(!m_PVContainername.empty()){


//      std::unique_ptr<int > PV_count(new int());
      std::string pvstring = "OriginalCount_";
      pvstring+= m_PVContainername;
      static EventInfo::Decorator< int > PV_count(pvstring);
      // CALCULATION OF THE NEW VARIABLE
      // Get Primary vertex
      const xAOD::VertexContainer* vertices =  evtStore()->retrieve< const xAOD::VertexContainer >(m_PVContainername);
   
      if(!vertices) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key PrimaryVertices");
        return StatusCode::FAILURE;
      }
      PV_count(*Info) = vertices->size();
//      *PV_count = vertices->size();
       

     }


     if(!m_TrackContainername.empty()){
         
//      std::unique_ptr<int > track_count(new int());
      std::string pvstring = "OriginalCount_";
      pvstring+= m_TrackContainername;
      static EventInfo::Decorator< int > track_count(pvstring);
      const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_TrackContainername);
      if(!tracks) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key PrimaryVertices");
        return StatusCode::FAILURE;
      }
      track_count(*Info) = tracks->size();


     }
     
 
      return StatusCode::SUCCESS;
  }
}

