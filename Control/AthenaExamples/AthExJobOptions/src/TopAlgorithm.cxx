/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/TopAlgorithm.h"
#include "AthExJobOptions/IVirtualTool.h"
#include "AthExJobOptions/IVirtualSvc.h"
#include "AthExJobOptions/SomeData.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"

#include "StoreGate/StoreGateSvc.h"


/////////////////////////////////////////////////////////////////////////////

TopAlgorithm::TopAlgorithm( const std::string& name, ISvcLocator* svcloc ) :
   AthAlgorithm( name, svcloc ), KeyMixin( this ),
   m_tool( "ToolUsingTool", this ), // 'this' makes it private tool
   m_publicTool( "ConcreteTool" ),  // public tool
   m_listOfPrivateTools( this ),            // 'this' makes it a list of private tools 
   m_listOfPublicTools(),           // absense of 'this' makes it a list of public tools 
   m_svc( "ConcreteSvc", name ),     // some default service
   m_emptyPrivateTool( "", this ),   // empty private tool
   m_emptyPublicTool ( "" )          // empty public tool
{
// declare any properties here
   declareProperty( "RndmFactor", m_rndmFactor = 1., "multiplicative random number factor" );

   declareProperty( "TheTool",       m_tool,       "helper to offload some tasks to" );
   declareProperty( "ThePublicTool", m_publicTool, "shared helper to offload some tasks to" );
   declareProperty( "PrivateToolList",  m_listOfPrivateTools, "list of private helper tools" );
   declareProperty( "PublicToolList",   m_listOfPublicTools,  "list of public helper tools" );
   declareProperty( "TheSvc",
		    m_svc = IVirtualSvc_t( "ConcreteSvc", this->name() ),
		    "Get important news from a service" );

   declareProperty( "EmptyPrivateTool", 
		    m_emptyPrivateTool,
		    "Testing an empty private tool" );

   declareProperty( "EmptyPublicTool", 
		    m_emptyPublicTool,
		    "Testing an empty public tool" );

   // initialise list of tools
   m_listOfPrivateTools.push_back( "ConcreteTool/Tool1" );
   m_listOfPrivateTools.push_back( "ToolUsingTool/Tool2" );
   m_listOfPrivateTools.push_back( "ToolSpace::TemplatedTool<double>/Tool3" );

   m_listOfPublicTools.push_back( "ConcreteTool/Tool4" );
   m_listOfPublicTools.push_back( "ToolUsingTool/Tool5" );
   m_listOfPublicTools.push_back( "ToolSpace::TemplatedTool<double>/Tool6" );

}

//___________________________________________________________________________
StatusCode TopAlgorithm::initialize()
{

// perform necessary one-off initialization

// verify that our tool handle is pointing to an accessible tool
   if ( m_tool.retrieve().isFailure() ) {
     ATH_MSG_FATAL ("Failed to retrieve " << m_tool);
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " << m_tool);
   }

   if ( m_listOfPrivateTools.retrieve().isFailure() ) {
     ATH_MSG_ERROR ("Failed to retreive " << m_listOfPrivateTools);
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " << m_listOfPrivateTools);
   }
   
   if ( m_listOfPublicTools.retrieve().isFailure() ) {
     ATH_MSG_ERROR ("Failed to retreive " << m_listOfPublicTools);
      return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " << m_listOfPublicTools);
   }
   
   StatusCode sc = evtStore().retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR ("could not retrieve StoreGateSvc");
     return sc;
   }

   // retrieve the svc
   if ( !m_svc.retrieve().isSuccess() ) {
     ATH_MSG_ERROR ("could not retrieve [" << m_svc.typeAndName() << "] !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " << m_svc);
   }

   // check the empty private tool is empty :)
   if ( !m_emptyPrivateTool.empty() ) {
     ATH_MSG_ERROR ("Empty private tool is NOT empty !!");
   } else {
     ATH_MSG_INFO ("Empty private tool is empty (OK)");
   }

   // check the empty public tool is empty :)
   if ( !m_emptyPublicTool.empty() ) {
     ATH_MSG_ERROR ("Empty public tool is NOT empty !!");
   } else {
     ATH_MSG_INFO ("Empty public tool is empty (OK)");
   }

   ATH_CHECK( m_publicTool.retrieve() );

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TopAlgorithm::reinitialize()
{

// perform necessary re-initialization

   return initialize();
}

//___________________________________________________________________________
StatusCode TopAlgorithm::beginRun()
{

// perform work needed at start of every run

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TopAlgorithm::execute()
{

// perform work done every event

   std::string quote = "";
   if ( !m_svc->qotd( quote ).isSuccess() ) {
     ATH_MSG_ERROR ("could not retrieve the quote of the day !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("got this quote [" << quote << "].");
   }

   SomeData* data = 0;   // holder to retrieve data in, if requested

   if ( ! getInputKey().empty() ) {

   // retrieve data of type SomeData, as available under the input key
     StatusCode sc = evtStore()->retrieve( data, getInputKey() );
     if ( sc.isFailure() || ! data ) {
       ATH_MSG_ERROR ("could not retrieve data for key: " << getInputKey());
       return sc;
     }

     ATH_MSG_DEBUG
       ("read: " << data->getValue() << " from key: " << getInputKey());
     
   } else {
     ATH_MSG_DEBUG ("no reading requested");
   }

   ATH_MSG_INFO ("doing my work ...");

// calculate a new value based on the old data and the RndmFactor property
   double newvalue = ( data ? data->getValue() : 0. ) +
                     m_rndmFactor * randSvc()->engine()->rndm(); // see Algorithm base class

// make the tool do it's work on the data
   StatusCode sc = m_tool->performTask( newvalue );
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR
       ("tool " << m_tool.typeAndName() << " failed it's task ... stop!");
     return sc;
   }

// make list of private tools do their work
   ToolHandleArray< IVirtualTool >::iterator itPriv = m_listOfPrivateTools.begin(),
      itPrivEnd = m_listOfPrivateTools.end();
   for (  ; itPriv != itPrivEnd; ++itPriv ) {
      sc = (*itPriv)->performTask( newvalue );
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR 
	  ("tool " << (*itPriv).typeAndName() << " failed it's task ... stop!");
         return sc;
      }
   }

// make my special tool do its work, if present
   ToolHandle<IVirtualTool>* mySpecialTool = m_listOfPrivateTools["Special"];
   if ( mySpecialTool ) {
     ATH_MSG_INFO ("Putting my special tool at work: " << *mySpecialTool);
     sc = (*mySpecialTool)->performTask( newvalue );
     if ( sc.isFailure() ) {
       ATH_MSG_ERROR (mySpecialTool->typeAndName() << " failed it's task ... stop!");
       return sc;
     }
   }

// make list of tools do their work
   ToolHandleArray< IVirtualTool >::iterator itPub = m_listOfPublicTools.begin(),
      itPubEnd = m_listOfPublicTools.end();
   for (  ; itPub != itPubEnd; ++itPub ) {
      sc = (*itPub)->performTask( newvalue );
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR ("tool " << (*itPub).typeAndName()
		       << " failed it's task ... stop!");
	return sc;
      }
   }

// store result in a recordable object
   SomeData* newdata = new SomeData( newvalue );

   ATH_MSG_INFO ("... my work is done! ");

// put the new data into the store
   sc = evtStore()->record( newdata, getOutputKey() );
   if ( sc.isFailure() ) {
      delete newdata;    // our responsibility to delete if record fails
      ATH_MSG_ERROR ("failed to record data under key: " << getOutputKey());
      return sc;
   }

   ATH_MSG_DEBUG 
     ("wrote data: " << newdata->getValue() << " for key: " << getOutputKey());

// no deletion of newdata after successful record
   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TopAlgorithm::endRun()
{

// perform work done at end of every run

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TopAlgorithm::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}
