///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AddVarAlg.cxx
// Implementation file for class AddVarAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "AddVarAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
AddVarAlg::AddVarAlg( const std::string& name,
                      ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_tool("AddVarTool/AddVarTool", this),
  m_setInCollKey(false),
  m_setVarName(false),
  m_setVarType(false),
  m_setSelection(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",       m_jos, "The JobOptionService instance.");

  declareProperty("AddVarTool",          m_tool, "The private AddVarTool" );

  declareProperty("AddVarTo",   m_inCollKey="",
                  "Name of the container or object where the new variable will be added to" );
  m_inCollKey.declareUpdateHandler( &AddVarAlg::setupInputContainer, this );

  declareProperty("VarName",             m_varName="",   "The name of the new variable" );
  m_varName.declareUpdateHandler( &AddVarAlg::setupVarName, this );

  declareProperty("VarType",             m_varType="float",
                  "The type of the new variable (allowed values are: 'bool', 'int', 'float')" );
  m_varType.declareUpdateHandler( &AddVarAlg::setupVarType, this );

  declareProperty("Selection",         m_selection="",
                  "The selection string that defines which xAOD::IParticles to select from the container" );
  m_selection.declareUpdateHandler( &AddVarAlg::setupSelection, this );
}



// Destructor
///////////////
AddVarAlg::~AddVarAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode AddVarAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_tool );
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_varName );
  ATH_MSG_DEBUG ( " using = " << m_varType );
  ATH_MSG_DEBUG ( " using = " << m_selection );


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;


  // Get the JobOptionService
  // We will use this to set the properties of our private skimming tool
  // from the properties of this algorithm.
  ATH_MSG_VERBOSE( "Getting the JobOptionService");
  ATH_CHECK( m_jos.retrieve() );

  // Get the full name of the private skimTool
  ATH_MSG_VERBOSE( "Getting the full name of the tool");
  const std::string& fullToolName = this->name() + "." + m_tool.name();
  ATH_MSG_DEBUG( "Got the full name of the tool: " << fullToolName );


  // Now, set all properties of the private skimTool that were acutally configured
  if (m_setInCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKey
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_inCollKey) );
  }
  if (m_setVarName) {
    ATH_MSG_DEBUG( "Setting property" << m_varName
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_varName) );
  }
  if (m_setVarType) {
    ATH_MSG_DEBUG( "Setting property" << m_varType
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_varType) );
  }
  if (m_setSelection) {
    ATH_MSG_DEBUG( "Setting property" << m_selection
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_selection) );
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_tool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode AddVarAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_tool.release() );

  return StatusCode::SUCCESS;
}



StatusCode AddVarAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the tool
  ATH_CHECK( m_tool->addBranches() );

  return StatusCode::SUCCESS;
}
