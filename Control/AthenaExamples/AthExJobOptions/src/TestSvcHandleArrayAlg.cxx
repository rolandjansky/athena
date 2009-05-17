/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/TestSvcHandleArrayAlg.h"
#include "AthExJobOptions/IVirtualSvc.h"


/////////////////////////////////////////////////////////////////////////////

TestSvcHandleArrayAlg::TestSvcHandleArrayAlg( const std::string& name, ISvcLocator* svcloc ) :
  AthAlgorithm( name, svcloc ),
  m_listOfSvcs(name)
{
   declareProperty( "SvcList",    m_listOfSvcs,    "Get important news from a service" );
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::initialize()
{

// perform necessary one-off initialization

   // retrieve the svc
   if ( !m_listOfSvcs.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("could not retrieve [" << m_listOfSvcs << "] !");
      return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " <<m_listOfSvcs);
   }

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::reinitialize()
{

// perform necessary re-initialization

   return initialize();
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::beginRun()
{

// perform work needed at start of every run

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::execute()
{
  std::string quote = "", totalquote="";
  StatusCode sc;

  ServiceHandleArray< IVirtualSvc >::iterator itSvc = m_listOfSvcs.begin();
  
  for (  ; itSvc != m_listOfSvcs.end(); ++itSvc ) {
    sc = (*itSvc)->qotd(quote);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR
	("Service " << (*itSvc) << " failed to get quote ... stop!");
      return sc;
    }
    totalquote+=quote+"\n";
  } 
  ATH_MSG_INFO ("got this quote [" << totalquote << "].");
  
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::endRun()
{
  
   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode TestSvcHandleArrayAlg::finalize()
{

   return StatusCode::SUCCESS;
}
