/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuEntryVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "TrackRecord/TrackRecordCollection.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode MuEntryVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

  ATH_MSG_DEBUG("do fillNSWMuEntryVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  CHECK( this->clearVariables() );

  const TrackRecordCollection* trackRecordCollection = nullptr;
  CHECK( m_evtStore->retrieve(trackRecordCollection, m_ContainerName.c_str()) );

  for(const auto& it : *trackRecordCollection ) {

    const CLHEP::Hep3Vector momentum = it.GetMomentum();
    const CLHEP::Hep3Vector position = it.GetPosition();
    m_MuEntry_particlePt->push_back(momentum.perp());
    m_MuEntry_particleEta->push_back(momentum.getEta());
    m_MuEntry_particlePhi->push_back(momentum.getPhi());
    m_MuEntry_particlePdg_id->push_back(it.GetPDGCode());
    m_MuEntry_particleBarcode->push_back(it.GetBarCode());
    m_MuEntry_positionEta->push_back(position.getEta());
    m_MuEntry_positionPhi->push_back(position.getPhi());
    m_MuEntry_positionX->push_back(position.x());
    m_MuEntry_positionY->push_back(position.y());
    m_MuEntry_positionZ->push_back(position.z());
    m_MuEntry_nParticles++;

  }

  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode MuEntryVariables::clearVariables()
{
  m_MuEntry_nParticles = 0;
  m_MuEntry_particlePt->clear();
  m_MuEntry_particleEta->clear();
  m_MuEntry_particlePhi->clear();
  m_MuEntry_particlePdg_id->clear();
  m_MuEntry_particleBarcode->clear();
  m_MuEntry_positionEta->clear();
  m_MuEntry_positionPhi->clear();
  m_MuEntry_positionX->clear();
  m_MuEntry_positionY->clear();
  m_MuEntry_positionZ->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode MuEntryVariables::initializeVariables()
{

  m_MuEntry_nParticles = 0;
  m_MuEntry_particlePt = new std::vector<double>;
  m_MuEntry_particleEta = new std::vector<double>;
  m_MuEntry_particlePhi = new std::vector<double>;
  m_MuEntry_particlePdg_id = new std::vector<int>;
  m_MuEntry_particleBarcode = new std::vector<int>;
  m_MuEntry_positionEta = new std::vector<double>;
  m_MuEntry_positionPhi = new std::vector<double>;
  m_MuEntry_positionX = new std::vector<double>;
  m_MuEntry_positionY = new std::vector<double>;
  m_MuEntry_positionZ = new std::vector<double>;
  if(m_tree) {
    m_tree->Branch("MuEntry_Particle_n", &m_MuEntry_nParticles, "MuEntryParticle_n/i");
    m_tree->Branch("MuEntry_Particle_Pt", &m_MuEntry_particlePt);
    m_tree->Branch("MuEntry_Particle_Eta", &m_MuEntry_particleEta);
    m_tree->Branch("MuEntry_Particle_Phi", &m_MuEntry_particlePhi);
    m_tree->Branch("MuEntry_Particle_Pdg", &m_MuEntry_particlePdg_id);
    m_tree->Branch("MuEntry_Particle_Barcode", &m_MuEntry_particleBarcode);
    m_tree->Branch("MuEntry_Position_Eta", &m_MuEntry_positionEta);
    m_tree->Branch("MuEntry_Position_Phi", &m_MuEntry_positionPhi);
    m_tree->Branch("MuEntry_Position_X", &m_MuEntry_positionX);
    m_tree->Branch("MuEntry_Position_Y", &m_MuEntry_positionY);
    m_tree->Branch("MuEntry_Position_Z", &m_MuEntry_positionZ);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void MuEntryVariables::deleteVariables()
{

  m_MuEntry_nParticles = 0;

  delete m_MuEntry_particlePt;
  delete m_MuEntry_particleEta;
  delete m_MuEntry_particlePhi;
  delete m_MuEntry_particlePdg_id;
  delete m_MuEntry_particleBarcode;
  delete m_MuEntry_positionEta;
  delete m_MuEntry_positionPhi;
  delete m_MuEntry_positionX;
  delete m_MuEntry_positionY;
  delete m_MuEntry_positionZ;

  m_MuEntry_particlePt = nullptr;
  m_MuEntry_particleEta = nullptr;
  m_MuEntry_particlePhi = nullptr;
  m_MuEntry_particlePdg_id = nullptr;
  m_MuEntry_particleBarcode = nullptr;
  m_MuEntry_positionEta = nullptr;
  m_MuEntry_positionPhi = nullptr;
  m_MuEntry_positionX = nullptr;
  m_MuEntry_positionY = nullptr;
  m_MuEntry_positionZ = nullptr;

  return;
}
