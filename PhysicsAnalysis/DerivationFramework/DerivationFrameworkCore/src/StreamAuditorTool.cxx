/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// StreamAuditorTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Part of the DerivationFramework
// This tool keeps count of event selections in each stream, and provides
// a summary of	overall counts and overlaps at the end. Tool must be
// passed to each instance of the derivation kernel

#include "DerivationFrameworkCore/StreamAuditorTool.h"
#include <vector>
#include <string>


// Constructor
DerivationFramework::StreamAuditorTool::StreamAuditorTool( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_incidentSvc("IncidentSvc",n)
  //m_algNames,
  //m_algCounts,
  //m_overlapCounts,
  {
    declareInterface<DerivationFramework::StreamAuditorTool>(this);
  }
  
// Destructor
DerivationFramework::StreamAuditorTool::~StreamAuditorTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::StreamAuditorTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     // Get the incident service (so the tool knows when the end of an event is encountered)
     if (m_incidentSvc.retrieve().isFailure()) {
       ATH_MSG_FATAL("Could not retrieve IncidentService '" << m_incidentSvc << "'. Exiting.");
       return StatusCode::FAILURE;
     }
     // register to the incident service: BeginEvent for TrackRecordCollection
     m_incidentSvc->addListener( this, IncidentType::EndEvent);
     m_totalEvents=0;

     return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::StreamAuditorTool::finalize()
{
     ATH_MSG_INFO( "========================================================");
     ATH_MSG_INFO( "|| SUMMARY OF THE DERIVATION FRAMEWORK STREAM AUDITOR ||");
     ATH_MSG_INFO( "========================================================");
     ATH_MSG_INFO( "Total number of events processed: " << m_totalEvents); 
     std::map<std::pair<std::string,std::string> , unsigned int>::iterator mapItr;
     ATH_MSG_INFO( "Stream event overlaps follow:");
     for (mapItr=m_overlapMap.begin(); mapItr!=m_overlapMap.end(); ++mapItr) {
     	ATH_MSG_INFO( ((*mapItr).first).first << " " << ((*mapItr).first).second << " " << (*mapItr).second); 
     }
     ATH_MSG_INFO( "========================================================");
     return StatusCode::SUCCESS;
}

// Called by each instance of the kernel
void DerivationFramework::StreamAuditorTool::registerAlgorithmName(const std::string &name) 
{
     // Build the name list 	
     m_algNames[name]=0;	

     // Build the overlap table
     std::map<std::string,unsigned int>::iterator mapItrOuter;
     std::map<std::string,unsigned int>::iterator mapItrInner;
     for (mapItrOuter=m_algNames.begin(); mapItrOuter!=m_algNames.end(); ++mapItrOuter) {
        for (mapItrInner=mapItrOuter; mapItrInner!=m_algNames.end(); ++mapItrInner) {
	   std::pair<std::string,std::string> tmpPair;
	   tmpPair = std::make_pair( (*mapItrOuter).first, (*mapItrInner).first);
	   m_overlapMap[tmpPair]=0;
	}
     }	

     return; 
}  

// Called by kernel whenever an event is accepted
void DerivationFramework::StreamAuditorTool::eventAccepted(const std::string &name) 
{
     std::map<std::string,unsigned int>::iterator mapItr;
     mapItr = m_algNames.find(name);
     (*mapItr).second += 1; 

     return;           
} 

// framework handle - called whenever end-of-event incident encountered
void DerivationFramework::StreamAuditorTool::handle( const Incident& inc ) {

  // check the incident type
  if ( inc.type() == IncidentType::EndEvent ) {

    // Run if end of event 
    ++m_totalEvents;
    std::map<std::string,unsigned int>::iterator mapItrOuter;
    std::map<std::string,unsigned int>::iterator mapItrInner;
    std::map<std::pair<std::string,std::string> , unsigned int>::iterator olItr;
    for (mapItrOuter=m_algNames.begin(); mapItrOuter!=m_algNames.end(); ++mapItrOuter) {
	std::string outerString = (*mapItrOuter).first;
	unsigned int outerCount = (*mapItrOuter).second;
	for (mapItrInner=mapItrOuter; mapItrInner!=m_algNames.end(); ++mapItrInner) { 
		std::string innerString = (*mapItrInner).first;
		unsigned int innerCount = (*mapItrInner).second;		    
 		for (olItr=m_overlapMap.begin(); olItr!=m_overlapMap.end(); ++olItr) {
			if ( (((*olItr).first).first==outerString) && (((*olItr).first).second==innerString) && (innerCount==1 && outerCount==1) ) {
				(*olItr).second += 1;
				break;
			}
		}
	}
    }
    std::map<std::string,unsigned int>::iterator mapItr;
    for (mapItr=m_algNames.begin(); mapItr!=m_algNames.end(); ++mapItr) {    	 
    	(*mapItr).second = 0;
    }
  }

    return;
}
    	   
