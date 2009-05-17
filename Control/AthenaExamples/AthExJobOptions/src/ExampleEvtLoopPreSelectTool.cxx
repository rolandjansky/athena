/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/ExampleEvtLoopPreSelectTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"


/////////////////////////////////////////////////////////////////////////////

ExampleEvtLoopPreSelectTool::ExampleEvtLoopPreSelectTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
   : AthAlgTool( type, name, parent )
{

// declare any properties here
   declareInterface<IAthenaEvtLoopPreSelectTool>( this );
   declareProperty( "PassIfMod", m_prescale = 2, "Keep 1 event in n" );

}

//___________________________________________________________________________
StatusCode ExampleEvtLoopPreSelectTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IAthenaEvtLoopPreSelectTool::interfaceID() )  {
      *ppvIf = (IAthenaEvtLoopPreSelectTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

//___________________________________________________________________________
StatusCode ExampleEvtLoopPreSelectTool::initialize()
{

// perform necessary one-off initialization

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode ExampleEvtLoopPreSelectTool::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}

//__________________________________________________________________
bool  ExampleEvtLoopPreSelectTool::passEvent(const EventInfo* pEvent)
{

// do what needs to be done
   ATH_MSG_DEBUG ("Entering PassEvent ");


   EventID::number_type evtNumber = pEvent->event_ID()->event_number();
   return (0 == (evtNumber % m_prescale));
 }
