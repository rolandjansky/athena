/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "METReconstruction/METBadTrackEventFilter.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

METBadTrackEventFilter::METBadTrackEventFilter(const std::string& name, ISvcLocator* pSvcLocator) :  AthAlgorithm(name, pSvcLocator) 
{
  declareProperty("TrackSelectorTool",  m_trkseltool  );
}

StatusCode METBadTrackEventFilter::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode METBadTrackEventFilter::execute(){
  //TODO: If we want to enable it, need to run on all nonisolated high pt tracks
  this->setFilterPassed(false);
  const xAOD::VertexContainer *vxCont = 0;
  const xAOD::Vertex* pv = 0;
  if ( evtStore()->retrieve(vxCont, "PrimaryVertices").isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve primary vertex container");
        return StatusCode::SUCCESS;
  } else if(vxCont->empty()) {
        ATH_MSG_WARNING("Event has no primary vertices!");
        return StatusCode::SUCCESS;
  }
  for(const auto& vx : *vxCont) if(vx->vertexType()==xAOD::VxType::PriVtx) {pv = vx; break;}
  if(!pv) {
    ATH_MSG_WARNING("Failed to find primary vertex!");
    return StatusCode::SUCCESS;
  }
  const xAOD::CaloClusterContainer* tcCont = 0;
  ATH_CHECK(evtStore()->retrieve(tcCont, "CaloCalTopoClusters"));
  const xAOD::TrackParticleContainer* trkCont = 0;
  ATH_CHECK(evtStore()->retrieve(trkCont, "InDetTrackParticles"));
  std::vector<const xAOD::TrackParticle*> goodtracks;
  for(const auto& trk : *trkCont) {
    if(m_trkseltool->accept( *trk, pv )) goodtracks.push_back(trk);
  }
  for(const auto& trk : goodtracks) {
    if( (fabs(trk->eta())<1.5 && trk->pt()>200e3) ||
        (fabs(trk->eta())>=1.5 && trk->pt()>120e3) ) {

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());

      // first compute track and calo isolation variables
      float ptcone20 = 0;
      for(const auto& testtrk : goodtracks) {
        if(testtrk==trk) continue;
        if(testtrk->p4().DeltaR(trk->p4()) < 0.2) {
          ptcone20 += testtrk->pt();
        }
      }
      float isolfrac = ptcone20 / trk->pt();
      ATH_MSG_VERBOSE( "Track isolation fraction: " << isolfrac );

      float etcone10em = 0.;
      float etcone10lc = 0.;
      for(const auto& clus : *tcCont) {
        {
          CaloClusterChangeSignalStateList stateHelperList;    
          stateHelperList.add(clus,xAOD::CaloCluster::State(0));
          if(clus->p4().DeltaR(trk->p4()) < 0.1) etcone10em += clus->pt();
        }
        {
          CaloClusterChangeSignalStateList stateHelperList;    
          stateHelperList.add(clus,xAOD::CaloCluster::State(1));
          if(clus->p4().DeltaR(trk->p4()) < 0.1) etcone10lc += clus->pt();
        }
      }
      float EoverPem = etcone10em/trk->pt();
      float EoverPlc = etcone10lc/trk->pt();
      ATH_MSG_VERBOSE( "Track EM E/P: " << EoverPem << ", Track LC E/P: " << EoverPlc );

      //Fix for goodtracks bug
      //if(isolfrac<0.1)
      {
        // isolated track cuts
        if(Rerr>0.4) this->setFilterPassed(true);
        else if (EoverPem<0.65 && (EoverPem>0.1 || Rerr>0.1)) this->setFilterPassed(true);
        else if (EoverPlc<0.65 && (EoverPlc>0.1 || Rerr>0.1)) this->setFilterPassed(true);
      }
      if(isolfrac>=0.1) {
        // non-isolated track cuts
        float trkptsum = ptcone20+trk->pt();
        if(etcone10em/trkptsum<0.6 && trk->pt()/trkptsum>0.6) this->setFilterPassed(true);
        if(etcone10lc/trkptsum<0.6 && trk->pt()/trkptsum>0.6) this->setFilterPassed(true);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode METBadTrackEventFilter::finalize(){
  return StatusCode::SUCCESS;
}

