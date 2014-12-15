///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssociationTool.cxx 
// Implementation file for class METAssociationTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METAssociationTool.h"

// MET EDM
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

#include <iomanip>

namespace met {

  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETComposition;
  using xAOD::MissingETAuxContainer;
  //
  using std::string;
  using std::setw;
  using std::setprecision;
  using std::fixed;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METAssociationTool::METAssociationTool(const std::string& name) : 
    AsgTool(name)
  {
    declareProperty( "METAssociators", m_metassociators           );
    declareProperty( "METSuffix",   m_metsuffix = "AntiKt4LCTopo" );
  }

  // Destructor
  ///////////////
  METAssociationTool::~METAssociationTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociationTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_metsuffix.size()==0 ) {
      ATH_MSG_ERROR("Output suffix for MET names must be provided.");
      return StatusCode::FAILURE;
    } else {
      m_corename = "MET_Core_"+m_metsuffix;
      m_mapname = "METAssoc_"+m_metsuffix;
      ATH_MSG_INFO("Tool configured to build MET with names:");
      ATH_MSG_INFO("   Core container  ==> " << m_corename);
      ATH_MSG_INFO("   Association map ==> " << m_mapname);
    }

    // retrieve builders
    for(ToolHandleArray<IMETAssocToolBase>::const_iterator iBuilder=m_metassociators.begin();
	iBuilder != m_metassociators.end(); ++iBuilder) {
      ToolHandle<IMETAssocToolBase> tool = *iBuilder;
      if( tool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("Failed to retrieve tool: " << tool->name());
	return StatusCode::FAILURE;
      };
      ATH_MSG_INFO("Retrieved tool: " << tool->name() );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociationTool::execute() const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    if( evtStore()->contains<xAOD::MissingETAssociationMap>(m_mapname) ) {
      ATH_MSG_WARNING("Association map \"" << m_mapname << "\" is already present, exiting.");
      return StatusCode::SUCCESS;
    }

    // Create a MissingETAssociationMap with its aux store
    xAOD::MissingETAssociationMap* metMap = new xAOD::MissingETAssociationMap();
    if( evtStore()->record(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    xAOD::MissingETAuxAssociationMap* metAuxMap = new xAOD::MissingETAuxAssociationMap();
    if( evtStore()->record(metAuxMap, m_mapname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxAssociationMap: " << m_mapname+"Aux.");
      return StatusCode::SUCCESS;
    }
    metMap->setStore(metAuxMap);

    if( evtStore()->contains<MissingETContainer>(m_corename) ) {
      ATH_MSG_WARNING("MET_Core container \"" << m_corename << "\" is already present, exiting.");
      return StatusCode::SUCCESS;
    }
    MissingETContainer* metCont = new MissingETContainer();
    if( evtStore()->record(metCont, m_corename).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_corename);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_corename+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_corename+"Aux.");
      return StatusCode::SUCCESS;
    }
    metCont->setStore(metAuxCont);

    if( buildMET(metCont, metMap).isFailure() ) {
      ATH_MSG_WARNING("Failed in MissingET reconstruction");
      return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociationTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

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

  StatusCode METAssociationTool::buildMET(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {
    // Run the MET reconstruction tools in sequence
    for(ToolHandleArray<IMETAssocToolBase>::const_iterator iBuilder=m_metassociators.begin();
	iBuilder != m_metassociators.end(); ++iBuilder) {
      ToolHandle<IMETAssocToolBase> tool = *iBuilder;
      if (tool->execute(metCont,metMap).isFailure()){
        ATH_MSG_WARNING("Failed to execute tool: " << tool->name());
        return StatusCode::FAILURE;
      }
    }
    bool foundOverlaps = metMap->identifyOverlaps();
    ATH_MSG_DEBUG( (foundOverlaps ? "Overlaps" : "No overlaps") << " identified!");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
