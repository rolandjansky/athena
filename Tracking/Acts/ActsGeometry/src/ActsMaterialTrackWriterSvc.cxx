/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialTrackWriterSvc.h"
#include "GaudiKernel/IInterface.h"

#include "TTree.h"
#include "TFile.h"

#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Utilities/Helpers.hpp"

using namespace Acts::VectorHelpers;

#include <vector>
#include <deque>
#include <mutex>
#include <thread>

ActsMaterialTrackWriterSvc::ActsMaterialTrackWriterSvc( const std::string& name, ISvcLocator* svc )
: base_class(name, svc) {
}

StatusCode
ActsMaterialTrackWriterSvc::initialize()
{

  std::string filePath = m_filePath;
  std::string treeName = m_treeName;
  p_tFile = TFile::Open(filePath.c_str(), "RECREATE");
  p_tFile->cd();
  p_tree = new TTree(treeName.c_str(), treeName.c_str());

  p_tree->Branch("X0", &m_treeX0);
  p_tree->Branch("L0", &m_treeL0);
  p_tree->Branch("phi", &m_treePhi);
  p_tree->Branch("theta", &m_treeTheta);
  p_tree->Branch("T", &m_treeT);
  p_tree->Branch("dInX0", &m_treedInX0);
  p_tree->Branch("dInL0", &m_treedInL0);

  p_tree->Branch("step_X0", &m_treeStepX0);
  p_tree->Branch("step_L0", &m_treeStepL0);
  p_tree->Branch("step_A", &m_treeStepA);
  p_tree->Branch("step_Z", &m_treeStepZ);
  p_tree->Branch("step_rho", &m_treeStepRho);
  p_tree->Branch("step_t", &m_treeStepT);
  p_tree->Branch("step_dInX0", &m_treeStepdInX0);
  p_tree->Branch("step_dInL0", &m_treeStepdInL0);

  p_tree->Branch("step_x", &m_treeStepPosX);
  p_tree->Branch("step_y", &m_treeStepPosY);
  p_tree->Branch("step_z", &m_treeStepPosZ);
  p_tree->Branch("step_r", &m_treeStepPosR);
  p_tree->Branch("step_phi", &m_treeStepPosPhi);

  p_tree->Branch("step_geo_id", &m_treeStepGeoID);

  ATH_MSG_INFO("Starting writer thread");
  ATH_MSG_DEBUG("Maximum queue size is set to:" << m_maxQueueSize);
  m_doEnd = false;
  m_writeThread = std::thread(&ActsMaterialTrackWriterSvc::writerThread, this);

  return StatusCode::SUCCESS;
}
  
StatusCode 
ActsMaterialTrackWriterSvc::finalize()
{

  ATH_MSG_INFO("Waiting for writer thread to finish.");
  m_doEnd = true;
  m_writeThread.join();
  ATH_MSG_INFO("Writer thread has terminated.");
  
  ATH_MSG_INFO("Closing TFile");
  p_tFile->cd();
  p_tree->FlushBaskets();
  p_tree->AutoSave();
  p_tree->Write();
  //p_tFile->Write()
  p_tFile->Close();

  return StatusCode::SUCCESS;
}

void 
ActsMaterialTrackWriterSvc::write(const Acts::MaterialTrack& mTrack)
{
  std::lock_guard<std::mutex> lock(m_writeMutex);

  ATH_MSG_VERBOSE("Appending material track to write queue");
  m_mTracks.push_back(mTrack);
}

void
ActsMaterialTrackWriterSvc::writerThread()
{
  using namespace std::chrono_literals;
  // wait until we have events
  while(m_mTracks.size() == 0) {
    std::this_thread::sleep_for(2s);
    if (m_doEnd) return;
  }

  ATH_MSG_DEBUG("Begin regular write loop");
  while(true) {
    ATH_MSG_VERBOSE("Obtaining write lock");
    std::unique_lock<std::mutex> lock(m_writeMutex);
    
    if (m_mTracks.empty()) {
      lock.unlock();
      if (!m_doEnd) {
        ATH_MSG_VERBOSE("Queue was empty, delay next execution");
        std::this_thread::sleep_for(0.1s);
        continue;
      } else {
        ATH_MSG_INFO("Writer thread caught termination signal. Shutting down.");

        return;
      }
    }


    //if(m_mTracks.size() < m_maxQueueSize) {
      // just pop one
      ATH_MSG_VERBOSE("Queue at " << m_mTracks.size() << "/" << m_maxQueueSize 
          << ": Pop entry and write");
      Acts::MaterialTrack mTrack = std::move(m_mTracks.front());
      m_mTracks.pop_front();
      // writing can now happen without lock
      lock.unlock();
      doWrite(std::move(mTrack));
    //}
    //else {
      //ATH_MSG_DEBUG("Queue at " << m_mTracks.size() << "/" << m_maxQueueSize 
          //<< ": Lock and write until empty");
      //while(!m_mTracks.empty()) {
        //ATH_MSG_VERBOSE("Pop entry and write");
        //// keep the lock!
        //MaterialTrack mTrack = std::move(m_mTracks.front());
        //m_mTracks.pop_front();
        //doWrite(std::move(mTrack));
      //}
      //ATH_MSG_DEBUG("Queue is empty, continue");

    //}


  }
}

void
ActsMaterialTrackWriterSvc::doWrite(const Acts::MaterialTrack& mTrack)
{
  ATH_MSG_VERBOSE("Write to tree");
  size_t nSteps = mTrack.materialSteps().size();
  //m_treeStepPos.clear();
  //m_treeStepPos.reserve(nSteps);
  m_treeStepX0.clear();
  m_treeStepX0.reserve(nSteps);
  m_treeStepL0.clear();
  m_treeStepL0.reserve(nSteps);
  m_treeStepA.clear();
  m_treeStepA.reserve(nSteps);
  m_treeStepZ.clear();
  m_treeStepZ.reserve(nSteps);
  m_treeStepRho.clear();
  m_treeStepRho.reserve(nSteps);
  m_treeStepT.clear();
  m_treeStepT.reserve(nSteps);
  m_treeStepdInX0.clear();
  m_treeStepdInX0.reserve(nSteps);
  m_treeStepdInL0.clear();
  m_treeStepdInL0.reserve(nSteps);

  m_treeStepPosX.clear();
  m_treeStepPosX.reserve(nSteps);
  m_treeStepPosY.clear();
  m_treeStepPosY.reserve(nSteps);
  m_treeStepPosZ.clear();
  m_treeStepPosZ.reserve(nSteps);
  m_treeStepPosR.clear();
  m_treeStepPosR.reserve(nSteps);
  m_treeStepPosPhi.clear();
  m_treeStepPosPhi.reserve(nSteps);

  m_treeStepGeoID.clear();
  m_treeStepGeoID.reserve(nSteps);


  m_treeX0 = mTrack.thicknessInX0(); // name?
  m_treeL0 = mTrack.thicknessInL0(); // name?
  m_treeTheta = mTrack.theta();
  m_treePhi = mTrack.phi();
  m_treeT = 0;
  m_treedInX0 = 0;
  m_treedInL0 = 0;

  for(const auto& step : mTrack.materialSteps()) {
    const Acts::MaterialProperties& matProp = step.materialProperties();
    const Acts::Material& mat = matProp.material();
    const Acts::Vector3D pos = step.position();

    m_treeStepPosX.push_back(pos.x());
    m_treeStepPosY.push_back(pos.y());
    m_treeStepPosZ.push_back(pos.z());
    m_treeStepPosR.push_back(perp(pos));
    m_treeStepPosPhi.push_back(phi(pos));

    m_treeStepX0.push_back(mat.X0());
    m_treeStepL0.push_back(mat.L0());
    m_treeStepA.push_back(mat.A());
    m_treeStepZ.push_back(mat.Z());
    m_treeStepRho.push_back(mat.rho());
    m_treeStepT.push_back(matProp.thickness());
    m_treeStepdInX0.push_back(matProp.thicknessInX0());
    m_treeStepdInX0.push_back(matProp.thicknessInL0());

    m_treeT += matProp.thickness();
    m_treedInX0 += matProp.thicknessInX0();
    m_treedInL0 += matProp.thicknessInL0();

    m_treeStepGeoID.push_back(step.geoID());

  }

  p_tree->Fill();
  //m_treeTTot = 0;

  ATH_MSG_VERBOSE("Write complete");
}
