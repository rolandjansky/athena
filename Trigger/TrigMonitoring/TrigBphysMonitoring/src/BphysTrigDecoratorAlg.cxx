///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BphysTrigDecoratorAlg.cxx 
// Implementation file for class BphysTrigDecoratorAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TrigBphysMonitoring includes
#include "BphysTrigDecoratorAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "xAODTracking/Vertex.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
BphysTrigDecoratorAlg::BphysTrigDecoratorAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{}

// Destructor
///////////////
BphysTrigDecoratorAlg::~BphysTrigDecoratorAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode BphysTrigDecoratorAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  ATH_CHECK( m_decorators.retrieve() );
  ATH_CHECK( m_JpsiCandidatesKey.initialize() );
    
    
  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDecoratorAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDecoratorAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

    // get the dimuon candidates
    SG::ReadHandle<xAOD::VertexContainer> mumuContainer{m_JpsiCandidatesKey};
    if(!mumuContainer.isValid()){
        ATH_MSG_INFO("No vertex container " << m_JpsiCandidatesKey << " skipping decoration");
        return StatusCode::SUCCESS;
    } else {
        ATH_MSG_DEBUG("Found Container " << m_JpsiCandidatesKey << " with " <<
                      mumuContainer->size() << " candidates" );
    }
    
    // call each decorator tool on each vertex:
    for (auto vtx: *mumuContainer) {
        ATH_MSG_VERBOSE("Vertex: " << vtx);
        ATH_MSG_VERBOSE("Vertex: " << vtx->x() << " " << vtx->y() << " " << vtx->z());
        
        for (auto tool : m_decorators) {
            if (tool->decorateVertex(vtx,nullptr).isFailure()) {
                ATH_MSG_WARNING("Problems in tool: " << tool.name());
            } // if worked
        } // tool loop
    } // vertex loop
    
    
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


