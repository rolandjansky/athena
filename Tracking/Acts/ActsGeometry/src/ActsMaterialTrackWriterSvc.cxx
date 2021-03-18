/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialTrackWriterSvc.h"
#include "GaudiKernel/IInterface.h"

#include "TTree.h"
#include "TFile.h"

#include "Acts/Material/MaterialSlab.hpp"
#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/Surfaces/CylinderBounds.hpp>
#include <Acts/Surfaces/RadialBounds.hpp>
#include <Acts/Utilities/Helpers.hpp>

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

  p_tree->Branch("v_x", &m_v_x);
  p_tree->Branch("v_y", &m_v_y);
  p_tree->Branch("v_z", &m_v_z);
  p_tree->Branch("v_px", &m_v_px);
  p_tree->Branch("v_py", &m_v_py);
  p_tree->Branch("v_pz", &m_v_pz);
  p_tree->Branch("v_phi", &m_v_phi);
  p_tree->Branch("v_eta", &m_v_eta);

  p_tree->Branch("t_X0", &m_tX0);
  p_tree->Branch("t_L0", &m_tL0);

  p_tree->Branch("mat_x", &m_step_x);
  p_tree->Branch("mat_y", &m_step_y);
  p_tree->Branch("mat_z", &m_step_z);
  p_tree->Branch("mat_dx", &m_step_dx);
  p_tree->Branch("mat_dy", &m_step_dy);
  p_tree->Branch("mat_dz", &m_step_dz);
  p_tree->Branch("mat_step_length", &m_step_length);
  p_tree->Branch("mat_X0", &m_step_X0);
  p_tree->Branch("mat_L0", &m_step_L0);
  p_tree->Branch("mat_A", &m_step_A);
  p_tree->Branch("mat_Z", &m_step_Z);
  p_tree->Branch("mat_rho", &m_step_rho);

  if (m_storeSurface) {
    p_tree->Branch("sur_id", &m_sur_id);
    p_tree->Branch("sur_type", &m_sur_type);
    p_tree->Branch("sur_x", &m_sur_x);
    p_tree->Branch("sur_y", &m_sur_y);
    p_tree->Branch("sur_z", &m_sur_z);
    p_tree->Branch("sur_range_min", &m_sur_range_min);
    p_tree->Branch("sur_range_max", &m_sur_range_max);
  }
  if (m_storeVolume) {
    p_tree->Branch("vol_id", &m_vol_id);
  }

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
  p_tFile->Write();
  p_tFile->Close();

  return StatusCode::SUCCESS;
}

void
ActsMaterialTrackWriterSvc::write(const Acts::RecordedMaterialTrack& mTrack)
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


    if(m_mTracks.size() < m_maxQueueSize) {
      // just pop one
      ATH_MSG_VERBOSE("Queue at " << m_mTracks.size() << "/" << m_maxQueueSize
          << ": Pop entry and write");
      Acts::RecordedMaterialTrack mTrack = std::move(m_mTracks.front());
      m_mTracks.pop_front();
      // writing can now happen without lock
      lock.unlock();
      doWrite(std::move(mTrack));
    }
    else {
      ATH_MSG_DEBUG("Queue at " << m_mTracks.size() << "/" << m_maxQueueSize
          << ": Lock and write until empty");
      while(!m_mTracks.empty()) {
        ATH_MSG_VERBOSE("Pop entry and write");
        // keep the lock!
        Acts::RecordedMaterialTrack mTrack = std::move(m_mTracks.front());
        m_mTracks.pop_front();
        doWrite(std::move(mTrack));
      }
      ATH_MSG_DEBUG("Queue is empty, continue");

    }
  }
}

void
ActsMaterialTrackWriterSvc::doWrite(const Acts::RecordedMaterialTrack& mTrack)
{
  ATH_MSG_VERBOSE("Write to tree");
  size_t mints = mTrack.second.materialInteractions.size();

  // Clearing the vector first
  m_step_sx.clear();
  m_step_sy.clear();
  m_step_sz.clear();
  m_step_x.clear();
  m_step_y.clear();
  m_step_z.clear();
  m_step_ex.clear();
  m_step_ey.clear();
  m_step_ez.clear();
  m_step_dx.clear();
  m_step_dy.clear();
  m_step_dz.clear();
  m_step_length.clear();
  m_step_X0.clear();
  m_step_L0.clear();
  m_step_A.clear();
  m_step_Z.clear();
  m_step_rho.clear();

  if(m_storeSurface){
    m_sur_id.clear();
    m_sur_type.clear();
    m_sur_x.clear();
    m_sur_y.clear();
    m_sur_z.clear();
    m_sur_range_min.clear();
    m_sur_range_max.clear();
  }

  if (m_storeVolume) {
    m_vol_id.clear();
  }  
  // Reserve the vector then
  m_step_sx.reserve(mints);
  m_step_sy.reserve(mints);
  m_step_sz.reserve(mints);
  m_step_x.reserve(mints);
  m_step_y.reserve(mints);
  m_step_ez.reserve(mints);
  m_step_ex.reserve(mints);
  m_step_ey.reserve(mints);
  m_step_ez.reserve(mints);
  m_step_dx.reserve(mints);
  m_step_dy.reserve(mints);
  m_step_dz.reserve(mints);
  m_step_length.reserve(mints);
  m_step_X0.reserve(mints);
  m_step_L0.reserve(mints);
  m_step_A.reserve(mints);
  m_step_Z.reserve(mints);
  m_step_rho.reserve(mints);

  if(m_storeSurface){
    m_sur_id.reserve(mints);
    m_sur_type.reserve(mints);
    m_sur_x.reserve(mints);
    m_sur_y.reserve(mints);
    m_sur_z.reserve(mints);
    m_sur_range_min.reserve(mints);
    m_sur_range_max.reserve(mints);
  }
  if (m_storeVolume) {
    m_vol_id.reserve(mints);
  }  

  // reset the global counter
  m_tX0 = mTrack.second.materialInX0;
  m_tL0 = mTrack.second.materialInL0;

  // set the track information at vertex
  m_v_x   = mTrack.first.first.x();
  m_v_y   = mTrack.first.first.y();
  m_v_z   = mTrack.first.first.z();
  m_v_px  = mTrack.first.second.x();
  m_v_py  = mTrack.first.second.y();
  m_v_pz  = mTrack.first.second.z();
  m_v_phi = phi(mTrack.first.second);
  m_v_eta = eta(mTrack.first.second);

  // an now loop over the material
  for (auto& mint : mTrack.second.materialInteractions) {
    // The material step position information
    m_step_x.push_back(mint.position.x());
    m_step_y.push_back(mint.position.y());
    m_step_z.push_back(mint.position.z());
    m_step_dx.push_back(mint.direction.x());
    m_step_dy.push_back(mint.direction.y());
    m_step_dz.push_back(mint.direction.z());

    Acts::Vector3 prePos
        = mint.position - 0.5 * mint.pathCorrection * mint.direction;
    Acts::Vector3 posPos
        = mint.position + 0.5 * mint.pathCorrection * mint.direction;
    m_step_sx.push_back(prePos.x());
    m_step_sy.push_back(prePos.y());
    m_step_sz.push_back(prePos.z());
    m_step_ex.push_back(posPos.x());
    m_step_ey.push_back(posPos.y());
    m_step_ez.push_back(posPos.z());

    if (m_storeSurface) {
      const Acts::Surface* surface = mint.surface;
      Acts::GeometryIdentifier layerID;
      if (surface) {
        auto sfIntersection = surface->intersect(
            Acts::GeometryContext(), mint.position, mint.direction, true);
        layerID = surface->geometryId();
        m_sur_id.push_back(layerID.value());
        m_sur_type.push_back(surface->type());
        m_sur_x.push_back(sfIntersection.intersection.position.x());
        m_sur_y.push_back(sfIntersection.intersection.position.y());
        m_sur_z.push_back(sfIntersection.intersection.position.z());

        const Acts::SurfaceBounds& surfaceBounds = surface->bounds();
        const Acts::RadialBounds* radialBounds =
            dynamic_cast<const Acts::RadialBounds*>(&surfaceBounds);
        const Acts::CylinderBounds* cylinderBounds =
            dynamic_cast<const Acts::CylinderBounds*>(&surfaceBounds);

        if (radialBounds) {
          m_sur_range_min.push_back(radialBounds->rMin());
          m_sur_range_max.push_back(radialBounds->rMax());
        } else if (cylinderBounds) {
          m_sur_range_min.push_back(
              -cylinderBounds->get(Acts::CylinderBounds::eHalfLengthZ));
          m_sur_range_max.push_back(
              cylinderBounds->get(Acts::CylinderBounds::eHalfLengthZ));
        } else {
          m_sur_range_min.push_back(0);
          m_sur_range_max.push_back(0);
        }
      } else {
        layerID.setVolume(0);
        layerID.setBoundary(0);
        layerID.setLayer(0);
        layerID.setApproach(0);
        layerID.setSensitive(0);
        m_sur_id.push_back(layerID.value());
        m_sur_type.push_back(-1);

        m_sur_x.push_back(0);
        m_sur_y.push_back(0);
        m_sur_z.push_back(0);
        m_sur_range_min.push_back(0);
        m_sur_range_max.push_back(0);
      }
    }
    // store volume information
    if (m_storeVolume) {
      const Acts::Volume* volume = mint.volume;
      Acts::GeometryIdentifier vlayerID;
      if (volume) {
        vlayerID = volume->geometryId();
        m_vol_id.push_back(vlayerID.value());
      } else {
        vlayerID.setVolume(0);
        vlayerID.setBoundary(0);
        vlayerID.setLayer(0);
        vlayerID.setApproach(0);
        vlayerID.setSensitive(0);
        m_vol_id.push_back(vlayerID.value());
      }
    }
    // the material information
    const auto& mprops = mint.materialSlab;
    m_step_length.push_back(mprops.thickness());
    m_step_X0.push_back(mprops.material().X0());
    m_step_L0.push_back(mprops.material().L0());
    m_step_A.push_back(mprops.material().Ar());
    m_step_Z.push_back(mprops.material().Z());
    m_step_rho.push_back(mprops.material().massDensity());

  }
  p_tree->Fill();
  ATH_MSG_VERBOSE("Write complete");
}
