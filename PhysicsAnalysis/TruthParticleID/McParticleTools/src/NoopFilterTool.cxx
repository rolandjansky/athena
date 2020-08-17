/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// NoopFilterTool.cxx 
// Implementation file for class NoopFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenInteracting.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// McParticleTools includes
#include "NoopFilterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace TruthHelper;

/// Constructors
////////////////
NoopFilterTool::NoopFilterTool( const std::string& type, 
				const std::string& name, 
				const IInterface* parent ) : 
  TruthParticleFilterBaseTool( type, name, parent )
{
  //
  // Property declaration
  // 
}

/// Destructor
///////////////
NoopFilterTool::~NoopFilterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode NoopFilterTool::buildMcAod( const McEventCollection* in,
				       McEventCollection* out )
{
  if ( 0 == in || 0 == out ) {
    ATH_MSG_ERROR("Invalid pointer to McEventCollection !" << endmsg
		  << "  in: " << in << endmsg
		  << " out: " << out);
    return StatusCode::FAILURE;
  }

  // we just copy the input McEventCollection and put it into the output one
  // No filtering is applied
  out->operator=( *in );

  for ( unsigned int iEvt = 0; iEvt != out->size(); ++iEvt ) {
    const HepMC::GenEvent * outEvt = (*out)[iEvt];
    if ( 0 == outEvt ) {
      ATH_MSG_WARNING
	("Could not launch filtering procedure for GenEvent number ["
	 << iEvt << "] from McEventCollection ["
	 << m_mcEventsReadHandleKey.key() << "] !!"
	 << endmsg
	 << "  outEvt: " << outEvt);
      continue;
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

