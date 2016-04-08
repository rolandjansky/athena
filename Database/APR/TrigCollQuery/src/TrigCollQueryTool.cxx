/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCollQuery/TrigCollQueryTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "boost/tokenizer.hpp"

#include <iostream>
#include <sstream>

#include <map>

using namespace std;

typedef boost::tokenizer<boost::char_separator<char> > Tizer;



TrigCollQueryTool::TrigCollQueryTool(const std::string& name,
                                     const std::string& type,
                                     const IInterface* parent)
      : AthAlgTool(name, type, parent)
{
   // properties - default values set in TrigCollQuery::TrigCollQuery()
   declareProperty( "TriggerMapDir", m_XMLDir );
   declareProperty( "UseCTPWord", m_useCTPWord ); 
   declareProperty( "QueryRuns", m_queryRuns ); 
   declareProperty( "RunNumberAttribute", m_runNumberColumnName );
   declareProperty( "TriggerMapWebService", m_triggerWebService );
   declareProperty( "Project", m_dataPeriod );
   declareProperty( "AMITag", m_AMITag );
   
   declareInterface<IAthenaSelectorTool>(this);
}


TrigCollQueryTool::~TrigCollQueryTool()
{}




/// Initialize AlgTool
StatusCode TrigCollQueryTool::initialize()
{
   MsgStream log( msgSvc(), name() );
   log << MSG::DEBUG << "Initialize" << endreq;
   
   queryRuns( m_queryRuns );
   if( !m_XMLDir.empty() ) {
      log << MSG::DEBUG << "Using Trigger configuration from XML files: " << m_XMLDir << endreq;
      XMLConfigDir( m_XMLDir );
      readTriggerMap( );
   }
   
   return StatusCode::SUCCESS;
}


/// Called at the end of initialize
StatusCode TrigCollQueryTool::postInitialize()
{
   MsgStream log( msgSvc(), name() );
   log << MSG::DEBUG << "postInitialization start" << endreq;

   const IService* parentSvc = dynamic_cast<const IService*>(this->parent());
   if (parentSvc == 0) {
      log << MSG::ERROR << "Unable to get parent Service" << endreq;
      return(StatusCode::FAILURE);
   }
   const IProperty* propertyServer = dynamic_cast<const IProperty*>(parentSvc);
   if( propertyServer == 0 ) {
      log << MSG::ERROR << "Unable to get Property Server from the parent service" << endreq;
      return(StatusCode::FAILURE);
   }
   StringProperty queryProperty("Query", "");
   StatusCode status = propertyServer->getProperty(&queryProperty);
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Unable to get Query property from " << parentSvc->name() << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::INFO << "EventSelector property Query is: " << queryProperty.toString() << endreq;
   StringProperty collProperty("CollectionType", "");
   status = propertyServer->getProperty(&collProperty);
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Unable to get CollectionType property from " << parentSvc->name() << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::DEBUG << "EventSelector property CollectionType is: " << collProperty.toString() << endreq;
   try {
      string remappedQ = triggerQueryRemap(queryProperty.toString(), collProperty.toString());
      log << MSG::INFO << "Remappeed Trigger Query: " << remappedQ << endreq;
      queryProperty.setValue(remappedQ);
   } catch( runtime_error &e ) {	
       log << MSG::ERROR << "Trigger Query Error: " << e.what() << endreq;
       return StatusCode::FAILURE;
   } 
   status = const_cast<IProperty*>(propertyServer)->setProperty(queryProperty);
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Unable to set Query property from " << parentSvc->name() << endreq;
      return(StatusCode::FAILURE);
   }

   log << MSG::DEBUG << "postInitialization end"  << endreq; 
   return StatusCode::SUCCESS;
}
   


/// Called at the beginning of execute
StatusCode TrigCollQueryTool::preNext()
{
   return StatusCode::SUCCESS;
}


/// Called at the end of execute
StatusCode TrigCollQueryTool::postNext()
{
   return StatusCode::SUCCESS;
}


/// Called at the beginning of finalize
StatusCode TrigCollQueryTool::preFinalize()
{
   return StatusCode::SUCCESS;
}


/// Finalize AlgTool
StatusCode TrigCollQueryTool::finalize()
{
   return StatusCode::SUCCESS;
}


