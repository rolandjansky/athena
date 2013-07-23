/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include files
#include "GaudiKernel/MsgStream.h"
// Tool example
#include "ISProperty/ISPropTestAlgTool.h"

ISPropTestAlgTool::ISPropTestAlgTool( const std::string& type, const std::string& name, const IInterface* parent )
  : AlgTool( type, name, parent ),
    log(msgSvc(),name) {

  // declare my special interface
  declareInterface<IISPropTestAlgTool>(this);

  // declare properties
  declareProperty( "Counter",    m_Counter    = 100,"Some counter in my tool");
}

//------------------------------------------------------------------------------
void  ISPropTestAlgTool::doIt()
//------------------------------------------------------------------------------
{
  log << MSG::INFO << "doIt() has been called" << endreq;

  //Increase the local counter variable
  ++m_Counter;
  //Update our counter property only every seventh event
  if ( ( m_Counter % 7 ) == 0 )
    setProperty("Counter",m_Counter);

}

//------------------------------------------------------------------------------
StatusCode  ISPropTestAlgTool::initialize()
//------------------------------------------------------------------------------
{
  log.setLevel(outputLevel());
  log << MSG::INFO << "intialize()" << endreq;

  log << MSG::INFO << "Counter = " << m_Counter    << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode  ISPropTestAlgTool::finalize()
//------------------------------------------------------------------------------
{
  log << MSG::INFO << "finalize() has been called" << endreq;
  return StatusCode::SUCCESS;
}

