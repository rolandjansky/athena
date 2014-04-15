/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventCheck.cxx
 * @brief Implementation of class EventCheck
 */
 
#include "EventCheck.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "PersistentDataModel/DataHeader.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"

//___________________________________________________________________________
EventCheck::EventCheck(const std::string& name, ISvcLocator* pSvcLocator) : 
   Algorithm(name, pSvcLocator), 
   m_sGevent(0),
   m_pCLIDSvc(0),
   m_chkData(false),
   m_maxCheck(10),
   m_nev(0)
{
   // Declare the properties
   declareProperty("CheckData", m_chkData);
   declareProperty("MaxCheck", m_maxCheck);
   declareProperty("ExcludeCLIDS", m_exclude);
   declareProperty("SelectClasses", m_toCheck);
}

EventCheck::~EventCheck()
{}

StatusCode EventCheck::initialize() 
{
   // clear containers
   m_success.clear();
   m_failures.clear();
   m_badclids.clear();
   m_selectedclids.clear();

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = service("StoreGateSvc", m_sGevent);
   if (!sc.isSuccess() || 0 == m_sGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      sc = StatusCode::FAILURE;
   }

   sc = service("ClassIDSvc", m_pCLIDSvc, true);
   if (!sc.isSuccess() || 0 == m_pCLIDSvc) {
      log << MSG::ERROR << "Could not find ClassIDSvc" << endreq;
      sc = StatusCode::FAILURE;
   }

   return(sc);
}

StatusCode EventCheck::execute() 
{
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   // Get the event header, print out event and run number
   const DataHandle<EventInfo> evt;
   sc = m_sGevent->retrieve(evt);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   if (!evt.isValid()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   ++m_nev;
   log << MSG::DEBUG << "EventInfo event: " << evt->event_ID()->event_number() 
	            << " run: " << evt->event_ID()->run_number() << endreq;
   if (m_nev==1) {
       // setup exclude set
       std::vector<int>::const_iterator iter = m_exclude.value().begin();
       while (iter != m_exclude.value().end()) {
	   log << MSG::DEBUG << "Excluding " << *iter << endreq;
           m_badclids.insert(*iter);
           ++iter;
       }
       // setup select set
       std::vector<std::string>::const_iterator itstr = m_toCheck.value().begin();
       while (itstr != m_toCheck.value().end()) {
           CLID id;
           sc = m_pCLIDSvc->getIDOfTypeName(*itstr, id);
           if (sc.isSuccess()) {
               log << MSG::DEBUG << "Selecting " << *itstr << "/" << id << endreq;
               m_selectedclids.insert(id);
           }
           else {
               log << MSG::DEBUG << "Could not find clid for " << *itstr << endreq;
           }
           ++itstr;
       }
   }

   log << MSG::DEBUG << "will check " << m_maxCheck << " events" << endreq;
   // 
   // Now check what objects are in storegate
   //
   const DataHandle<DataHeader> beg; 
   const DataHandle<DataHeader> ending; 
   StatusCode status = m_sGevent->retrieve(beg,ending);
   if (status.isFailure() || beg==ending) {
    	log << MSG::DEBUG << "No DataHeaders present in StoreGate"
	    << endreq;
   }
   else {
       // Find input data header
       for (; beg != ending; ++beg) {
           if (beg->isInput()) {
	       DataHeader localDH = *beg;
	       log << MSG::DEBUG << "DataHeader with key " 
		   << beg.key() << " is input" << endreq;
	       std::vector<DataHeaderElement>::const_iterator it = localDH.begin();
	       std::vector<DataHeaderElement>::const_iterator last = localDH.end();
	       // Loop over DataHeaderElements
	       log << MSG::DEBUG << " About to loop over DH elements" << endreq;
	       for (; it!=last; ++it) {
	           std::set<CLID> list = it->getClassIDs();
	           std::set<CLID>::iterator c = list.begin();
	           // Loop over clids in dhe
	           while (c!=list.end()) {
		       log << MSG::DEBUG << "Inserting CLID " << *c << endreq;
		       // -----------------------------------------
		       // Check loading of objects in DataHeader 
		       // on the condition that
		       //   - requested the check
		       //   - not too many events
		       //   - not the DataHeader
		       //   - not a known problem
		       // -----------------------------------------
		       if ( m_chkData && 
		            m_nev <= m_maxCheck && 
			    *c != 222376821 &&
			    m_badclids.find(*c) == m_badclids.end() &&
                            (m_selectedclids.size() == 0 || 
                             m_selectedclids.find(*c) != m_selectedclids.end())
			  ) {
			   DataObject* test=0;
			   test = m_sGevent->accessData(*c,it->getKey());
			   if (!static_cast<bool>(test)) {
			      log << MSG::DEBUG 
				  << " accessData failed for CLID " << *c
				  << " for key " << it->getKey()
				  << " for event "
				  << evt->event_ID()->event_number()
				  << endreq;
			      m_failures.insert(*c);
			   }
			   else { m_success.insert(*c); }
		       }
	               ++c;
	           }
	       }
 	   }
	   else {
	       log << MSG::DEBUG << "Not input header" << endreq;
	   }
       }
   }
   
   return(StatusCode::SUCCESS);
}

StatusCode EventCheck::finalize() 
{
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in finalize()" << endreq;
   if (m_pCLIDSvc) {
       StatusCode sc;
       std::string classname = "NONE";
       // Print out any successes
       if (m_success.size() > 0) {
           log << MSG::INFO << "-------***- S U C C E S S -***-------" << endreq;
           std::set<int>::iterator it = m_success.begin();
           while (it != m_success.end()) {
               sc = m_pCLIDSvc->getTypeNameOfID(*it,classname);
               if (sc.isFailure()) classname = "Unknown";
               log << MSG::INFO << "  " << *it << " " << classname << endreq;
               ++it;
           } 
           log << MSG::INFO << "-------***-----------------***-------" << endreq;
       }
       // print out any failures
       if (m_failures.size() > 0) {
           log << MSG::INFO << "-------((- F A I L U R E S -))-------" << endreq;
	   std::set<int>::iterator it = m_failures.begin();
           while (it != m_failures.end()) {
               sc = m_pCLIDSvc->getTypeNameOfID(*it,classname);
               if (sc.isFailure()) classname = "Unknown";
               log << MSG::INFO << "  " << *it << " " << classname << endreq;
               ++it;
           } 
           log << MSG::INFO << "-------((-------------------))-------" << endreq;
       }
       // print out any known problem classes
       if (m_badclids.size() > 0) {
	   log << MSG::INFO << "----------- S K I P P E D -----------" << endreq;
	   std::set<int>::iterator it = m_badclids.begin();
           while (it != m_badclids.end()) {
               sc = m_pCLIDSvc->getTypeNameOfID(*it,classname);
               if (sc.isFailure()) classname = "Unknown";
               log << MSG::INFO << "  " << *it << " " << classname << endreq;
               ++it;
           } 
           log << MSG::INFO << "-------------------------------------" << endreq;
       }
   }
   else {
       log << MSG::INFO << "Could not initialize ClassIDSvc" << endreq;
   }
   return(StatusCode::SUCCESS);
}
