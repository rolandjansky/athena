/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthTrackGetterTool.h"

#include "GaudiKernel/IPartPropSvc.h"

namespace D3PD {

TruthTrackGetterTool::TruthTrackGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_particleDataTable(0)
{
  declareProperty("KeepOnlyPrimaryParticles", m_keepOnlyPrimaryParticles = false);
  declareProperty("KeepOnlyChargedParticles", m_keepOnlyChargedParticles = true);
  declareProperty("KeepV0", m_keepV0 = true);
  declareProperty("TrueTrackPtCut", m_trueTrackPtCut = 1000.);
  declareProperty("TrueTrackEtaCut", m_trueTrackEtaCut = 3.);
  declareProperty("MaxRAll", m_maxRAll = 360);
  declareProperty("MaxZAll", m_maxZAll = 2000);
  declareProperty("MaxRPrimary", m_maxRPrimary = 25);
  declareProperty("MaxZPrimary", m_maxZPrimary = 200);
}

StatusCode TruthTrackGetterTool::initialize()
{
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

const std::type_info& TruthTrackGetterTool::typeinfo() const
{
  return typeid(McEventCollection);
}


const std::type_info& TruthTrackGetterTool::elementTypeinfo() const
{
  return typeid(HepMC::GenParticle);
}


size_t TruthTrackGetterTool::sizeHint(bool allowMissing)
{
  // fix me...
  const McEventCollection* mcTracks = get(allowMissing);

  if (!mcTracks) {
    return 0;
  }

  const HepMC::GenEvent* genEvent = *(mcTracks->begin());

  return genEvent->particles_size();
}


StatusCode TruthTrackGetterTool::reset(bool allowMissing)
{

  const McEventCollection* mcEvent = get(allowMissing);

  if (!mcEvent) {
    m_evtItr = m_evtEnd;
    m_partItr = m_partEnd;
    return StatusCode::FAILURE;
  }

  m_evtItr = mcEvent->begin();
  m_evtEnd = mcEvent->end();

  if(m_evtItr == m_evtEnd){
    m_partItr = m_partEnd;
    return StatusCode::SUCCESS;
  }

  m_partItr = (*m_evtItr)->particles_begin();
  m_partEnd = (*m_evtItr)->particles_end();

  return StatusCode::SUCCESS;
}

const void* TruthTrackGetterTool::nextUntyped()
{

  const HepMC::GenParticle* particle = 0;

  do{

    if(m_evtItr == m_evtEnd){
      return 0;
    }

    if (m_partItr == m_partEnd)
      particle = 0;
    else {
      particle = *m_partItr;
      m_partItr++;
    }

    if(m_partItr == m_partEnd){
      m_evtItr++;
      if(m_evtItr != m_evtEnd){
	m_partItr = (*m_evtItr)->particles_begin();
	m_partEnd = (*m_evtItr)->particles_end();
      }
    }

  } while(!particle || !selectParticle(particle));

  return particle;

}

bool TruthTrackGetterTool::selectParticle(const  HepMC::GenParticle* particle)
{

  // stables only
  if(1!=particle->status()%1000){
    return false;
  }

  // must have a valid production vertex
  if(particle->production_vertex() == NULL){
    //REPORT_MESSAGE (MSG::DEBUG) << "GenParticle has NULL Production Vertex";
    return false;
  }

  // require track origin inside ID: relaxed
  if ( fabs(particle->production_vertex()->point3d().perp()) > m_maxRAll ||
       fabs(particle->production_vertex()->point3d().z())    > m_maxZAll ){
    return false;
  }

  // require track origin inside beam pipe (primary): optional
  if ( m_keepOnlyPrimaryParticles &&
       ( fabs(particle->production_vertex()->point3d().perp()) > m_maxRPrimary ||
	 fabs(particle->production_vertex()->point3d().z())    > m_maxZPrimary ) ){
    return false;
  }

  int pdg = particle->pdg_id();

  // ignore nuclei from hadronic interactions
  if (abs(pdg) > 1000000000 ){
    return false;
  }

  const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdg));

  if (!pd) {
    //REPORT_MESSAGE (MSG::DEBUG) << "Could not get particle data for pdg = " << pdg;
    return false;
  }
  float charge = pd->charge();

  // require charge: optional
  if(fabs(charge)<0.5 && m_keepOnlyChargedParticles){
    // keep V0: optional
    if(m_keepV0){
      HepMC::GenVertex *epv = particle->end_vertex();
      if( !epv || epv->particles_out_size() != 2 ) {
	return false;
      }
    }else{
      return false;
    }
  }

  ////// particle properties
  double pt = particle->momentum().perp();
  double eta = particle->momentum().pseudoRapidity();

  // pt cut
  if(fabs(pt)<m_trueTrackPtCut){
    return false;
  }
    
  // eta cut
  if(fabs(eta)>m_trueTrackEtaCut){
    return false;
  }

  return true;
}

} // namespace D3PD
