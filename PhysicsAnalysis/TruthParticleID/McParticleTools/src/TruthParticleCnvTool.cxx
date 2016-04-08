/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleCnvTool.cxx 
// Implementation file for class TruthParticleCnvTool
// Author: S.Binet<binet@cern.ch>
//         P.A Delsart <delsart in2p3 dot fr> (pile-up compatibility)
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <sstream>

// boost includes
#include "boost/array.hpp"

// FrameWork includes
#include "GaudiKernel/IPartPropSvc.h"

// CLHEP/HepMC includes
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Polarization.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepPDT/ParticleData.hh"

// McParticleKernel includes
#include "McParticleKernel/ITruthIsolationTool.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleUtils includes
#include "McParticleUtils/McUtils.h" // for chargeFromPdgId

// McParticleTools includes
#include "TruthParticleCnvTool.h"

// GeneratorObjects includes
#include "GeneratorObjects/HepMcParticleLink.h"


/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
TruthParticleCnvTool::TruthParticleCnvTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_dataType         ( ParticleDataType::True ),
  m_vxCandidatesName ( ),
  m_pdt              ( 0 ),
  m_selectSignalTypeProp ( 0 )
{
  //
  // Property declaration
  // 
  
  declareProperty( "DataType", 
		   m_dataType_int = 4,
		   "Type of data we are dealing with (Full/Fast/Truth...)" );

  declareProperty( "McEvents", 
		   m_mcEventsName = "GEN_AOD",
		   "Name of the McEventCollection the TruthParticles will be "
		   "constructed from" );

  declareProperty( "TruthParticlesOutput", 
		   m_mcPartsOutputName = "SpclMC",
		   "Name of the output TruthParticle collection (built from the "
		   "McEventCollection)" );

  declareProperty( "TruthIsolationTool",
		   m_isolationTool = IsolTool_t( "TruthIsolationTool" ),
		   "Pointer to the TruthIsolationTool to be able to compute "
		   "transverse energy isolations for various isolation cones cuts. "
		   "See McParticleEvent/TruthParticleParameters.h for cone cuts." );


  declareProperty( "DoEtIsolations",
		   m_doEtIsolation = false,
		   "Switch to compute or not the Et-isolations for TruthParticle "
		   "(and their underlying @c HepMC::GenParticle).\n"
		   "Default is to not compute these Et-isolations (and save CPU)." );

  setDataType( m_dataType_int );

  declareProperty( "SelectSignalType",
		   m_selectSignalTypeProp,
		   "Switch to select different type of signal\n"
		   " hard scatter               0\n"
		   " hard scatter plus minbias  1\n" 
		   " only minbias              -1\n" 
		   "Default is hard scatter only." );

  declareInterface<ITruthParticleCnvTool>(this);
}

/// Destructor
///////////////
TruthParticleCnvTool::~TruthParticleCnvTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode TruthParticleCnvTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // Get the Particle Properties Service
  ServiceHandle<IPartPropSvc> partPropSvc("PartPropSvc", name());
  if ( !partPropSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR(" Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }      

  m_pdt = partPropSvc->PDT();
  if ( 0 == m_pdt ) {
    ATH_MSG_ERROR("Could not retrieve HepPDT::ParticleDataTable from "\
		  "ParticleProperties Service !!");
    return StatusCode::FAILURE;
  }

  // retrieve the TruthIsolation tool only if asked for.
  if ( m_doEtIsolation.value() &&
       !m_isolationTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Creation of TruthIsolationTool FAILED !");
    return StatusCode::FAILURE;    
  }


  // for compatibility with earlier version we accept <0 values :
  if(m_selectSignalTypeProp<0) m_selectSignalType = PileUpClassification::ALLMINBIAS;
  else m_selectSignalType = (PileUpClassification::PileuType_t) m_selectSignalTypeProp;
  

  ATH_MSG_INFO
    (" DoEtIsolations:       [" << std::boolalpha 
     << m_doEtIsolation.value() << "]" << endreq
     << " McEvents:             [" << m_mcEventsName.value() << "]" << endreq
     << " TruthParticlesOutput: [" << m_mcPartsOutputName.value() << "]"
     << endreq
     << " SelectSignalType:     [" << m_selectSignalType << "]");

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleCnvTool::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  TruthParticleContainer * mcParts = new TruthParticleContainer;
  if ( evtStore()->record( mcParts, m_mcPartsOutputName.value() ).isFailure() ) {
    ATH_MSG_ERROR("Could not record TruthParticleContainer at ["
                  << m_mcPartsOutputName.value() << "] !!");
    delete mcParts;
    mcParts = 0;
    return StatusCode::FAILURE;
  }

  if ( !evtStore()->setConst( mcParts ).isSuccess() ) {
    ATH_MSG_WARNING("Could not setConst TruthParticleContainer at ["
                    << m_mcPartsOutputName.value() << "]");
  }

  const McEventCollection * mcEvent = 0;
  if ( evtStore()->retrieve( mcEvent, m_mcEventsName.value() ).isFailure() || 
       0 == mcEvent ) {
    ATH_MSG_WARNING("Could not retrieve McEventCollection at ["
                    << m_mcEventsName.value() << "] !!"
                    << endreq
                    << "TruthParticleContainer [" << m_mcPartsOutputName.value()
                    << "] will be EMPTY !!");
    return StatusCode::RECOVERABLE;
  }

  ATH_MSG_DEBUG(" Found McEventCollection of size = "<< mcEvent->size() );
  // do the actual convertion (it is merely an interface adaptation now...)
  //  selection for particles from minbias copied from the JetsFromTruthTool

  std::size_t genEventIndex = 0;
  const ITruthParticleVisitor* dummyVisitor = 0;
  bool all_good = true;

  McEventCollection::const_iterator fEvt = mcEvent->begin();
  McEventCollection::const_iterator lEvt = mcEvent->end();
  PileUpClassification::findEventIterators(m_selectSignalType, fEvt,lEvt);

  ATH_MSG_DEBUG(" Found McEventCollection iterators : "<< (fEvt-mcEvent->begin()) << " to "<< (lEvt-mcEvent->begin()) );

  for(McEventCollection::const_iterator it=fEvt ; it != lEvt; it++){
    const HepMC::GenEvent* evt = *it;
    // there are holes in a McEventCollection when pile-up is enabled
    // so deal with it
    if (0 == evt) {
      continue;
    }
    genEventIndex = (it - mcEvent->begin());
    ATH_MSG_DEBUG(" adding event id="<< evt->signal_process_id()<<"  genEventIndex="<< genEventIndex );

    if( evt->signal_process_id() == 0 ) continue;
    if (!this->convert( mcEvent, genEventIndex, mcParts, dummyVisitor ).isSuccess()) {
      ATH_MSG_DEBUG("Failed to convert an event...");
      all_good = false;
    }
  }

  // VERY IMPORTANT ! Reset the index to the first GenEvent included
  // in this TruthParticleContainer. This will be used when read back from disk.
  mcParts->setGenEvent( mcEvent, (fEvt - mcEvent->begin() ) );

  
  return all_good 
    ? StatusCode::SUCCESS
    : StatusCode::RECOVERABLE;
}


/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode
TruthParticleCnvTool::convert( const McEventCollection * mcCollection, 
                               const unsigned int genEventIndex,
                               TruthParticleContainer * container,
                               const ITruthParticleVisitor* visitor ) const 
{
  ATH_MSG_DEBUG("Converting McEventCollection to TruthParticleContainer");

  if ( 0 == m_pdt ) {
    ATH_MSG_ERROR("Could not convert McEventCollection into "\
		  "TruthParticleContainer if NO ParticleDataTable is "\
		  "available !!");
    return StatusCode::FAILURE;
  }
  
  if ( 0 == mcCollection ) {
    ATH_MSG_WARNING("Null pointer to McEventCollection !");
    return StatusCode::RECOVERABLE;
  }

  if ( mcCollection->size() <= genEventIndex ) {
    ATH_MSG_WARNING("McEventCollection size: " << mcCollection->size() 
		    << endreq
		    << "Requested element nbr : " << genEventIndex << " !!");
    return StatusCode::RECOVERABLE;
  }

  /// Get GenEvent from McEventCollection
  ATH_MSG_DEBUG("Retrieve the GenEvent from given McEventCollection");
  const HepMC::GenEvent * evt = (*mcCollection)[genEventIndex];
  container->setGenEvent( mcCollection, genEventIndex );

  // reserve enough space for the container so we don't have to relocate it
  container->reserve( container->size() + evt->particles_size() );

  /// Create a map to enhance access between GenParticles and TruthParticles
  TruthParticleContainer::Map_t bcToMcPart = container->m_particles;

  const HepMC::GenEvent::particle_const_iterator itrEnd = evt->particles_end();
  for ( HepMC::GenEvent::particle_const_iterator itrPart=evt->particles_begin();
	itrPart != itrEnd;
	++itrPart ) {
    const HepMC::GenParticle * hepMcPart = (*itrPart);

    TruthParticle * mcPart = new TruthParticle( hepMcPart, container );
    container->push_back( mcPart );
    
    if ( visitor ) {
      visitor->visit( mcPart );
    }

    mcPart->setCharge( McUtils::chargeFromPdgId( mcPart->pdgId(), m_pdt ) );
    mcPart->setGenEventIndex( genEventIndex);

    if ( hepMcPart != mcPart->genParticle() ) {
      ATH_MSG_ERROR("TruthParticle is not wrapping the GenParticle : " 
		    << hepMcPart->barcode() << " !!");
    }
    //bcToMcPart[ hepMcPart->barcoade() ] = mcPart;
    HepMcParticleLink mcLink( hepMcPart->barcode(), genEventIndex );
    bcToMcPart[ mcLink.compress() ] = mcPart;

  }//> end loop over particles

  // at this point the whole GenEvent has been proxied.
  // so we can setup its VectorMap
  container->setParticles( bcToMcPart );

  // connect the TruthParticleContainer to the container of TruthEtIsolations
  // if it exists and if we are asked for
  if ( m_doEtIsolation.value() ) {
    const std::string& etIsolName 
      = m_isolationTool->etIsolationsName( container->genEventName() );
    if ( etIsolName.empty() ) {
      ATH_MSG_WARNING("Could not retrieve the name of the "
		      "TruthEtIsolations container (requested: [" 
		      << container->genEventName() << "])");
      return StatusCode::RECOVERABLE;
    }

    const TruthEtIsolationsContainer* etIsols = 0;
    if ( !evtStore()->retrieve( etIsols, etIsolName ).isSuccess() ) {
      ATH_MSG_WARNING("Could not retrieve the TruthEtIsolations container at ["
		      << etIsolName << "] !!");
      return StatusCode::RECOVERABLE;
    }

    // index of HepMC::GenEvent within the McEventCollection is the same
    // than the one of the TruthEtIsolations within the TruthEtIsolationsCont.
    container->setEtIsolations( etIsols, genEventIndex );
  }

  return StatusCode::SUCCESS;
}
