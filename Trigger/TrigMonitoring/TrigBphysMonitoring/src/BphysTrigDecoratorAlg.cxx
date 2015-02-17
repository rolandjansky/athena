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
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
BphysTrigDecoratorAlg::BphysTrigDecoratorAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
,m_decorators(this)
{
  //
  // Property declaration
    declareProperty( "JpsiContainerName", m_JpsiCandidatesKey = "JpsiCandiates"   );
    declareProperty( "PVContainerName",   m_pvCandidatesKey   = "PrimaryVertices" );

    declareProperty( "Decorators",   m_decorators );

    
}

// Destructor
///////////////
BphysTrigDecoratorAlg::~BphysTrigDecoratorAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode BphysTrigDecoratorAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

    for (auto tool : m_decorators) {
        ATH_CHECK(tool.retrieve());
    }
    
    
  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDecoratorAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDecoratorAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

    // get the dimuon candidates
    const xAOD::VertexContainer* mumuContainer(0);
    if(evtStore()->retrieve(mumuContainer   ,m_JpsiCandidatesKey).isFailure()){
        ATH_MSG_INFO("No vertex container " << m_JpsiCandidatesKey << " skipping decoration");
        return StatusCode::SUCCESS;
    } else {
        ATH_MSG_DEBUG("Found Container " << m_JpsiCandidatesKey << " with " <<
                      (mumuContainer? mumuContainer->size() : -1) << " candidates" );
    }
    
    // call each decorator tool on each vertex:
    for (auto vtx: *mumuContainer) {
        ATH_MSG_VERBOSE("Vetex: " << vtx);
        ATH_MSG_VERBOSE("Vetex: " << vtx->x() << " " << vtx->y() << " " << vtx->z());
        
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


