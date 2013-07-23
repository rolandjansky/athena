/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ISProperty/ISPropTestAlg.h"
#include "ISProperty/ISPropTestAlgTool.h"

// Constructor 
ISPropTestAlg::ISPropTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name,pSvcLocator),
  ISPropertyMgr(name,pSvcLocator), //<--- Call ISPropertyMgr constructor
  m_AnyService("ISPropertySvc/ISPropertySvc",name),
  m_AnyTool("ISPropTestAlgTool",this,true),
  log(msgSvc(),name)
{
  //  declare properties using "declareISProperty" instead of "declareProperty"
  Property* myProp = declareISProperty("MyInt", m_myInt, "My integer value");
  // Add an update handler for this property
  myProp->declareUpdateHandler( &ISPropTestAlg::updateHandler, this );
  // another property
  myProp = declareISProperty("Delay", m_Delay, "Delay inbetween execute calls");
  //There can only be one update handler for each property
  myProp->declareUpdateHandler( &ISPropTestAlg::updateHandler, this );

  //Add a simple property
  declareISOnlyProperty("Events", m_Events, "Total number of events");

  /**
   * NOTE: We can also have properties of remote objects being published
   * throught this interface. However, this only works for objects that
   * have a GaudiHandle and can be retrieved in the constructor.
   **/

  //Add a property from a remote service
  declareISRemoteProperty("OutputLevel",m_AnyService);
  //Add a property from a remote tool
  declareISRemoteProperty("Counter",m_AnyTool);

  //Make the update time a jobOption
  declareProperty("UpdateTime",m_updateTime=1.,"Minimum time after which the IS object can be updated on the athena side");
}


// Update handler
void ISPropTestAlg::updateHandler(Property& p)
{
   log << MSG::INFO << "Update handler called for property: " << p << endmsg;
}


// Initialisation 
StatusCode  ISPropTestAlg::initialize()
{
  log.setLevel(outputLevel());
  log << MSG::VERBOSE << " initialize()" << endreq;

  //publish our properties
  return publishProperties();
}

// Finalisation 
StatusCode  ISPropTestAlg::finalize()
{
  //unpublish our properties
  return unpublishProperties();
}

// Execution 
StatusCode  ISPropTestAlg::execute()
{
  //Update some properties
  log << MSG::INFO << "Updating property MyInt" << endreq;
  
  //normal properties need to be updated with setProperty 
  setProperty( "MyInt", m_myInt+1 );

  //SimpleProperties can be updated using operator
  m_Events = m_Events + 1; 

  //Have our tool also do something
  m_AnyTool->doIt();

  //sleep for the time given by DelayProperty
  usleep((long)(m_Delay*1e6));  

  return StatusCode::SUCCESS;
}
