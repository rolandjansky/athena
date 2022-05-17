///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METBuilderTool.cxx 
// Implementation file for class METBuilderTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETComponentMap.h"

// For DeltaR
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METBuilderTool::METBuilderTool(const std::string& name) : 
    AsgTool(name)
  {
  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METBuilderTool::initialize()
  {
    ATH_MSG_DEBUG ("Initializing " << name() << "...");

    // Determine if modified clusters are used for jet inputs
    m_useModClus = !m_mod_clus_key.empty();

    return StatusCode::SUCCESS;
  }

  StatusCode METBuilderTool::execute(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    if( metTerm==0 ) {
      ATH_MSG_ERROR("Invalid input MissingET pointer provided!");
      return StatusCode::FAILURE;
    }
    if( metMap==0 ) {
      ATH_MSG_ERROR("Invalid input MissingETComponentMap pointer provided!");
      return StatusCode::FAILURE;
    }

    metTerm->setName(m_output_met_key);
    return this->executeTool(metTerm,metMap);
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

  bool METBuilderTool::addToMET(const xAOD::IParticle* object,
				const std::vector<const xAOD::IParticle*>& acceptedSignals,
				xAOD::MissingET* metTerm,
				xAOD::MissingETComponentMap* metMap,
				MissingETBase::Types::weight_t& objWeight,
				MissingETBase::UsageHandler::Policy p) const
  {

    metTerm->add(object->pt()*cos(object->phi())*objWeight.wpx(),
		 object->pt()*sin(object->phi())*objWeight.wpy(),
		 object->pt()*objWeight.wet()); // or actually ET?)

    return MissingETComposition::insert(metMap,metTerm,object,acceptedSignals,objWeight,p);
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
