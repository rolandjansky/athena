/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/CalibNtupleLoader.h"

//CalibNtupleReader
#include "CalibNtupleReader/NTReader.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibEventInfo.h"

// StoreGateSvc, IncidentSvc
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Athena EventInfo
#include "EventInfo/EventType.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//root
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"

//c - c++
#include "fstream"

namespace MuonCalib {

///////////////////////
// CalibNtupleLoader //
///////////////////////
CalibNtupleLoader::CalibNtupleLoader(const std::string &t, const std::string &n, const IInterface *p): 
  AthAlgTool(t, n, p), m_prev_event_nr(-1), m_prev_run_nr(0), m_reg_sel_svc("RegionSelectionSvc", n),
  m_sgSvc("StoreGateSvc", n), m_incSvc("IncidentSvc", n) {
  m_filelist="file_list.txt";
  declareProperty("FileList", m_filelist);
  m_first = 0;
  declareProperty("FirstEvent", m_first);
  m_last = -1;
  declareProperty("LastEvent", m_last);
  m_ntuple_type="AUTO";
  declareProperty("NtupleType", m_ntuple_type);
  m_skip_double_events = false;
  declareProperty("SkipDoubleEvents", m_skip_double_events);
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
  declareProperty("StoreGateSvc", m_sgSvc);
  declareProperty("IncidentSvc", m_incSvc);
  declareInterface< CalibSegmentPreparationTool >(this);
}

////////////////
// initialize //
////////////////
StatusCode CalibNtupleLoader::initialize(void) {
  ATH_MSG_INFO( "Filelist is '"<<m_filelist<<"'" );
  //convert Ntuple Type
  if(m_ntuple_type == "AUTO") {
    m_ntuple_type_num = 0;
  } else if (m_ntuple_type == "NORMAL") {
    m_ntuple_type_num = 1;
  } else if (m_ntuple_type == "REGION")	{
    m_ntuple_type_num = 2;
  } else {
    ATH_MSG_FATAL("Invalid ntuple type. Must be AUTO/NORMAL/REGION!");
    return StatusCode::FAILURE;
  }
//read filelist an build chain
  m_chain = new TChain("/PatternNtupleMaker/Segments");
  std::string sdummy;
  int count(0);
  std::ifstream fl(m_filelist.c_str());
  if(fl.fail())	{
    ATH_MSG_INFO( "Cannot open file '"<<m_filelist<<"' for reading!" );
    return  StatusCode::FAILURE;
  }
  while (!fl.eof()) {
    fl >> sdummy;
    if (fl.eof()) break;
    if (sdummy=="[") {
      ATH_CHECK( read_dictionary(fl,count) );
      continue;
    }
    bool is_region(false);
    if(m_ntuple_type_num == 2) is_region = true;
    if(m_ntuple_type_num == 0) {
      TFile *testf=TFile::Open(sdummy.c_str());
      if(!testf) {
	ATH_MSG_WARNING( "Cannot open file '"<<sdummy.c_str()<<"'" );
	continue;
      }
      if(testf->GetKey("PatternNtupleMaker") == NULL) {
	is_region = true;
      } else {
	is_region = false;
      }
      delete testf;
    }
    if (is_region) {
      std::list<std::string> dummy_lst;
      if(m_reg_sel_svc.empty()) {
	ATH_CHECK( m_reg_sel_svc.retrieve() );
      }
      ATH_MSG_INFO( "Added "<<m_reg_sel_svc->AddRegionNtuples(sdummy.c_str(), m_chain, dummy_lst)<<" regions from file "<<sdummy );
    } else {
      m_chain->AddFile(sdummy.c_str());
      ATH_MSG_INFO( "Added file '"<<sdummy<<"' to filelist!" );
    }
    count++;
  }
  if(count==0) {
    ATH_MSG_FATAL( "No files in filelist!" );
    return  StatusCode::FAILURE;
  }
  m_reader = new NTReader(m_chain);

  // retrieve the StoreGate Service for access to the EventInfo
  ATH_CHECK( m_sgSvc.retrieve() );

  // retrieve IncidentService to fire Incidents after changing EventInfo
  ATH_CHECK( m_incSvc.retrieve() ) ;

  return StatusCode::SUCCESS;	
}  //end CalibNtupleLoader::initialize

/////////////////////
// prepareSegments //
/////////////////////
void CalibNtupleLoader::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> & /*segments*/) {
  if(m_last>0 && m_last<=m_first) {
    ATH_MSG_INFO( "Enough events read!" );
    event=NULL;
    return;
  }
  event = &(m_reader->getEvent(m_first));
  if (event->rawHitCollection()==NULL) {
    ATH_MSG_INFO( "End of file reached" );
    event=NULL;
    return;
  }
  if(m_skip_double_events) {
    while(static_cast<int>(event->eventInfo().eventNumber()) == m_prev_event_nr) {
      //			std::cout<<"Double ...."<< m_prev_event_nr <<std::endl;
      m_first++;
      if(m_last>0 && m_last<=m_first) {
	ATH_MSG_INFO( "Enough events read!" );
	event=NULL;
	return;
      }
      event = &(m_reader->getEvent(m_first));
      if (event->rawHitCollection()==NULL) {
	ATH_MSG_INFO( "End of file reached" );
	event=NULL;
	return;
      }
    }
  }
  m_prev_event_nr = event->eventInfo().eventNumber();	
  m_first++;

  // get the current EventInfo from StoreGateSvc
  const EventInfo *currentEvent(NULL) ;
  StatusCode sc = m_sgSvc->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
    ATH_MSG_INFO( "Couldnt retrieve EventInfo from StoreGateSvc" );
    event = NULL ;
    return ;
  }

  // Fire the EndEvent incident; this is normally done by the 
  // AthenaEventLoopMgr after calling the "execute" method of all
  // Algorithms
  m_incSvc->fireIncident(Incident(name(),IncidentType::EndEvent,Gaudi::Hive::currentContext()));

  // Change the EventInfo in StoreGate to the correct info from 
  // the event in the ntuple
  EventInfo *nonconst_currentEvent = 
    const_cast<EventInfo*>(currentEvent) ;
  nonconst_currentEvent->event_ID()->set_run_number(event->eventInfo().runNumber());
  nonconst_currentEvent->event_ID()->set_event_number(event->eventInfo().eventNumber());
  nonconst_currentEvent->event_ID()->set_time_stamp(event->eventInfo().timeStamp());

  ATH_MSG_DEBUG( "Hacked Run.Event,Time: "
      << "[" << currentEvent->event_ID()->run_number()
      << "." << currentEvent->event_ID()->event_number()
      << "," << currentEvent->event_ID()->time_stamp()
      << "]" );

  // Fire EndRun and BeginRun Incidents if the run number changed
  if ( event->eventInfo().runNumber() != m_prev_run_nr ) {
    m_prev_run_nr = event->eventInfo().runNumber() ;
    m_incSvc->fireIncident(Incident(name(),"EndRun"));
    m_incSvc->fireIncident(Incident(name(),IncidentType::BeginRun,Gaudi::Hive::currentContext()));
  }

  // Fire BeginEvent Incident
  m_incSvc->fireIncident(Incident(name(),IncidentType::BeginEvent,Gaudi::Hive::currentContext()));

} //end CalibNtupleLoader::prepareSegments

inline StatusCode CalibNtupleLoader::read_dictionary(std::istream &fl, int &tot_count)	{
  MsgStream log(msgSvc(), name());
  std::string filename;
  fl>>filename;
  if(fl.eof()) {
    ATH_MSG_FATAL( "Filelist has wrong format" );
    return StatusCode::FAILURE;
		}
//read regions
  int count=0;
  std::list<std::string> regions;
  while(1) {
    std::string region;
    fl>>region;
    if(region=="]") {
      break;
    }
    if(fl.eof()) {
      ATH_MSG_FATAL( "Filelist has wrong format" );
      return StatusCode::FAILURE;
    }
    if (region=="PatternNtupleMaker") {
      m_chain->AddFile(filename.c_str());
      count++;
    } else {
      regions.push_back(region);
    }
  }
  if(regions.size()>0) {
    if(m_reg_sel_svc.empty()) {
      ATH_CHECK( m_reg_sel_svc.retrieve() );
    }
    count+=m_reg_sel_svc->AddRegionNtuples(filename.c_str(), m_chain, regions);
  }
  if (count>0) {
    ATH_MSG_INFO( "Added " << count << " regions from "<< filename );
  }
  tot_count+=count;
  return StatusCode::SUCCESS;
}  //end CalibNtupleLoader::read_dictionary

} //namespace MuonCalib
