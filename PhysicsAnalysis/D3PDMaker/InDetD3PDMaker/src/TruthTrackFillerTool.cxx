/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#include "TruthTrackFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

namespace D3PD {

TruthTrackFillerTool::TruthTrackFillerTool (const std::string& type,
					    const std::string& name,
					    const IInterface* parent)
  : Base (type, name, parent),
    m_particleDataTable(0)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode TruthTrackFillerTool::initialize(){

  CHECK( Base::initialize() );

  // Get Particle Data Table
  IPartPropSvc* partPropSvc;
  StatusCode sc = service("PartPropSvc", partPropSvc, true);
  if(sc.isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not initialize Particle Properties Service";
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  return StatusCode::SUCCESS;
}

StatusCode TruthTrackFillerTool::book()
{
  // truth id
  CHECK( addVariable ("charge", m_charge) );
  CHECK( addVariable ("child_n", m_child_n) );
  CHECK( addVariable ("child_barcode", m_child_barcode) );
  CHECK( addVariable ("child_pdg", m_child_pdg) );
  // origin vertex
  CHECK( addVariable ("begVtx_x", m_begVtx_x) );
  CHECK( addVariable ("begVtx_y", m_begVtx_y) );
  CHECK( addVariable ("begVtx_z", m_begVtx_z) );
  CHECK( addVariable ("begVtx_barcode", m_begVtx_barcode) );
  // end vertex
  CHECK( addVariable ("endVtx_x", m_endVtx_x) );
  CHECK( addVariable ("endVtx_y", m_endVtx_y) );
  CHECK( addVariable ("endVtx_z", m_endVtx_z) );
  CHECK( addVariable ("endVtx_barcode", m_endVtx_barcode) );


  return StatusCode::SUCCESS;
}

StatusCode TruthTrackFillerTool::fill (const HepMC::GenParticle& particle)
{

  this->clearData();

  // origin
  HepMC::GenVertex *pv = particle.production_vertex();
  if(!pv){
    //REPORT_MESSAGE (MSG::DEBUG) << "Pointer to production vertex is NULL";
  }else{
    *m_begVtx_x = pv->position().x();
    *m_begVtx_y = pv->position().y();
    *m_begVtx_z = pv->position().z();
    *m_begVtx_barcode = pv->barcode();
  }

  int pdg = particle.pdg_id();
  const HepPDT::ParticleData* pdata = m_particleDataTable->particle(HepPDT::ParticleID(abs(pdg)));
  if(pdata) {
    int sign = pdg/abs(pdg);
    *m_charge = sign*pdata->charge();
  } else {
    //REPORT_MESSAGE (MSG::DEBUG) << "Pointer to particle data is NULL";
  }

  // end
  HepMC::GenVertex *epv = particle.end_vertex();
  if(!epv){
    //REPORT_MESSAGE (MSG::DEBUG) << "Pointer to end vertex is NULL";
  }else{
    *m_endVtx_x = epv->position().x();
    *m_endVtx_y = epv->position().y();
    *m_endVtx_z = epv->position().z();
    *m_endVtx_barcode = epv->barcode();
    *m_child_n = epv->particles_out_size();
    HepMC::GenVertex::particles_out_const_iterator itr = epv->particles_out_const_begin();
    HepMC::GenVertex::particles_out_const_iterator end = epv->particles_out_const_end();
    for(; itr!=end; itr++){
      m_child_barcode->push_back((*itr)->barcode());
      m_child_pdg->push_back((*itr)->pdg_id());
    }
  }

  return StatusCode::SUCCESS;
}

void TruthTrackFillerTool::clearData(){

  *m_charge = 1;
  *m_child_n = 0;
  m_child_barcode->clear();
  m_child_pdg->clear();
  *m_begVtx_x = 0;
  *m_begVtx_y = 0;
  *m_begVtx_z = 0;
  *m_endVtx_x = 0;
  *m_endVtx_y = 0;
  *m_endVtx_z = 0;
}

} // namespace MinBiasD3PD
