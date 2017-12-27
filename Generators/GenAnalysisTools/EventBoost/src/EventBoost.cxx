/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////
//                                             //
//  File:  EventBoost.cxx                      //
//                                             //
//  Base class for algorithms designed to      //
//  analyse truth events (GenEvents).          //
//                                             //
//  Author: Esben Klinkby <klinkby@nbi.dk>     //
//                                             //
/////////////////////////////////////////////////

#include "EventBoost/EventBoost.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "StoreGate/StoreGateSvc.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <algorithm>

//______________________________________________________________________________

EventBoost::EventBoost( const std::string& name,
			ISvcLocator* pSvcLocator ) 
  :  AthAlgorithm(name, pSvcLocator), m_nModifiedEvent(0), m_nFailedEvent(0), m_nModifiedTotal(0), m_nFailedTotal(0),
     m_flat_rand_x(0.), m_flat_rand_y(0.), m_flat_rand_z(0.), m_gauss_rand_x(0.), m_gauss_rand_y(0.), m_gauss_rand_z(0.), m_pxsum(0.)
{
  declareProperty("BeamInclination", m_beam_inclination );
  declareProperty("GenEvtInputKey", m_genEvtInKey );
  declareProperty("GenEvtOutputKey", m_genEvtOutKey );
  declareProperty("DoGaussianVertexSmearing", m_gaussian_vertex_smearing );
  declareProperty("DoFlatVertexSmearing", m_flat_vertex_smearing );
  declareProperty("GaussianMean", m_gaussian_mean );
  declareProperty("GaussianWidth", m_gaussian_width );
  declareProperty("FlatSmearingBoundaryMin", m_flat_smearing_boundary_min );
  declareProperty("FlatSmearingBoundaryMax", m_flat_smearing_boundary_max );
}


//______________________________________________________________________________
StatusCode EventBoost::initialize(){

  msg(MSG::INFO) << "Base Class initializing" << endmsg;

  //Call user job initialization:
  return GenAnalysis_initialize();
}

//______________________________________________________________________________
StatusCode EventBoost::execute() {


  //  MsgStream msglog(messageService(), name());
  msg(MSG::VERBOSE) << "Begin execute()" << endmsg;


  // Read Data from Transient Store
  const McEventCollection* mcCollptr;
  if ( sgSvc()->retrieve(mcCollptr, m_genEvtInKey).isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve McEventCollection" << m_genEvtInKey<< endmsg;
    return StatusCode::FAILURE;
  }


  //Call user event initialization:
  if (Analyse_BeginEvent().isFailure())
    return StatusCode::FAILURE;


  McEventCollection::const_iterator it;

  for(it=mcCollptr->begin(); it!=mcCollptr->end(); it++) {

    const HepMC::GenEvent* genEvt = (*it);
    EventCopy(genEvt);

  }

  
  const McEventCollection* mcCollptrCOPY;
  if ( sgSvc()->retrieve(mcCollptrCOPY, m_genEvtOutKey).isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve boosted McEventCollection" << m_genEvtOutKey<< endmsg;
    return StatusCode::FAILURE;
  }


  for(it=mcCollptrCOPY->begin(); it!=mcCollptrCOPY->end(); it++) {

    const HepMC::GenEvent* newEvt = (*it);

    //Call user analysis for this genevent:
    if (AnalyseGenEvent(newEvt).isFailure())
      return StatusCode::FAILURE;
  }

  

  //Call user event finalization:
  if (Analyse_EndEvent().isFailure())
    return StatusCode::FAILURE;


  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
StatusCode EventBoost::finalize() {

  //Call user job finalization:
  return GenAnalysis_finalize();

}
//______________________________________________________________________________

StatusCode EventBoost::Analyse_BeginEvent() {
  m_nModifiedEvent = 0;
  m_nFailedEvent = 0;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________



StatusCode EventBoost::Analyse_EndEvent() {
  m_nModifiedTotal += m_nModifiedEvent;
  m_nFailedTotal += m_nFailedEvent;
  if ( m_nModifiedEvent+m_nFailedEvent == 0)
    msg(MSG::VERBOSE) << "No particles requiring modifications found in event." << endmsg;
  else
    msg(MSG::DEBUG) << "Modified a total of "
	   <<m_nModifiedEvent<<" particles ("
	   <<m_nFailedEvent<<" failures)."<< endmsg;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode EventBoost::GenAnalysis_initialize(){

  msg(MSG::INFO) << "EventBoost initializing" << endmsg;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode EventBoost::GenAnalysis_finalize(){

  if ( m_nModifiedTotal+m_nFailedTotal == 0)
    msg(MSG::WARNING) << "No particles requiring modifications found during entire job." << endmsg;
  else
    msg(MSG::INFO) << "Modified a total of "
	   <<m_nModifiedTotal<<" particles ("
	   <<m_nFailedTotal<<" failures) during job."<< endmsg;
   return StatusCode::SUCCESS;
}

//______________________________________________________________________________

StatusCode EventBoost::AnalyseGenEvent(const HepMC::GenEvent* genEvt) {

  msg(MSG::VERBOSE) << "EventBoost begin AnalyseGenEvent()" << endmsg;

  HepMC::GenEvent::particle_const_iterator p = genEvt->particles_begin();
  HepMC::GenEvent::particle_const_iterator pEnd = genEvt->particles_end();

  std::vector<HepMC::GenParticle*> particles_needing_modification;

  for(; p != pEnd; ++p ) {
    particles_needing_modification.push_back(*p);
  }

  std::vector<HepMC::GenParticle*>::const_iterator it = particles_needing_modification.begin();
  std::vector<HepMC::GenParticle*>::const_iterator itE = particles_needing_modification.end();

  m_pxsum=0.;

  for (;it!=itE;++it) {
    if (!doModification(*it,m_pxsum)) {
      msg(MSG::WARNING) << "Problems modifying HepMC record!" << endmsg;
      ++m_nFailedEvent;
    } else
      ++m_nModifiedEvent;
  }
  msg(MSG::DEBUG) << "Difference in total Px momentum = " << m_pxsum << " MeV" <<endmsg;

  
  if ((m_gaussian_vertex_smearing)||(m_flat_vertex_smearing)) {

    HepMC::GenEvent::vertex_const_iterator v = genEvt->vertices_begin();
    HepMC::GenEvent::vertex_const_iterator vEnd = genEvt->vertices_end();

    std::vector<HepMC::GenVertex*> vertices_needing_modification;
    
    for(; v != vEnd; ++v ) {
      vertices_needing_modification.push_back(*v);
    }

    std::vector<HepMC::GenVertex*>::const_iterator vit  = vertices_needing_modification.begin();
    std::vector<HepMC::GenVertex*>::const_iterator vitE = vertices_needing_modification.end();


    Rndm::Numbers GaussVertexModifier_x(randSvc(), Rndm::Gauss(m_gaussian_mean[0],m_gaussian_width[0]));
    Rndm::Numbers GaussVertexModifier_y(randSvc(), Rndm::Gauss(m_gaussian_mean[1],m_gaussian_width[1]));
    Rndm::Numbers GaussVertexModifier_z(randSvc(), Rndm::Gauss(m_gaussian_mean[2],m_gaussian_width[2]));
    
    Rndm::Numbers FlatVertexModifier_x(randSvc(), Rndm::Flat(m_flat_smearing_boundary_min[0], m_flat_smearing_boundary_max[0]));
    Rndm::Numbers FlatVertexModifier_y(randSvc(), Rndm::Flat(m_flat_smearing_boundary_min[1], m_flat_smearing_boundary_max[1]));
    Rndm::Numbers FlatVertexModifier_z(randSvc(), Rndm::Flat(m_flat_smearing_boundary_min[2], m_flat_smearing_boundary_max[2]));
    
    if ((m_gaussian_vertex_smearing)&&(m_flat_vertex_smearing)) {
      msg(MSG::ERROR) << "Bad input settings: cannot smear according to multiple distributions: Gauss & Flat" << endmsg;
      return StatusCode::FAILURE;
    }

  
    //For each GenEvent, all vertices are shifted by the same amount
    m_gauss_rand_x = GaussVertexModifier_x.shoot();
    m_gauss_rand_y = GaussVertexModifier_y.shoot();
    m_gauss_rand_z = GaussVertexModifier_z.shoot();

    m_flat_rand_x = FlatVertexModifier_x.shoot();
    m_flat_rand_y = FlatVertexModifier_y.shoot();
    m_flat_rand_z = FlatVertexModifier_z.shoot();


    for (;vit!=vitE;++vit) {

      if (m_gaussian_vertex_smearing) {
	if (!doVertexModification(*vit, m_gauss_rand_x, m_gauss_rand_y, m_gauss_rand_z)) {
	  msg(MSG::WARNING) << "Problems modifying HepMC record!" << endmsg;
	  ++m_nFailedEvent;
	} else
	  ++m_nModifiedEvent;
      }

      else if (m_flat_vertex_smearing) {
	if (!doVertexModification(*vit, m_flat_rand_x, m_flat_rand_y, m_flat_rand_z)) {
	  msg(MSG::WARNING) << "Problems modifying HepMC record!" << endmsg;
	  ++m_nFailedEvent;
	} else
	  ++m_nModifiedEvent;
      }
      

    }
  }

  
  return StatusCode::SUCCESS;
}


//__________________________________________________________________________


bool EventBoost::doModification(HepMC::GenParticle * part, double& m_pxsum ) {

  /*    
  From px' = gamma_x*px+beta_x*gamma_x*E
  and px=0, one gets: beta_x^2 = 1/((E/px')^2+1) or app beta_x = px'/E
  from pythagoras in the px',pz plane one gets with alpha = beam_inclination_angle:
  sin(px'/pz)=alpha or app px'=alpha*pz.
  I.e. beta_x = alpha*pz/E = alpha (unitless)
  */
  
  double boost_x =  m_beam_inclination*(1./(1000000.)); //conversion from micro rad to unitless
  CLHEP::Hep3Vector boostvector(boost_x,0.0,0.0);
  
  CLHEP::HepLorentzVector mom(part->momentum().px(),
		       part->momentum().py(),
		       part->momentum().pz(),
		       part->momentum().e());
  double temp=mom.x();
  
  mom.boost(boostvector);
  part->set_momentum(mom);
  if (part->status()==1) {
    m_pxsum+=mom.x()-temp;
  }

  return true;
}




//__________________________________________________________________________


bool EventBoost::doVertexModification(HepMC::GenVertex * ver, double rand_x, double rand_y, double rand_z ) {

  CLHEP::HepLorentzVector vertex(ver->position().x(),
			  ver->position().y(),
			  ver->position().z(),
			  ver->position().t());
  
  CLHEP::HepLorentzVector vertex_offset(rand_x, rand_y, rand_z);
  CLHEP::HepLorentzVector new_vertex_pos(vertex.x()+vertex_offset.x(), vertex.y()+vertex_offset.y(), vertex.z()+vertex_offset.z());
  
  ver->set_position(new_vertex_pos);

  msg(MSG::DEBUG) << "Vertex position modified from ("<<vertex.x()<<", "<<vertex.y()<<", "<<vertex.z()<<") to ("<<new_vertex_pos.x()<<", "<<new_vertex_pos.y()<<", "<<new_vertex_pos.z()<<")" << endmsg;
  
  
  return true;
}


//_______________________________________________________________________

StatusCode EventBoost::EventCopy(const HepMC::GenEvent* evt) const
{
  
  HepMC::GenEvent* newEvt=new HepMC::GenEvent(*evt);
  
  McEventCollection* mcEvtColl;
  mcEvtColl = new McEventCollection;
  mcEvtColl->push_back(newEvt);
  
  if ( sgSvc()->record( mcEvtColl, m_genEvtOutKey ).isFailure() ) {
    msg(MSG::ERROR) << "Could not record boosted McEventCollection" << m_genEvtOutKey<< endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}
