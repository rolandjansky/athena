///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MenuAwareDQMonitoring.cxx 
// Implementation file for class MenuAwareDQMonitoring
// Author: Tatsiana Klimkovich <tklimk@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TrigHLTMonitoring includes
#include "TrigHLTMonitoring/MenuAwareDQMonitoring.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// Athena includes
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

//// Gaudi includes
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/GaudiException.h"
//#include "GaudiKernel/IToolSvc.h"
//
//// Event Info
//#include "EventInfo/EventIncident.h"
//#include "EventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"
//#include "EventInfo/EventType.h"

//#include "CoolKernel/Record.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// for online testing
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdint.h>


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MenuAwareDQMonitoring::MenuAwareDQMonitoring( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
   m_MCK("0")
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
}

// Destructor
///////////////
MenuAwareDQMonitoring::~MenuAwareDQMonitoring()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode MenuAwareDQMonitoring::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  myfile.open ("example.txt");

  //StatusCode sc;
  MsgStream log(msgSvc(), name());
  log <<MSG::DEBUG <<"in initialize()" <<endmsg;
 
  // Storegate
  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to get the StoreGateSvc" << endmsg;
      return sc;
     }

 // locate the conditions store ptr to it.
  sc = service("DetectorStore", m_detStore);
  if (!sc.isSuccess() || 0 == m_detStore)  {
    log <<MSG::ERROR <<"Could not find DetStore" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MenuAwareDQMonitoring::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //myfile.close();
  return StatusCode::SUCCESS;
}

StatusCode MenuAwareDQMonitoring::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  ATH_MSG_INFO ("This is the message of Tanya!");
  
  // here we are going to read MCK number according to IOV
  
  StatusCode sc;

  //const AthenaAttributeList*    attrList     = 0;
  const CondAttrListCollection* attrListColl = 0;  
  //const CondAttrListCollection* attrListColl = 0;  
  //const CondAttrListCollection* attrListColl = 0;  
  
  // AttrList
  sc = m_detStore->retrieve(attrListColl, "/TRIGGER/HLT/MenuAwareDQMonitoring5");
  if (sc.isFailure()) {
     ATH_MSG_ERROR ("Could not retrieve MenuAwareDQMonitoring5");
     //log <<MSG::ERROR <<"Could not retrieve IOVDbTestAttrList" <<endmsg;
     // Using COOL, is failure
     return( StatusCode::FAILURE);
  }

  std::ostringstream attrStr2;
        
  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
    std::ostringstream attrStr1;
    (*first).second.toOutputStream( attrStr1 );
    ATH_MSG_DEBUG  ("ChanNum " << (*first).first);
    // print out the name if present
    if (attrListColl->name_size()>0) {
      CondAttrListCollection::name_const_iterator nitr=attrListColl->chanNamePair((*first).first);
      if (nitr!=attrListColl->name_end())
	ATH_MSG_DEBUG( " name " << nitr->second);
    }
    ATH_MSG_DEBUG(" Attribute list " << attrStr1.str() );

    const coral::AttributeList& attrList = (*first).second;

    int mck = attrList["MCKdata"].data<uint32_t>();
    
    myfile << mck << " \n";
    myfile.close(); 
    // Print out range if it exits
    CondAttrListCollection::ChanNum chanNum = (*first).first;
    CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
    if (iovIt != attrListColl->iov_end()) {
      const IOVRange& range = (*iovIt).second;
      if(range.start().isTimestamp()) {
	ATH_MSG_DEBUG ("Range timestamp : since " << range.start().timestamp()
		       << " till " << range.stop().timestamp() );
      }
      else {
	ATH_MSG_DEBUG("Range R/E : since " << range.start().run() << " "
	    << range.start().event()
	    << " till " << range.stop().run() << " "
		      << range.stop().event());
   }
    }
    else {
      ATH_MSG_DEBUG("No range found ");
    }
  }
  
  

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

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


