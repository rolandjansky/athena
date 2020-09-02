/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsPropStepRootWriterSvc.h"
#include "GaudiKernel/IInterface.h"

#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Geometry/TrackingVolume.hpp"


#include <vector>
#include <deque>
#include <mutex>
#include <thread>

#include "TTree.h"
#include "TFile.h"


ActsPropStepRootWriterSvc::ActsPropStepRootWriterSvc( const std::string& name, ISvcLocator* svc )
: base_class(name, svc) {
}

StatusCode
ActsPropStepRootWriterSvc::initialize()
{


  std::string filePath = m_filePath;
  m_outputFile = TFile::Open(filePath.c_str(), "RECREATE");
  if(m_outputFile == nullptr) {
    ATH_MSG_ERROR("Unable to open output file at " << m_filePath);
    return StatusCode::FAILURE;
  }
  m_outputFile->cd();

  std::string treeName = m_treeName;
  m_outputTree = new TTree(treeName.c_str(), "Acts Propagation Steps");
  if(m_outputTree == nullptr) {
    ATH_MSG_ERROR("Unable to create TTree");
    return StatusCode::FAILURE;
  }

  m_outputTree->Branch("event_nr", &m_eventNum);
  m_outputTree->Branch("step_x", &m_s_pX);
  m_outputTree->Branch("step_y", &m_s_pY);
  m_outputTree->Branch("step_z", &m_s_pZ);
  m_outputTree->Branch("step_r", &m_s_pR);
  m_outputTree->Branch("volume_id", &m_s_volumeID);
  m_outputTree->Branch("boundary_id", &m_s_boundaryID);
  m_outputTree->Branch("layer_id", &m_s_layerID);
  m_outputTree->Branch("approach_id", &m_s_approachID);
  m_outputTree->Branch("sensitive_id", &m_s_sensitiveID);


  return StatusCode::SUCCESS;
}
  
StatusCode 
ActsPropStepRootWriterSvc::finalize()
{
  end();

  return StatusCode::SUCCESS;
}

void 
ActsPropStepRootWriterSvc::write(const StepVector& steps)
{
  
  auto ctx = Gaudi::Hive::currentContext();

  std::lock_guard<std::mutex> lock(m_writeMutex);

  //for(size_t i=0;i<ecells.size();++i) {
    //m_queue.emplace_back(ctx.eventID().event_number(), std::move(ecells[i]));
  //}
  doWrite(steps, ctx.eventID().event_number());
}

void
ActsPropStepRootWriterSvc::writeThread()
{
  using namespace std::chrono_literals;
  // wait until we have events
  while(m_queue.size() == 0) {
    std::this_thread::sleep_for(2s);
    if (m_doEnd) return;
  }

  while(true) {
    std::unique_lock<std::mutex> lock(m_writeMutex);
    
    if (m_queue.empty()) {
      lock.unlock();
      if (!m_doEnd) {
        std::this_thread::sleep_for(0.5s);
        continue;
      } else {
        ATH_MSG_INFO("Writer thread caught termination signal. Shutting down.");
        end();
        return;
      }
    }

    queue_item_t queue_item = std::move(m_queue.front());
    m_queue.pop_front();

    lock.unlock();
    
    size_t eventNum = queue_item.first;
    StepVector steps = std::move(queue_item.second);

    doWrite(steps, eventNum);
  }

}

void
ActsPropStepRootWriterSvc::doWrite(const StepVector& steps, size_t evtNum)
{
  using ag = Acts::GeometryID;

  m_eventNum = evtNum;
  m_s_pX.clear();
  m_s_pY.clear();
  m_s_pZ.clear();
  m_s_pR.clear();
  m_s_volumeID.clear();
  m_s_boundaryID.clear();
  m_s_layerID.clear();
  m_s_approachID.clear();
  m_s_sensitiveID.clear();

  for(const auto& step : steps) {
      Acts::GeometryID::Value volumeID    = 0;
      Acts::GeometryID::Value boundaryID  = 0;
      Acts::GeometryID::Value layerID     = 0;
      Acts::GeometryID::Value approachID  = 0;
      Acts::GeometryID::Value sensitiveID = 0;
      // get the identification from the surface first
      if (step.surface) {
        auto geoID  = step.surface->geoID();
        sensitiveID = geoID.sensitive();
        approachID  = geoID.approach();
        layerID     = geoID.layer();
        boundaryID  = geoID.boundary();
        volumeID    = geoID.volume();
      }
      // a current volume overwrites the surface tagged one
      if (step.volume) {
        volumeID = step.volume->geoID().volume();
      }
      // now fill
      m_s_sensitiveID.push_back(sensitiveID);
      m_s_approachID.push_back(approachID);
      m_s_layerID.push_back(layerID);
      m_s_boundaryID.push_back(boundaryID);
      m_s_volumeID.push_back(volumeID);

      m_s_pX.push_back(step.position.x());
      m_s_pY.push_back(step.position.y());
      m_s_pZ.push_back(step.position.z());
      m_s_pR.push_back(Acts::VectorHelpers::perp(step.position));
  }
  
  m_outputTree->Fill();
}

void
ActsPropStepRootWriterSvc::end()
{
  m_outputFile->cd();
  m_outputTree->Write();
  //m_outputFile->Close();
}
