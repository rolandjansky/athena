#include "GeomACTS/MaterialTrackWriterSvc.h"
#include "GaudiKernel/IInterface.h"

#include "TTree.h"
#include "TFile.h"

#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Material/MaterialProperties.hpp"


#include <vector>
#include <deque>
#include <mutex>
#include <thread>

Acts::MaterialTrackWriterSvc::MaterialTrackWriterSvc( const std::string& name, ISvcLocator* svc )
: base_class(name, svc) {
}

StatusCode
Acts::MaterialTrackWriterSvc::initialize()
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

  ATH_MSG_INFO("Starting writer thread");
  m_doEnd = false;
  m_writeThread = std::thread(&MaterialTrackWriterSvc::doWrite, this);

  return StatusCode::SUCCESS;
}
  
StatusCode 
Acts::MaterialTrackWriterSvc::finalize()
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
Acts::MaterialTrackWriterSvc::write(const Acts::MaterialTrack& mTrack)
{
  std::lock_guard<std::mutex> lock(m_writeMutex);

  ATH_MSG_DEBUG("Appending material track to write queue");
  m_mTracks.push_back(mTrack);
}

void
Acts::MaterialTrackWriterSvc::doWrite()
{
  using namespace std::chrono_literals;
  // wait until we have events
  while(m_mTracks.size() == 0) {
    std::this_thread::sleep_for(2s);
    if (m_doEnd) return;
  }

  ATH_MSG_DEBUG("Begin regular write loop");
  while(true) {
    ATH_MSG_DEBUG("Obtaining write lock");
    std::unique_lock<std::mutex> lock(m_writeMutex);
    
    if (m_mTracks.empty()) {
      lock.unlock();
      if (!m_doEnd) {
        ATH_MSG_DEBUG("Queue was empty, delay next execution");
        std::this_thread::sleep_for(0.5s);
        continue;
      } else {
        ATH_MSG_INFO("Writer thread caught termination signal. Shutting down.");

        return;
      }
    }

    ATH_MSG_DEBUG("Pop entry and write");
    MaterialTrack mTrack = std::move(m_mTracks.front());
    m_mTracks.pop_front();

    // writing can now happen without lock
    lock.unlock();
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

    m_treeX0 = mTrack.thicknessInX0(); // name?
    m_treeL0 = mTrack.thicknessInL0(); // name?
    m_treeTheta = mTrack.theta();
    m_treePhi = mTrack.phi();
    m_treeT = 0;
    m_treedInX0 = 0;
    m_treedInL0 = 0;

    for(const auto& step : mTrack.materialSteps()) {
      const MaterialProperties& matProp = step.materialProperties();
      const Material& mat = matProp.material();

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

    }

    p_tree->Fill();
    //m_treeTTot = 0;

    ATH_MSG_DEBUG("Write complete");
  }
}
