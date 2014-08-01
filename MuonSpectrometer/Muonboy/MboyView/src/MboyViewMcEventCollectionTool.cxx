/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TruthHelper/IsGenStable.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "HepPDT/ParticleData.hh"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewMcEventCollectionTool.h"

using namespace TruthHelper;

static const InterfaceID IID_IMboyViewMcEventCollectionTool("MboyViewMcEventCollectionTool", 1, 0);

const InterfaceID& MboyViewMcEventCollectionTool::interfaceID()
{
  return IID_IMboyViewMcEventCollectionTool;
}

MboyViewMcEventCollectionTool::MboyViewMcEventCollectionTool(const std::string& t, 
							     const std::string& n,
							     const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewMcEventCollectionTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  const HepPDT::ParticleDataTable m_particleDataTable("PDG Table");

}

MboyViewMcEventCollectionTool::~MboyViewMcEventCollectionTool(){}

// Initialize
StatusCode MboyViewMcEventCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO(  "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  if (StatusCode::SUCCESS != service("PartPropSvc", partPropSvc, true)) {
    ATH_MSG_FATAL( " Could not initialize Particle Properties Service" ) ;
    return StatusCode::FAILURE;
  }      
  m_particleDataTable = partPropSvc->PDT();


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewMcEventCollectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewMcEventCollectionTool::ViewMcEventCollection(
								std::string McEventCollectionLocation,
								std::ofstream* pOut,
								float& PtMinID
								){

  //Access
  if (!(evtStore()->contains<McEventCollection>(McEventCollectionLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << McEventCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }
  const McEventCollection* pMcEventCollection ;
  StatusCode sc = evtStore()->retrieve(pMcEventCollection, McEventCollectionLocation);
  if (sc.isFailure()){
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO(  "pMcEventCollection not found at " << McEventCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  //Dump
  // Loop over all events in McEventCollection
  for (McEventCollection::const_iterator it = pMcEventCollection->begin(); it!=pMcEventCollection->end(); ++it){
    const HepMC::GenEvent* event=(*it);
    HepMC::GenEvent::particle_const_iterator particle;
    for (particle = event->particles_begin(); particle != event->particles_end(); ++particle )
      {
      
	if ( (*particle)->momentum().perp() < PtMinID  ) continue;

	HepMC::GenVertex* verto = (**particle).production_vertex();
	HepMC::GenVertex* verte = (**particle).end_vertex();
	if ( ! verto ) continue;

	int p_pdg_id=(*particle)->pdg_id();
	double p_px=(*particle)->momentum().px();
	double p_py=(*particle)->momentum().py();
	double p_pz=(*particle)->momentum().pz();
      
	double vo_x=0;
	double vo_y=0;
	double vo_z=0;
	if ( verto ) {
	  vo_x=verto->position().x();
	  vo_y=verto->position().y();
	  vo_z=verto->position().z();
	}
	double ve_x=0.;
	double ve_y=0.;
	double ve_z=0.;
	if ( verte ) {
	  ve_x=verte->position().x();
	  ve_y=verte->position().y();
	  ve_z=verte->position().z();
	}

      
      
	// "Stolen" from /atlas/Generators/GenAnalysisTools/TruthTools/src/TruthSelector.cxx
	// G4 equivalent: use helper to select particle in final state.
	// Temporary cut at 0.1mm for non-final state particles
	// while Truth helpers get sorted
      
      
	IsGenStable isGenStable;
	if ( fabs(verto->position().perp()) < 0.1*CLHEP::mm  && 
	     ! isGenStable(*particle) )         continue;      
      
	// charged particle
	double charge = 0;
	const HepPDT::ParticleData*	particleData	= m_particleDataTable->particle(abs(p_pdg_id));
	if (particleData != 0 )
	  charge = p_pdg_id > 0 ? particleData->charge() : - particleData->charge();
      
	*pOut << "TRK" << " "
	      << "McEventCollection " << " "
	      << McEventCollectionLocation << " "
	      << " dummy " << " "
	      << std::setw(12)<<std::setprecision(3) <<vo_x      << " "
	      << std::setw(12)<<std::setprecision(3) <<vo_y      << " "
	      << std::setw(12)<<std::setprecision(3) <<vo_z      << " "
	      << std::setw(12)<<std::setprecision(3) <<ve_x      << " "
	      << std::setw(12)<<std::setprecision(3) <<ve_y      << " "
	      << std::setw(12)<<std::setprecision(3) <<ve_z      << " "
	      << std::setw(12)<<std::setprecision(3) <<p_px     << " "
	      << std::setw(12)<<std::setprecision(3) <<p_py     << " "
	      << std::setw(12)<<std::setprecision(3) <<p_pz     << " "
	      << std::setw(3)<<std::setprecision(0) <<charge   << " "
	      << std::setw(8)<<std::setprecision(3) <<p_pdg_id << " "
	      << std::endl;
      }
    
  }

  return StatusCode::SUCCESS;

}

