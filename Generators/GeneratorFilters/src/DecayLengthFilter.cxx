/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DecayLengthFilter.h"
#include "HepPDT/TableBuilder.hh"
#include <fstream>


DecayLengthFilter::DecayLengthFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("McEventCollection", m_McEventCollectionName = "GEN_EVENT"); //"TruthAOD"
  declareProperty("Rmin", m_Rmin = 150 );
  declareProperty("Rmax", m_Rmax = 200 );
  declareProperty("Zmin", m_Zmin = 300 );
  declareProperty("Zmax", m_Zmax = 400 );
  declareProperty("particle_id", m_particle_id = 1000022 ); //pdg_id of the decaying particle, default is Neutralino
}


StatusCode DecayLengthFilter::filterInitialize() {
  ATH_MSG_INFO("Configuration for desiered decay region: R=" << m_Rmin << "-" << m_Rmax << ", Z=" << m_Zmin << "-" << m_Zmax);
  return StatusCode::SUCCESS;
}


StatusCode DecayLengthFilter::filterEvent() {
  const McEventCollection* m_McEventCollection = 0;
  CHECK(evtStore()->retrieve( m_McEventCollection, m_McEventCollectionName));

  const HepMC::GenEvent* theGenEvent = *(m_McEventCollection->begin()); //supose ther is only one, no pile-up
  HepMC::GenEvent::vertex_const_iterator vtx_iter = theGenEvent->vertices_begin();
  HepMC::GenEvent::vertex_const_iterator vtx_end = theGenEvent->vertices_end();

  setFilterPassed(false);

  //First, find special vertices
  int nFound(0);
  for(; vtx_iter!=vtx_end; ++vtx_iter) {

    // now look for displaced vertices
    if((*vtx_iter)->particles_in_size()!=1) continue; // chi decays have only one in particle (the chi)
    if((*vtx_iter)->particles_out_size()<2) continue; // we don't want replication entries
    if((*vtx_iter)->point3d().x()==0 && (*vtx_iter)->point3d().y()==0 && (*vtx_iter)->point3d().z()==0) continue;

    HepMC::GenVertex::particles_in_const_iterator inParticle = (*vtx_iter)->particles_in_const_begin();
    if((*inParticle)->pdg_id()!=m_particle_id) continue;

    nFound++;
    //std::cout << "found the chi " << nFound << std::endl;

    if(nFound>2) {
      ATH_MSG_WARNING("More than 2 displaced vertices found !!!"); //should never happen
      break;
    }

    HepMC::GenVertex* decayVertex = *vtx_iter;
    HepMC::GenVertex* creationVertex = (*inParticle)->production_vertex();

    float distR = sqrt( pow(creationVertex->point3d().x() - decayVertex->point3d().x(),2) + pow(creationVertex->point3d().y() - decayVertex->point3d().y(),2) );
    float distZ = creationVertex->point3d().z() - decayVertex->point3d().z();
    ATH_MSG_DEBUG("distR " << distR << ", distZ " << distZ);

    //accept all events with at least one of the two decays in the orimeter
    if( isAccepted( distR, distZ) )
      setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}


bool DecayLengthFilter::isAccepted(float distR, float distZ) {
  return ( (fabs(distZ)>m_Zmin && fabs(distZ)<m_Zmax && distR<m_Rmax) ||  // endcaps
           (distR>m_Rmin && distR<m_Rmax && fabs(distZ)<m_Zmax) );  // barrel
}
