/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/McTruthNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "GeneratorObjects/McEventCollection.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <map>

namespace NtComponent {
  
  //---------------------------------------------------------------------

  McTruthNtComponent::McTruthNtComponent(NtupleAlgorithm *mainAlg,
					 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  McTruthNtComponent::~McTruthNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode McTruthNtComponent::setupServices(void) {
    StatusCode sc;

    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;

    // get the Particle Properties Service
    IPartPropSvc* partPropSvc = 0;
    sc =  m_mainAlg->service("PartPropSvc", partPropSvc, true);
    if (sc.isFailure()) {
      (*m_msg) << MSG::FATAL << " Could not initialize Particle Properties Service" << endreq;
      return StatusCode::FAILURE;
    } 

    // Get a pointer to the particle data table
    m_particleDataTable = partPropSvc->PDT();

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("McEventCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_truthCollectionKey = "TruthEvent"; // The default when no key is found.
    }
    else {
      m_truthCollectionKey = (*itr).second; // Use the setting.
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode McTruthNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_truth_evt_weights = new std::vector<double>();
    m_truth_part_px = new std::vector<float>();
    m_truth_part_py = new std::vector<float>();
    m_truth_part_pz = new std::vector<float>();
    m_truth_part_e = new std::vector<float>();
    m_truth_part_status = new std::vector<long>();
    m_truth_part_pdgId = new std::vector<long>();
    m_truth_part_barcode = new std::vector<long>();
    m_truth_part_charge = new std::vector<float>();
    m_truth_part_prod_vtx = new std::vector<long>();
    m_truth_part_end_vtx = new std::vector<long>();
    m_truth_vtx_x = new std::vector<float>();
    m_truth_vtx_y = new std::vector<float>();
    m_truth_vtx_z = new std::vector<float>();
    m_truth_vtx_t = new std::vector<float>();
    m_truth_vtx_id = new std::vector<long>();
    m_truth_vtx_barcode = new std::vector<long>();

    m_tree->Branch("truth_evt_weights",  &m_truth_evt_weights);

    m_tree->Branch("truth_npart",        &m_truth_npart, "truth_npart/I");
    m_tree->Branch("truth_part_px",      &m_truth_part_px);
    m_tree->Branch("truth_part_py",      &m_truth_part_py);
    m_tree->Branch("truth_part_pz",      &m_truth_part_pz);
    m_tree->Branch("truth_part_e",       &m_truth_part_e);
    m_tree->Branch("truth_part_status",  &m_truth_part_status);
    m_tree->Branch("truth_part_pdgId",   &m_truth_part_pdgId);
    m_tree->Branch("truth_part_barcode", &m_truth_part_barcode);
    m_tree->Branch("truth_part_charge",  &m_truth_part_charge);
    m_tree->Branch("truth_part_prod_vtx",&m_truth_part_prod_vtx);
    m_tree->Branch("truth_part_end_vtx", &m_truth_part_end_vtx);
    
    m_tree->Branch("truth_nvtx",         &m_truth_nvtx, "truth_nvtx/I");
    m_tree->Branch("truth_vtx_x",        &m_truth_vtx_x);
    m_tree->Branch("truth_vtx_y",        &m_truth_vtx_y);
    m_tree->Branch("truth_vtx_z",        &m_truth_vtx_z);
    m_tree->Branch("truth_vtx_t",        &m_truth_vtx_t);
    m_tree->Branch("truth_vtx_id",       &m_truth_vtx_id);
    m_tree->Branch("truth_vtx_barcode",  &m_truth_vtx_barcode);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode McTruthNtComponent::fill(void) {
    StatusCode sc;

    // Initialise
    m_truth_npart = 0;
    m_truth_nvtx = 0;

    // Clear the ntuple vectors
    m_truth_part_px->clear(); 
    m_truth_part_py->clear();
    m_truth_part_pz->clear();
    m_truth_part_e->clear();
    m_truth_part_status->clear();
    m_truth_part_pdgId->clear();
    m_truth_part_barcode->clear();
    m_truth_part_charge->clear();
    m_truth_part_prod_vtx->clear();
    m_truth_part_end_vtx->clear();
    m_truth_vtx_x->clear();
    m_truth_vtx_y->clear();
    m_truth_vtx_z->clear();
    m_truth_vtx_t->clear();
    m_truth_vtx_id->clear();
    m_truth_vtx_barcode->clear();

    const McEventCollection *mcEventCollection=0;

    sc=(*m_evtStore)->retrieve(mcEventCollection, m_truthCollectionKey);
    if( sc.isFailure()  || !mcEventCollection ) {
       if (!m_warningPrinted) {
	 (*m_msg) << MSG::WARNING << m_truthCollectionKey <<  " requested but not found." << endreq;
	 m_warningPrinted = true;
       }
      return StatusCode::SUCCESS;
    }

    // Loop over MC GenParticles contained in McEventCollection
    McEventCollection::const_iterator mcCollection_itr = mcEventCollection->begin();
    McEventCollection::const_iterator mcCollection_itr_end = mcEventCollection->end();
    HepMC::GenEvent::particle_const_iterator hepmc_part_itr;
    HepMC::GenEvent::vertex_const_iterator hepmc_vtx_itr;
    

    size_t iweight;
    long ipart = 0, ivtx = 0; 

  

    // Loop over the McEventCollections
    for (; mcCollection_itr != mcCollection_itr_end; mcCollection_itr++) {
      
      // Dump the event for debug
      if((*m_msg).level() <= MSG::DEBUG) {
	(*mcCollection_itr)->print();
      }
      
      // Get the weights container for this event record.
      HepMC::WeightContainer weightContainer = (*mcCollection_itr)->weights();
      m_truth_evt_weights->resize(weightContainer.size(),0.);
      for(iweight = 0; iweight < (size_t)weightContainer.size(); iweight++)
	m_truth_evt_weights->push_back(weightContainer[iweight]);

      std::map<HepMC::GenParticle *, long > particleIndexMap;
      std::map<HepMC::GenVertex *, long > vertexIndexMap;
      
      // Loop over the particles and build up the particle-index map.  
      for (hepmc_part_itr = (*mcCollection_itr)->particles_begin(); 
	   hepmc_part_itr != (*mcCollection_itr)->particles_end();
	   hepmc_part_itr++) {
	particleIndexMap.insert(std::make_pair((*hepmc_part_itr),ipart));
	ipart++;
      }
      
      // Update the total number of particles
      m_truth_npart = ipart;
      
      // Loop over the vertices and build up the vertex-index map.
      for (hepmc_vtx_itr = (*mcCollection_itr)->vertices_begin();
	   hepmc_vtx_itr != (*mcCollection_itr)->vertices_end();
	   hepmc_vtx_itr++) {
	vertexIndexMap.insert(std::make_pair((*hepmc_vtx_itr),ivtx));
	ivtx++;
      }
      
      // Update the total number of vertices
      m_truth_nvtx = ivtx;
      
      // Increase the size of the ntuple vectors
      m_truth_part_px->resize(m_truth_npart,0.);
      m_truth_part_py->resize(m_truth_npart,0.);
      m_truth_part_pz->resize(m_truth_npart,0.);
      m_truth_part_e->resize(m_truth_npart,0.);
      m_truth_part_status->resize(m_truth_npart,0);
      m_truth_part_pdgId->resize(m_truth_npart,0);
      m_truth_part_barcode->resize(m_truth_npart,0);
      m_truth_part_charge->resize(m_truth_npart,0.);
      m_truth_part_prod_vtx->resize(m_truth_npart,0);
      m_truth_part_end_vtx->resize(m_truth_npart,0);

      // Write out the event.
      
      // Write out the GenParticle objects data.
      std::map<HepMC::GenParticle *, long >::iterator part_index_itr;
      for (part_index_itr = particleIndexMap.begin(); 
	   part_index_itr != particleIndexMap.end(); 
	   part_index_itr++) {
	
	// Get the particle index
	ipart = (*part_index_itr).second;

	if((*m_msg).level() <= MSG::DEBUG) {
	  (*m_msg) << MSG::DEBUG << "Particle index = " << ipart << endreq;
	  (*m_msg) << MSG::DEBUG << "Particle barcode = " << (*part_index_itr).first->barcode() << endreq;
	}

	// Fill the particle entries
	(*m_truth_part_px)[ipart] = (*part_index_itr).first->momentum().px();
	(*m_truth_part_py)[ipart] = (*part_index_itr).first->momentum().py();
	(*m_truth_part_pz)[ipart] = (*part_index_itr).first->momentum().pz();
	(*m_truth_part_e)[ipart] = (*part_index_itr).first->momentum().e();
	(*m_truth_part_status)[ipart] = (*part_index_itr).first->status();
	(*m_truth_part_pdgId)[ipart] = (*part_index_itr).first->pdg_id();
	(*m_truth_part_barcode)[ipart] = (*part_index_itr).first->barcode();
	
	// Find the index of the production vertex
	sc = findVertexIndex (&vertexIndexMap, 
			      (*part_index_itr).first->production_vertex(),
			      &(*m_truth_part_prod_vtx)[ipart]);
	if(sc.isFailure()) return sc;
	
	if((*m_msg).level() <= MSG::DEBUG) {
	  (*m_msg) << MSG::INFO << "Production vertex = " << (*m_truth_part_prod_vtx)[ipart] << endreq;
	}

	// Find the index of the end vertex
	sc = findVertexIndex (&vertexIndexMap, 
			      (*part_index_itr).first->end_vertex(),
			      &(*m_truth_part_end_vtx)[ipart]);
	if(sc.isFailure()) return sc;

	if((*m_msg).level() <= MSG::DEBUG) {
	  (*m_msg) << MSG::DEBUG << "End vertex = " << (*m_truth_part_end_vtx)[ipart] << endreq;
	}
	
	// Record the charge if the PDG id is known
	(*m_truth_part_charge)[ipart]=-999.0;
	const HepPDT::ParticleData* particleData = m_particleDataTable->particle(HepPDT::ParticleID(abs((*part_index_itr).first->pdg_id())));
	if(particleData) {
	  (*m_truth_part_charge)[ipart] = particleData->charge();
	  if ((*part_index_itr).first->pdg_id()<0) {
	    (*m_truth_part_charge)[ipart] = -(*m_truth_part_charge)[ipart];
	  }
	}
      }

      // Increase the size of the vtx ntuple vectors
      m_truth_vtx_x->resize(m_truth_nvtx, 0.);
      m_truth_vtx_y->resize(m_truth_nvtx, 0.);
      m_truth_vtx_z->resize(m_truth_nvtx, 0.);
      m_truth_vtx_t->resize(m_truth_nvtx, 0.);
      m_truth_vtx_id->resize(m_truth_nvtx, 0);
      m_truth_vtx_barcode->resize(m_truth_nvtx, 0);
      
      // Write out the GenVertex object data.
      std::map<HepMC::GenVertex *, long >::iterator vtx_index_itr;
      std::vector<long > attachedParticles;
      for (vtx_index_itr = vertexIndexMap.begin(); 
	   vtx_index_itr != vertexIndexMap.end(); 
	   vtx_index_itr++) {
	
	// Get the vertex index
	ivtx = (*vtx_index_itr).second;

	if((*m_msg).level() <= MSG::DEBUG) {
	  (*m_msg) << MSG::DEBUG << "Vertex index = " << ivtx << endreq;
	  (*m_msg) << MSG::DEBUG << "Vertex barcode = " << (*vtx_index_itr).first->barcode() << endreq;
	}

	// Fill the vertex entries
	(*m_truth_vtx_x)[ivtx] = (*vtx_index_itr).first->position().x();
	(*m_truth_vtx_y)[ivtx] = (*vtx_index_itr).first->position().y();
	(*m_truth_vtx_z)[ivtx] = (*vtx_index_itr).first->position().z();
	(*m_truth_vtx_t)[ivtx] = (*vtx_index_itr).first->position().t();
	(*m_truth_vtx_id)[ivtx] = (*vtx_index_itr).first->id();
	(*m_truth_vtx_barcode)[ivtx] = (*vtx_index_itr).first->barcode();
      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode McTruthNtComponent::findVertexIndex (std::map<HepMC::GenVertex *, long > *vertexIndexMap, 
						  HepMC::GenVertex *vtxPtr, long *index) {
    std::map<HepMC::GenVertex *, long >::iterator vtx_index_itr;
    
    // Test if the vertex pointer is null or not.
    if(vtxPtr) {
      
      // Find the vertex in the map
      vtx_index_itr = vertexIndexMap->find(vtxPtr);
      
      if(vtx_index_itr==vertexIndexMap->end()) {
	(*m_msg) << MSG::ERROR << "Error: the vertex does not exist in the map" << endreq;
	*index = -999;
	return StatusCode::FAILURE;
      }
      
      // Assign the index of this vertex;
      *index = (*vtx_index_itr).second;
    }
    else {
      // There is no vertex assign -1;
      *index = -1;
    }
    
    return StatusCode::SUCCESS;
  }
}
