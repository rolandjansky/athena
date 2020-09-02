/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode TruthVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

  ATH_MSG_DEBUG("do fillNSWTruthVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  CHECK( this->clearVariables() );

  const McEventCollection *truthContainer = nullptr;
  CHECK( m_evtStore->retrieve(truthContainer, m_ContainerName.c_str()) );

  unsigned int nSubEvents = 0;
  for(auto it : *truthContainer) {
    const HepMC::GenEvent *subEvent = it;

    // Vertex 
    HepMC::ConstGenEventVertexRange vertex_range = subEvent->vertex_range();
    for(auto vit : vertex_range) {
      const HepMC::GenVertex *vertex = vit;
      const HepMC::FourVector position = vertex->position();
      m_Truth_vertexX->push_back(position.x());
      m_Truth_vertexY->push_back(position.y());
      m_Truth_vertexZ->push_back(position.z());
      m_Truth_vertexT->push_back(position.t());
      m_Truth_vertexId->push_back(vertex->id());
      m_Truth_nVertices++;
    }

    // Particle 
    HepMC::ConstGenEventParticleRange particle_range = subEvent->particle_range();
    for(auto pit : particle_range) {
      const HepMC::GenParticle *particle = pit;
      const HepMC::FourVector momentum = particle->momentum();
      m_Truth_particlePt->push_back(momentum.perp());
      m_Truth_particleEta->push_back(momentum.eta());
      m_Truth_particlePhi->push_back(momentum.phi());
      m_Truth_particleE->push_back(momentum.e());
      m_Truth_particleM->push_back(momentum.m());
      m_Truth_particlePdg_id->push_back(particle->pdg_id());
      m_Truth_particleStatus->push_back(particle->status());
      m_Truth_particleBarcode->push_back(particle->barcode());
      HepMC::GenVertex* production_vertex = particle->production_vertex();
      m_Truth_particleProduction_vertex_id->push_back(production_vertex ? production_vertex->id() : -1);
      HepMC::GenVertex* end_vertex = particle->end_vertex();
      m_Truth_particleEnd_vertex_id->push_back(end_vertex ? end_vertex->id() : -1);
      m_Truth_nParticles++;
    }

    nSubEvents++;
  }

  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode TruthVariables::clearVariables()
{

  m_Truth_nVertices = 0;
  m_Truth_vertexX->clear();
  m_Truth_vertexY->clear();
  m_Truth_vertexZ->clear();
  m_Truth_vertexT->clear();
  m_Truth_vertexId->clear();
  m_Truth_nParticles = 0;
  m_Truth_particlePt->clear();
  m_Truth_particleEta->clear();
  m_Truth_particlePhi->clear();
  m_Truth_particleE->clear();
  m_Truth_particleM->clear();
  m_Truth_particlePdg_id->clear();
  m_Truth_particleStatus->clear();
  m_Truth_particleBarcode->clear();
  m_Truth_particleProduction_vertex_id->clear();
  m_Truth_particleEnd_vertex_id->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode TruthVariables::initializeVariables()
{

  m_Truth_nVertices = 0;
  m_Truth_vertexX = new std::vector<double>;
  m_Truth_vertexY = new std::vector<double>;
  m_Truth_vertexZ = new std::vector<double>;
  m_Truth_vertexT = new std::vector<double>;
  m_Truth_vertexId = new std::vector<int>;
  m_Truth_nParticles = 0;
  m_Truth_particlePt = new std::vector<double>;
  m_Truth_particleEta = new std::vector<double>;
  m_Truth_particlePhi = new std::vector<double>;
  m_Truth_particleE = new std::vector<double>;
  m_Truth_particleM = new std::vector<double>;
  m_Truth_particlePdg_id = new std::vector<int>;
  m_Truth_particleStatus = new std::vector<int>;
  m_Truth_particleBarcode = new std::vector<int>;
  m_Truth_particleProduction_vertex_id = new std::vector<int>;
  m_Truth_particleEnd_vertex_id = new std::vector<int>;
  if(m_tree) {
    m_tree->Branch("TruthVertex_n", &m_Truth_nVertices, "TruthVertex_n/i");
    m_tree->Branch("TruthVertex_X", &m_Truth_vertexX);
    m_tree->Branch("TruthVertex_Y", &m_Truth_vertexY);
    m_tree->Branch("TruthVertex_Z", &m_Truth_vertexZ);    
    m_tree->Branch("TruthVertex_T", &m_Truth_vertexT);    
    m_tree->Branch("TruthVertex_Id", &m_Truth_vertexId);    
    m_tree->Branch("TruthParticle_n", &m_Truth_nParticles, "TruthParticle_n/i");
    m_tree->Branch("TruthParticle_Pt", &m_Truth_particlePt);
    m_tree->Branch("TruthParticle_Eta", &m_Truth_particleEta);
    m_tree->Branch("TruthParticle_Phi", &m_Truth_particlePhi);
    m_tree->Branch("TruthParticle_E", &m_Truth_particleE);
    m_tree->Branch("TruthParticle_M", &m_Truth_particleM);
    m_tree->Branch("TruthParticle_Pdg", &m_Truth_particlePdg_id);
    m_tree->Branch("TruthParticle_Status", &m_Truth_particleStatus);
    m_tree->Branch("TruthParticle_Barcode", &m_Truth_particleBarcode);
    m_tree->Branch("TruthParticle_Production_vertex_id", &m_Truth_particleProduction_vertex_id);
    m_tree->Branch("TruthParticle_End_vertex_id", &m_Truth_particleEnd_vertex_id);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called in the destructor */
void TruthVariables::deleteVariables()
{
  m_Truth_nVertices = 0;

  delete m_Truth_vertexX;
  delete m_Truth_vertexY;
  delete m_Truth_vertexZ;
  delete m_Truth_vertexT;
  delete m_Truth_vertexId;

  m_Truth_vertexX = nullptr;
  m_Truth_vertexY = nullptr;
  m_Truth_vertexZ = nullptr;
  m_Truth_vertexT = nullptr;
  m_Truth_vertexId = nullptr;

  m_Truth_nParticles = 0;

  delete m_Truth_particlePt;
  delete m_Truth_particleEta;
  delete m_Truth_particlePhi;
  delete m_Truth_particleE;
  delete m_Truth_particleM;
  delete m_Truth_particlePdg_id;
  delete m_Truth_particleStatus;
  delete m_Truth_particleBarcode;
  delete m_Truth_particleProduction_vertex_id;
  delete m_Truth_particleEnd_vertex_id;
  
  m_Truth_particlePt = nullptr;
  m_Truth_particleEta = nullptr;
  m_Truth_particlePhi = nullptr;
  m_Truth_particleE = nullptr;
  m_Truth_particleM = nullptr;
  m_Truth_particlePdg_id = nullptr;
  m_Truth_particleStatus = nullptr;
  m_Truth_particleBarcode = nullptr;
  m_Truth_particleProduction_vertex_id = nullptr;
  m_Truth_particleEnd_vertex_id = nullptr;

  return;
}
