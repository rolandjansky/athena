/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventCount.cxx
 * @brief Implementation of class EventCount
 */
 
#include "EventCount.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"
#include "EventInfo/EventInfo.h"
//#include "EventInfo/EventType.h"
//#include "EventInfo/EventID.h"
//#include "EventInfo/TriggerInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"

#include "xAODCutFlow/CutBookkeeperContainer.h"

//--------------- Utility Struct Constructors ----------------
EventCount::ObjSum::ObjSum() : num(-1)
{keys.clear();}

EventCount::ObjSum::ObjSum(int n) : num(n)
{keys.clear();}

//___________________________________________________________________________
EventCount::EventCount(const std::string& name, ISvcLocator* pSvcLocator) : 
   AthAlgorithm(name, pSvcLocator), 
   m_dump(false),
   m_pCLIDSvc( "ClassIDSvc",   name ),
   m_nev(0),
   m_first(-1),
   m_final(-1),
   m_firstlb(-1),
   m_finallb(-1),
   m_currentRun(-1)
{
   // Declare the properties
   declareProperty("Dump", m_dump,"Bool of whether to dump object summary");
}

EventCount::~EventCount()
{}

StatusCode EventCount::initialize() 
{
   // clear containers
   m_runs.clear();
   m_clids.clear();
   m_summaries.clear();
   //m_prov_keys.clear();
   m_guids.clear();

   ATH_MSG_INFO ( "in initialize()" );

   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   ATH_CHECK( incSvc.retrieve() );
   incSvc->addListener(this, "BeginInputFile", 100); 

   return StatusCode::SUCCESS;
}

void EventCount::handle(const Incident& inc)
{
  
  ATH_MSG_DEBUG("handle() " << inc.type());

  // Need to get input file name for event comparison
  if (inc.type()=="BeginInputFile") {
    const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
    m_currentFile = "Undefined";
    if (fileInc != 0) { 
      m_currentFile = fileInc->fileName();
    } 
    else { 
      ATH_MSG_ERROR("Could not get file name at BeginInputFile");
    }
    ServiceHandle<StoreGateSvc> mdstore("StoreGateSvc/InputMetaDataStore", name());
    if (mdstore.retrieve().isSuccess()) {
      const DataHandle<xAOD::CutBookkeeperContainer> compBook(NULL);
      if (mdstore->retrieve(compBook, "CutBookkeepers").isSuccess()) {
        ATH_MSG_INFO("CBK size = " << compBook->size());
        for (auto it = compBook->begin(); it != compBook->end(); ++it) {
          ATH_MSG_INFO("CBK name= " << (*it)->name() << " stream=" << (*it)->inputStream() << " N=" << (*it)->nAcceptedEvents() << " W=" << (*it)->sumOfEventWeights());
        }
      } else {
        ATH_MSG_INFO("CBK No bookkeepers " << mdstore->dump());
      }
      const DataHandle<xAOD::CutBookkeeperContainer> incompBook(NULL);
      if (mdstore->retrieve(incompBook, "IncompleteCutBookkeepers").isSuccess()) {
        ATH_MSG_INFO("CBK size = " << incompBook->size());
        for (auto it = incompBook->begin(); it != incompBook->end(); ++it) {
          ATH_MSG_INFO("CBK name= " << (*it)->name() << " stream=" << (*it)->inputStream() << " N=" << (*it)->nAcceptedEvents() << " W=" << (*it)->sumOfEventWeights());
        }
      } else {
        ATH_MSG_INFO("CBK No bookkeepers " << mdstore->dump());
      }
    }
  }
}

StatusCode EventCount::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );

   // Get the event header, print out event and run number
   ATH_MSG_INFO ( evtStore()->dump() );
   const DataHandle<xAOD::EventInfo> evt;
   ATH_CHECK( evtStore()->retrieve(evt) );
   if (!evt.isValid()) {
      ATH_MSG_FATAL ( "Could not find event" );
      return(StatusCode::FAILURE);
   }
   ++m_nev;
   if (m_nev==1) {
      m_first = evt->eventNumber(); 
      m_firstlb= evt->lumiBlock();
   }
   std::map<std::string,std::set<EventID> >::iterator kit = m_fileEvents.find(m_currentFile);
   if (kit==m_fileEvents.end()) {
     std::set<EventID> ini;
     ini.insert(EventID(evt->runNumber(),evt->eventNumber()));
     m_fileEvents.insert(std::make_pair(m_currentFile,ini));
   }
   else {
     kit->second.insert(EventID(evt->runNumber(),evt->eventNumber()));
   }
   // track final event number
   m_final = evt->eventNumber();
   m_finallb= evt->lumiBlock();
   // keep list of runs
   if (m_currentRun!=static_cast<int>(evt->runNumber())) {
      m_currentRun =static_cast<int>(evt->runNumber());
      m_runs.push_back(static_cast<int>(evt->runNumber()));
   }
   // keep list of event types
   if (evt->eventType(xAOD::EventInfo::EventType::IS_SIMULATION)) {
      m_types.insert("Simulation");
   }
   else {m_types.insert("Data");}
   if (evt->eventType(xAOD::EventInfo::EventType::IS_TESTBEAM)) {
      m_types.insert("Testbeam");
   }
   else {m_types.insert("Detector");}
   if (evt->eventType(xAOD::EventInfo::EventType::IS_CALIBRATION)) {
      m_types.insert("Calibration");
   }
   else {m_types.insert("Physics");}

   // Keep track of streams
   std::vector<xAOD::EventInfo::StreamTag>::const_iterator STit  = evt->streamTags().begin();
   std::vector<xAOD::EventInfo::StreamTag>::const_iterator STend = evt->streamTags().end();
   while (STit != STend) {
     // check if stream is in list, if not add it, otherwise increment it
     if (m_streams.find(STit->name()) == m_streams.end()) {
       m_streams.insert(make_pair(STit->name(),1));
     }
     else {
       m_streams[STit->name()] += 1;
     }
     ++STit;
   }
   

   ATH_MSG_DEBUG( "EventInfo event: " << evt->eventNumber() 
                  << " run: " << evt->runNumber() );
   // 
   // Now check what objects are in storegate
   //
   //if (m_dump) {
       const DataHandle<DataHeader> beg; 
       const DataHandle<DataHeader> ending; 
       StatusCode status = evtStore()->retrieve(beg,ending);
       if (status.isFailure() || beg==ending) {
     	 ATH_MSG_DEBUG ( "No DataHeaders present in StoreGate" );
       }
       else {
         // Find input data header
         for (; beg != ending; ++beg) {
             if (beg->isInput()) {
	         DataHeader doh = *beg;
	         ATH_MSG_DEBUG ( "DataHeader with key " 
                                 << beg.key() << " is input" );

                 std::vector<DataHeaderElement>::const_iterator it = doh.begin();
                 std::vector<DataHeaderElement>::const_iterator last = doh.end();
                 std::vector<DataHeaderElement>::const_iterator it_pr = doh.beginProvenance();
                 std::vector<DataHeaderElement>::const_iterator last_pr = doh.endProvenance();

	         // Loop over DataHeaderElements
	         ATH_MSG_DEBUG ( " About to loop over DH elements" );
                 int ccnt = 0;
	         for (; it!=last; ++it) {
	             std::set<CLID> list = it->getClassIDs();
	             std::set<CLID>::iterator c = list.begin();
	             // Loop over clids in dhe
                     int dcnt = 0;
	             while (c!=list.end()) {
                       ATH_MSG_DEBUG ( ccnt << " " << dcnt << " Inserting CLID " << *c 
                                       << " " << it->getKey() );
                         // Look for clid in current list
		         std::map<int,int>::iterator cpos = m_clids.find(*c);
		         if (cpos==m_clids.end()) {
		             // if it doesn't exist, make it and set count to 1
		 	     m_clids.insert(std::make_pair(*c,1));
		         }
		         else {
		             // if it does exist, increment counter
			     cpos->second++;
		         }
                         // For this clid look for key in summary
		         std::map<int,ObjSum>::iterator spos = m_summaries.find(*c);
		         if (spos==m_summaries.end()) {
		             // if it doesn't exist, make it and set count to 1
			     ObjSum sum(1);
			     sum.keys.insert(it->getKey());
			     m_summaries.insert(std::make_pair(*c,sum));
		         }
		         else {
		             // if it does exist, increment counter
			     ObjSum sum = spos->second;
			     sum.num++;
			     sum.keys.insert(it->getKey());
			     spos->second=sum;
		         }
	                 ++c;
                         ++dcnt;
	             }
                     ++ccnt;
	         }
	         
                 // Now Loop over Provenance records
	         for (; it_pr!=last_pr; ++it_pr) {
                    // For this clid look for key in summary
		    //std::set<std::string>::iterator spos = m_prov_keys.find(it_pr->getKey());
		    //if (spos==m_prov_keys.end()) {
		        // if it doesn't exist, add key to list
/*
                        std::pair<std::set<std::string>::iterator,bool> ins_stat; 
		        ins_stat = m_prov_keys.insert(it_pr->getKey());
                        if (ins_stat.second) {
  	                    log << MSG::DEBUG << "Provenance key " 
                                              << it_pr->getKey() << endreq;
                        }
*/
                        // Add token to list, and associate it with the key
                        const Token* tk = it_pr->getToken();
                        std::pair<std::map<Guid,std::string>::iterator,bool> ins_stat2; 
                        ins_stat2 = m_guids.insert(std::make_pair(tk != 0 ? tk->dbID() : Guid::null(),it_pr->getKey()));
                        if (ins_stat2.second) {
                          ATH_MSG_DEBUG ( "Provenance key " 
                                          << it_pr->getKey() );
                          ATH_MSG_DEBUG ( "New provenance token found " 
                                          << (tk != 0 ? tk->toString() : "") );
                        }
/*
                        else {
                            log << MSG::INFO << "Provenance key " << it_pr->getKey() 
                                             << " appears multiple times" << endreq;
                        }
*/
    	            //}
	         }
 	     }  // input check if, else
	     else {
               ATH_MSG_DEBUG ( "Not input header" );
	     }
          }  // loop over data headers
       }  
   //}  // end of dump section
   
   return(StatusCode::SUCCESS);
}

StatusCode EventCount::finalize() 
{
   ATH_MSG_DEBUG ( "in m_finalize()" );
   
   // file event overlaps
   ATH_MSG_INFO ( "-- OUTPUT FILE EVENT OVERLAP SUMMARY --" );
   for (std::map<std::string, std::set<EventID> >::const_iterator fit = m_fileEvents.begin();
        fit != m_fileEvents.end(); ++fit) {
      std::string curfile = fit->first;
      std::set<EventID> curList = fit->second;
      ATH_MSG_INFO ( "File " << curfile << " " << curList.size() << " events" );
      for (std::map<std::string, std::set<EventID> >::const_iterator fit2 = m_fileEvents.begin();
          fit2 != m_fileEvents.end(); ++fit2) {
        if (fit2->first != curfile) {
	   std::vector<EventID> test; 
           std::set_intersection(curList.begin(),curList.end(),
                                 fit2->second.begin(),fit2->second.end(),
                                 std::back_inserter(test));
           ATH_MSG_INFO ( "--> overlap fraction " << fit2->first 
                          << " " << float(test.size())/float(curList.size()) );
        }
      }
   }

   //
   // Present summary information regardless of dump setting
   //
   // Show beginning and ending event numbers
   //
   ATH_MSG_INFO ( "---------- INPUT FILE SUMMARY ----------" );
   ATH_MSG_INFO ( "Input contained: " << m_nev << " events" );
   ATH_MSG_INFO ( " -- Event Range ( " << m_first
                  << " .. " <<  m_final << " )" );
   ATH_MSG_INFO ( " -- Lumiblock Range ( " << m_firstlb
                  << " .. " <<  m_finallb << " )" );
   ATH_MSG_INFO ( "Input contained: " << m_runs.size() << " runs" );

   msg() << MSG::INFO << " --";
   // List all the runs
   std::vector<int>::iterator it = m_runs.begin();
   while (it!=m_runs.end()) {
     msg() << " " << *it; ++it;
   }
   msg() << endreq;
   // List all the event types
   ATH_MSG_INFO ( "Input contained the following Event Types" );
   std::set<std::string>::iterator itype=m_types.begin();
   while (itype != m_types.end()) {
      ATH_MSG_INFO ( " -- " << *itype );
      ++itype;
   }
   // List all the event streams
   ATH_MSG_INFO ( "Input contained the following Streams" );
   std::map<std::string,int>::iterator istr=m_streams.begin();
   while (istr != m_streams.end()) {
      ATH_MSG_INFO ( " -- " << istr->first << " (" << istr->second <<")" );
      ++istr;
   }
/*
   log << MSG::INFO << "Input contained the following Provenance Tags" << endreq;
   std::set<std::string>::iterator ipr = m_prov_keys.begin();
   while (ipr!=m_prov_keys.end()) {
      log << MSG::INFO << " -- " << *ipr << endreq;
      ++ipr;
   }
*/
   ATH_MSG_INFO ( "Input contained references to the following File GUID's" );
   std::string fkey = "";
   std::map<Guid,std::string>::iterator ifr = m_guids.begin();
   while (ifr!=m_guids.end()) {
      if (ifr->second != fkey) {
          fkey = ifr->second;
          ATH_MSG_INFO ( " -> " << fkey );
      }
      ATH_MSG_INFO ( "      - " << ifr->first );
      ++ifr;
   }
   //
   // If dump flag set, then show full contents
   //
   if (m_dump) {
      ATH_MSG_INFO ( "Input contained the following CLIDs and Keys" );
      // Get pointer to classid service
      bool clidTran = false;
      if (m_pCLIDSvc.retrieve().isSuccess()) {
	  clidTran = true;
      }
      else {
        ATH_MSG_WARNING ( "Could not locate ClassIDSvc" );
      }
      //log << MSG::INFO << "--------- D A T A - O B J ---------" << endreq;
      std::map<int,int>::iterator i = m_clids.begin();
      std::string classname = "NONE";
      StatusCode clidsvc_sc;
      while (i!=m_clids.end()) {
          classname = "Unknown";
          if(clidTran) clidsvc_sc = m_pCLIDSvc->getTypeNameOfID(i->first,classname);
          if (!clidsvc_sc.isSuccess())
            ATH_MSG_DEBUG ("CLIDSvc bad return code" );
          ATH_MSG_INFO ( " -> " << i->first << " " 
                           << classname
		           << " (" << i->second << ") "  );
	  // Now check for key list
	  std::map<int,ObjSum>::iterator keyloc = m_summaries.find(i->first);
	  if (keyloc != m_summaries.end()) {
	      ObjSum out = keyloc->second;
	      std::set<std::string>::iterator ot = out.keys.begin();
	      while(ot != out.keys.end()) {
                  ATH_MSG_INFO ( "      - " << *ot );
		  ++ot;
	      }
	  } 
          ++i;
      }
      ATH_MSG_INFO ( "----------------------------------------" );
   }
   return(StatusCode::SUCCESS);
}
