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

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

#include <iomanip>

namespace met {

  using namespace xAOD;
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
    declareProperty( "METAssociators", m_metassociators              );
    declareProperty( "METSuffix",      m_metsuffix = "AntiKt4LCTopo" );
    declareProperty( "TCSignalState",  m_signalstate = 1             );
    declareProperty( "AllowOverwrite", m_overwrite = false           );
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
    bool mapExists = evtStore()->contains<xAOD::MissingETAssociationMap>(m_mapname);
    bool coreExists = evtStore()->contains<xAOD::MissingETContainer>(m_corename);
    if(!m_overwrite && mapExists) {
      ATH_MSG_WARNING("Association map \"" << m_mapname << "\" is already present and AllowOverwrite=False, exiting.");
      return StatusCode::SUCCESS;
    }
    if(!m_overwrite && coreExists) {
      ATH_MSG_WARNING("MET_Core container \"" << m_corename << "\" is already present and AllowOverwrite=False, exiting.");
      return StatusCode::SUCCESS;
    }
    if(mapExists!=coreExists) {
      ATH_MSG_WARNING("Overwriting only " << (mapExists?"map":"core container") << " may result in meaningless results.");
      return StatusCode::SUCCESS;
    }

    //Create map and core containers
    xAOD::MissingETAuxAssociationMap* metAuxMap = new xAOD::MissingETAuxAssociationMap();
    xAOD::MissingETAssociationMap* metMap = new xAOD::MissingETAssociationMap();
    metMap->setStore(metAuxMap);
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    MissingETContainer* metCont = new MissingETContainer();
    metCont->setStore(metAuxCont);

    //Record or overwrite association map
    if(!mapExists && evtStore()->record(metAuxMap, m_mapname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxAssociationMap: " << m_mapname+"Aux.");
      return StatusCode::SUCCESS;
    }
    if(mapExists && evtStore()->overwrite(metAuxMap, m_mapname+"Aux.",true,false).isFailure() ) {
      ATH_MSG_WARNING("Unable to overwrite MissingETAuxAssociationMap: " << m_mapname+"Aux.");
      return StatusCode::SUCCESS;
    }
    if(!mapExists && evtStore()->record(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    if(mapExists && evtStore()->overwrite(metMap, m_mapname,true,false).isFailure() ) {
      ATH_MSG_WARNING("Unable to overwrite MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }

    //Record or overwrite core container
    if(!coreExists && evtStore()->record(metCont, m_corename).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_corename);
      return StatusCode::SUCCESS;
    }
    if(coreExists && evtStore()->overwrite(metCont, m_corename,true,false).isFailure() ) {
      ATH_MSG_WARNING("Unable to overwrite MissingETContainer: " << m_corename);
      return StatusCode::SUCCESS;
    }
    if(!coreExists && evtStore()->record(metAuxCont, m_corename+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_corename+"Aux.");
      return StatusCode::SUCCESS;
    }
    if(coreExists && evtStore()->overwrite(metAuxCont, m_corename+"Aux.",true,false).isFailure() ) {
      ATH_MSG_WARNING("Unable to overwrite MissingETAuxContainer: " << m_corename+"Aux.");
      return StatusCode::SUCCESS;
    }

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

    // Set the topocluster signal states for the duration of this method
    // Cluster signal states will revert upon the return.
    CaloClusterChangeSignalStateList stateHelperList;    
    if(m_signalstate>=0) { // can ignore this for PFlow
      const CaloClusterContainer* clusters(0);
      if( evtStore()->retrieve(clusters,"CaloCalTopoClusters").isFailure() ) {
	ATH_MSG_WARNING("Failed to set topocluster signal states!");
	return StatusCode::FAILURE;
      } else {
	for(const auto& clus : *clusters) {
	  stateHelperList.add(clus,CaloCluster::State(m_signalstate));
	}
      }
    }

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
