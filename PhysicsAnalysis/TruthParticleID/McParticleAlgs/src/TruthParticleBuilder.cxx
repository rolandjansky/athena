/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleBuilder.cxx 
// Implementation file for class TruthParticleBuilder
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleFilterTool.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleAlgs includes
#include "TruthParticleBuilder.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
TruthParticleBuilder::TruthParticleBuilder( const std::string& name, 
					    ISvcLocator* pSvcLocator ) : 
  AthAlgorithm ( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr = "";
//   descr += "Fully qualified name ('[class name]/[instance name]') of the ";
//   descr += "AlgTool to be ran to create a filtered McEventCollection\n";
//   descr += "ex: \"EtaPtFilterTool/EtaPtFilterTool\"";
//   declareProperty( "Filter", 
// 		   m_filterName = "EtaPtFilterTool",
// 		   descr );
//   m_filterName.declareUpdateHandler( &TruthParticleBuilder::setupFilterTool,
// 				     this );

  descr  = "Pointer to the filter algtool (which will create the filtered ";
  descr += "McEventCollection)";
  declareProperty( "FilterTool",
		   m_filterTool = FilterTool_t( "EtaPtFilterTool", 
						this ),
		   descr );

  descr  = "Pointer to the converter algtool which will create a ";
  descr += "TruthParticleContainer from the filtered McEventCollection";
  declareProperty( "CnvTool",
		   m_cnvTool = CnvTool_t( "TruthParticleCnvTool", 
					  this ),
		   descr );

  // switches
  descr  = "Switch to build or not a filtered McEventCollection";
  declareProperty( "DoFiltering", 
		   m_doFiltering = true,
		   descr );
  m_doFiltering.declareUpdateHandler( &TruthParticleBuilder::setupFilterTool,
				      this );

  descr  = "Switch to build or not the TruthParticleContainer from the ";
  descr += "filtered GenEvent";
  declareProperty( "DoTruthParticles", 
		   m_doTruthParticles = true,
		   descr );
  m_doTruthParticles.declareUpdateHandler( &TruthParticleBuilder::setupCnvTool,
					   this );
}

/// Destructor
///////////////
TruthParticleBuilder::~TruthParticleBuilder()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode TruthParticleBuilder::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // setup the filter tool
  setupFilterTool( m_doFiltering );
  
  // setup the converter tool
  setupCnvTool( m_doTruthParticles );

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleBuilder::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode TruthParticleBuilder::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  // create a new McEventCollection (AOD) from an old one (ex:ESD)
  // apply some filtering criterion (if any)
  if ( m_doFiltering && 
       !m_filterTool->execute().isSuccess() ) {
    ATH_MSG_WARNING
      ("Could not build the AOD McEventCollection ! [algtool="
       << m_filterTool.type() << "]");
    return StatusCode::SUCCESS;
  }
  
  // create a TruthParticleContainer from a McEventCollection
  if ( m_doTruthParticles && 
       !m_cnvTool->execute().isSuccess() ) {
    ATH_MSG_WARNING 
      ("Could not convert the AOD McEventCollection into "\
       "a TruthParticleContainer !");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void TruthParticleBuilder::setupFilterTool( Gaudi::Details::PropertyBase& /*doFiltering*/ )
{
  if ( m_doFiltering ) {
    
    if ( !m_filterTool.retrieve().isSuccess() ) {
      ATH_MSG_ERROR
	("Could not retrieve algTool ITruthParticleFilterTool ["
	 << m_filterTool.type() << "/" 
	//<< m_filterTool.name() 
	 << "] !!");
      throw std::runtime_error("Could not setup FilterTool property !");
    } else {
      ATH_MSG_DEBUG
	("Retrieved and configured algTool ["
	 << m_filterTool.type() << "/" 
	//<< m_filterTool.name() 
	 << "]");
    }
  } else {
    if ( !m_filterTool.release().isSuccess() ) {
      ATH_MSG_WARNING
	("Could not release algTool ITruthParticleFilterTool ["
	 << m_filterTool.type() << "/" 
	//<< m_filterTool.name() 
	 << "] !!"
	 << endmsg
	 << "Memory won't be freed until ::finalize()...");
    } else {
      ATH_MSG_DEBUG
	("Released algTool [" << m_filterTool.type() << "/" 
	 //<< m_filterTool.name() 
	 << "]");
    }
  }

  return;
}

void TruthParticleBuilder::setupCnvTool( Gaudi::Details::PropertyBase& /*doTruthParticles*/ )
{
  if ( m_doTruthParticles ) {

    if ( !m_cnvTool.retrieve().isSuccess() ) {
      ATH_MSG_ERROR
	("Could not retrieve algTool ITruthParticleCnvTool ["
	 << m_cnvTool.type() << "/" 
	 //<< m_cnvTool.name() 
	 << "] !!");
      throw std::runtime_error("Could not setup CnvTool property !");
    } else {
      ATH_MSG_DEBUG
	("Retrieved and configured algTool [" << m_cnvTool.type() << "/" 
	 //<< m_cnvTool.name() 
	 << "]");
    }

  } else {
    if ( !m_cnvTool.release().isSuccess() ) {
      ATH_MSG_WARNING
	("Could not release algTool ITruthParticleCnvTool ["
	 << m_cnvTool.type() << "/" 
	 //<< m_cnvTool.name() 
	 << "] !!"
	 << endmsg
	 << "Memory won't be freed until ::finalize()...");
    } else {
      ATH_MSG_DEBUG
	("Released algTool [" << m_cnvTool.type() << "/" 
	 //<< m_cnvTool.name() 
	 << "]");
    }
  }

  return;
}
