/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBBTool.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TBBTool::TBBTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
  : AthAlgTool( type, name, parent ), m_myMessage("Default message set in TBBTool.cxx"), m_increment(1), m_called(1)
{
  declareProperty( "MyMessage", m_myMessage, "something to say" );
  declareProperty( "MyIncrement", m_increment, "something to add" );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TBBTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IHelloTool::interfaceID() )  {
      *ppvIf = (IHelloTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TBBTool::saySomething(std::string& something)
{
  //wasting time trying to get slammed by another thread;
  long long old_called = m_called;
  something += " - " + m_myMessage;
  double val = atan((double)m_called);
  if (rand() > 0.5) val += atan((double)m_called/2.);
  ATH_MSG_DEBUG ("message #" << m_called 
		<< " to the world: " << something);
  StatusCode c(StatusCode::SUCCESS);
  m_called = old_called + (val == 0 ? 1 : round(val/val));
  return c;
}

int TBBTool::addToIt(int soFar) { 
  ATH_MSG_DEBUG ("adding " << m_increment
		<< " to " << soFar);
  return (soFar + m_increment); 
}
