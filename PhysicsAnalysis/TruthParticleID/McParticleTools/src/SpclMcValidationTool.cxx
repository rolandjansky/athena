/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// SpclMcValidationTool.cxx 
// Implementation file for class SpclMcValidationTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes

#include "GeneratorObjects/McEventCollection.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleKernel includes
#include "McParticleKernel/IIOMcAodTool.h"

// McParticleTools includes
#include "SpclMcValidationTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
SpclMcValidationTool::SpclMcValidationTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) : 
  TruthParticleValidationBaseTool( type, name, parent )
{
  //
  // Property declaration
  // 
  declareProperty( "McEvents",       
		   m_mcEventsName = "GEN_AOD",
		   "Name of the reference McEventCollection" );

  declareProperty( "TruthParticles", 
		   m_truthParticlesName = "SpclMC",
		   "Name of the 'to-be-validated' TruthParticleContainer" );

  declareProperty( "McAodWriterTool",
		   m_mcAodWriterTool = McAodTool_t( "McAodWriterTool", this ),
		   "Tool to write the TruthParticleContainer into a "
		   "dedicated file" );
}

/// Destructor
///////////////
SpclMcValidationTool::~SpclMcValidationTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode SpclMcValidationTool::initializeTool()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  // retrieve and configure McAod writer tool
  if ( setupMcAodWriterTool().isFailure() ) {
    ATH_MSG_ERROR("Could not configure the McAod writer tools !!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SpclMcValidationTool::finalizeTool()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode SpclMcValidationTool::executeTool()
{
  // retrieve McEventCollection
  const McEventCollection * mcEvents = 0;
  if ( evtStore()->retrieve( mcEvents, m_mcEventsName ).isFailure() ||
       0 == mcEvents ) {
    ATH_MSG_ERROR("Could not retrieve McEventCollection at ["
		  << m_mcEventsName << "] !!");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE("Successfully retrieved McEventCollection at ["
		    << m_mcEventsName << "]");
  }

  // retrieve TruthParticleContainer
  const TruthParticleContainer * mcParts = 0;
  if ( evtStore()->retrieve( mcParts, m_truthParticlesName ).isFailure() ||
       0 == mcParts ) {
    ATH_MSG_ERROR("Could not retrieve TruthParticleContainer at ["
		  << m_truthParticlesName << "] !!");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE("Successfully TruthParticleContainer at ["
		    << m_truthParticlesName << "]");
  }

  return executeTool( mcEvents, mcParts );
}

StatusCode 
SpclMcValidationTool::executeTool( const McEventCollection* mcEvents,
				   const TruthParticleContainer* mcParts )
{
  if ( 0 == mcEvents ) {
    ATH_MSG_ERROR("NULL pointer to reference McEventCollection !!");
    return StatusCode::FAILURE;
  }

  if ( 0 == mcParts ) {
    ATH_MSG_ERROR("NULL pointer to the 'to-be-validated' "\
		  "TruthParticleContainer !!");
    return StatusCode::FAILURE;
  }

  if ( mcEvents->empty() ) {
    ATH_MSG_WARNING("Empty McEventCollection !");
    return mcParts->empty() ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }

  const HepMC::GenEvent * genEvt = (*mcEvents)[0];
  if ( 0 == genEvt ) {
    ATH_MSG_ERROR("NULL pointer to reference GenEvent !!");
    return StatusCode::FAILURE;
  }

#ifdef HEPMC3
  int particles_size=genEvt->particles().size();
#else
  int particles_size=genEvt->particles_size();
#endif
  if ( static_cast<int>( mcParts->size() ) != particles_size ) {
    ATH_MSG_ERROR("TruthParticleContainer and McEventCollection don't have "\
		  "the same number of particles !!" << endmsg
		  << "\tTruthParticleContainer: " << mcParts->size() << endmsg
		  << "\tHepMC::GenEvent:        " << particles_size);
    return StatusCode::FAILURE;
  }

  if ( !m_mcAodWriterTool->execute().isSuccess() ) {
    ATH_MSG_WARNING("Could NOT write out the TruthParticleContainer through the IIOMcAodTool !!");
  }

  const TruthParticleContainer::const_iterator itrEnd  = mcParts->end();
  for ( TruthParticleContainer::const_iterator itrPart = mcParts->begin();
	itrPart != itrEnd;
	++itrPart ) {
    const TruthParticle * mc = *itrPart;
    auto hepMc = mc->genParticle();

    if ( hepMc->momentum() != HepMC::FourVector(mc->hlv().x(),mc->hlv().y(),mc->hlv().z(),mc->hlv().t()) ) {
      ATH_MSG_ERROR("TruthParticle and GenParticle-link don't have same 4-mom !!");
      return StatusCode::FAILURE;
    }

    auto genPart = HepMC::barcode_to_particle(genEvt,HepMC::barcode(hepMc));

    if ( hepMc->momentum() != genPart->momentum() ) {
      ATH_MSG_ERROR("GenParticle-link and GenParticle from McEvtColl "\
		    "don't have same 4-mom !!");
      return StatusCode::FAILURE;
    }

    
  }
  return StatusCode::SUCCESS;
}

StatusCode SpclMcValidationTool::setupMcAodWriterTool()
{
  if ( !m_mcAodWriterTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Creation of algTool [" 
		  << m_mcAodWriterTool.type() << "/"
		  //<< m_mcAodWriterTool.name() 
		  << "] FAILED !");
    return StatusCode::FAILURE;
  }

  // now we configure the tools
  StringProperty prop( "TruthParticles", m_truthParticlesName.value() );
  if ( m_mcAodWriterTool->setProperty( prop ).isFailure() ) {
    ATH_MSG_ERROR("Could not set property [" << prop.name() << "] for tool ["
		  << m_mcAodWriterTool.type() << "/"
		  //<< m_mcAodWriterTool.name() 
		  << "] !!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
