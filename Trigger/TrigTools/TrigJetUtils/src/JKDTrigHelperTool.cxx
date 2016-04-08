///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JKDTrigHelperTool.cxx 
// Implementation file for class JKDTrigHelperTool
/////////////////////////////////////////////////////////////////// 

// TrigJetUtils includes
#include "JKDTrigHelperTool.h"
#include "setupJetKeyDescriptor.h"

// FrameWork includes
#include "AthenaKernel/errorcheck.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"

#include "JetEvent/JetKeyDescriptor.h"

// Constructors
////////////////
JKDTrigHelperTool::JKDTrigHelperTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ), m_is_setup(false)
{
  declareInterface< IJKDTrigHelperTool >(this);

}

// Destructor
///////////////
JKDTrigHelperTool::~JKDTrigHelperTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode JKDTrigHelperTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  //check that we're a public tool/common instance
  if ( this->parent() != this->toolSvc() ) {
    ATH_MSG_ERROR("This tool should be used as a public tool");
    return StatusCode::FAILURE;
  }


  // Get handle on IncidentSvc
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  CHECK(incidentSvc.retrieve());

  incidentSvc->addListener(this, "BeginEvent");  

  return StatusCode::SUCCESS;
}

StatusCode JKDTrigHelperTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 


StatusCode JKDTrigHelperTool::setupJetKeyDescriptor(StoreGateSvc* store, HLT::Navigation* navig) 
{

  //if already setup, simply return
  if (m_is_setup) return StatusCode::SUCCESS;

  //sanity checks
  if ( !store || !navig) { ATH_MSG_ERROR("Null pointers.."); return StatusCode::FAILURE; }

  // setup the JetKey descriptor
  if ( TrigJetUtils::setupJetKeyDescriptor(store , msg()).isFailure() ){
    ATH_MSG_ERROR(" Couldn't setup JetKeyDescriptor ");
    return StatusCode::FAILURE;
  }
  else { ATH_MSG_DEBUG("Successfully setup JetKeyDescriptor"); }

  std::string jetKeyDesName = "JetKeyMap";
  if ( !navig->attachFeature(navig->getInitialNode(), JetKeyDescriptorInstance::instance()->getKeyStore(), HLT::Navigation::ObjectInStoreGate, jetKeyDesName, "TrigJetKeyMap")) {
       ATH_MSG_ERROR("Failed to attach JKD as a feature");
       return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Using EF output jet key descriptor JetKeyMap = "<< MSG::hex << JetKeyDescriptorInstance::instance()->getKeyStore() << MSG::dec);

  m_is_setup = true;

 return StatusCode::SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////
// Handle incidents
////////////////////////////////////////////////////////////////////////////////
void JKDTrigHelperTool::handle(const Incident& inc)
{
  if (inc.type()=="BeginEvent") {
      ATH_MSG_DEBUG("Got a BeginEvent incident, reset flag");
      m_is_setup = false;
  }

}


