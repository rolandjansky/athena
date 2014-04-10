/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Athena EventInfo
#include "EventInfo/EventIncident.h"
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

CalibNtupleLoader :: CalibNtupleLoader(const std::string & t, const std::string & n, const IInterface *p): 
  AlgTool(t, n, p), m_prev_event_nr(-1), m_prev_run_nr(0), p_reg_sel_svc(NULL)
	{
	m_filelist="file_list.txt";
	declareProperty("FileList", m_filelist);
	m_first = 0;
	declareProperty("FirstEvent", m_first);
	m_last = -1;
	declareProperty("LastEvent", m_last);
	m_ntuple_type="AUTO";
	declareProperty("NtupleType", m_ntuple_type);
	m_scip_double_events = false;
	declareProperty("ScipDoubleEvents", m_scip_double_events);
	
	declareInterface< CalibSegmentPreparationTool >(this);
	}

	
////////////////
// initialize //
////////////////

StatusCode CalibNtupleLoader :: initialize(void)
	{
	MsgStream log(msgSvc(), name());
	log<< MSG::INFO << "Filelist is '"<<m_filelist<<"'" <<endreq;
//convert Ntuple Type
	if(m_ntuple_type == "AUTO")
		{
		m_ntuple_type_num = 0;
		}
	else if (m_ntuple_type == "NORMAL")
		{
		m_ntuple_type_num = 1;
		}
	else if (m_ntuple_type == "REGION")
		{
		m_ntuple_type_num = 2;
		}
	else
		{
		log<<MSG::FATAL<<"Invalid ntuple type. Must be AUTO/NORMAL/REGION!"<<endreq;
		return StatusCode :: FAILURE;
		}
//read filelist an build chain
	m_chain = new TChain("/PatternNtupleMaker/Segments");
	std::string sdummy;
	int count(0);
	std::ifstream fl(m_filelist.c_str());
	if(fl.fail())
		{
		log<< MSG::INFO << "Cannot open file '"<<m_filelist<<"' for reading!" <<endreq;
		return  StatusCode::FAILURE;
		}
	while (!fl.eof()) 
		{
		fl >> sdummy;
		if (fl.eof()) 
			break;
		if (sdummy=="[")
			{
			StatusCode sc=read_dictionary(fl,count);
			if(!sc.isSuccess())
				{
				return sc;
				}
			continue;
			}
		bool is_region(false);
		if(m_ntuple_type_num == 2)
			is_region = true;
		if(m_ntuple_type_num == 0)
			{
			TFile *testf=TFile::Open(sdummy.c_str());
			if(!testf)
				{
				log<<MSG::WARNING<<"Cannot open file '"<<sdummy.c_str()<<"'"<<endreq;
				continue;
				}
			if(testf->GetKey("PatternNtupleMaker") == NULL)
				{
				is_region = true;
				}
			else
				{
				is_region = false;
				}
			delete testf;
			}
		if (is_region)
			{
			std::list<std::string> dummy_lst;
			if(p_reg_sel_svc == NULL)
				{
				StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
				if(!sc.isSuccess())
					{
					log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endreq;
				return sc;
					}	
				}
			log<<MSG::INFO<< "Added "<<p_reg_sel_svc->AddRegionNtuples(sdummy.c_str(), m_chain, dummy_lst)<<" regions from file "<<sdummy<<endreq;
			}
		else
			{
			m_chain->AddFile(sdummy.c_str());
			log<< MSG::INFO << "Added file '"<<sdummy<<"' to filelist!" <<endreq;
			}
		count++;
		}
	if(count==0)
		{
		log<< MSG::FATAL<< "No files in filelist!"<<endreq;
		return  StatusCode::FAILURE;
		}
	m_reader = new NTReader(m_chain);

  // retrieve the StoreGate Service for access to the EventInfo
  StatusCode sc = service("StoreGateSvc",m_sgSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return sc ;
  }
  else 
    log << MSG::INFO << "StoreGateSvc retrieved!" << endreq;
  // retrieve IncidentService to fire Incidents after changing EventInfo
  sc = service("IncidentSvc",m_incSvc) ;
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not retrieve IncidentSvc!" << endreq;
    return sc ;
  }
  else 
    log << MSG::INFO << "IncidentSvc retrieved!" << endreq;


	return StatusCode :: SUCCESS;	
	}


/////////////////////
// prepareSegments //
/////////////////////

void CalibNtupleLoader :: prepareSegments(const MuonCalibEvent *& event, std::map<NtupleStationId, MuonCalibSegment *> & /*segments*/)
	{
	MsgStream log(msgSvc(), name());
	if(m_last>0 && m_last<=m_first)
		{
		log<< MSG::INFO << "Enough events read!"<< endreq;
		event=NULL;
		return;
		}
	event = &(m_reader->getEvent(m_first));
	if (event->rawHitCollection()==NULL)
		{
		log<< MSG::INFO << "End of file reached"<< endreq;
		event=NULL;
		return;
		}
	if(m_scip_double_events)
		{
		while(static_cast<int>(event->eventInfo().eventNumber()) == m_prev_event_nr)
			{
//			std::cout<<"Double ...."<< m_prev_event_nr <<std::endl;
			m_first++;
			if(m_last>0 && m_last<=m_first)
				{
				log<< MSG::INFO << "Enough events read!"<< endreq;
				event=NULL;
				return;
				}
			event = &(m_reader->getEvent(m_first));
			if (event->rawHitCollection()==NULL)
				{
				log<< MSG::INFO << "End of file reached"<< endreq;
				event=NULL;
				return;
				}
			}
		}
	m_prev_event_nr = event->eventInfo().eventNumber();	
	m_first++;

	// get the current EventInfo from StoreGateSvc
	const EventInfo * currentEvent(NULL) ;
	StatusCode sc = m_sgSvc->retrieve(currentEvent) ;
	if ( sc.isFailure() ) {
	  log << MSG::INFO << "Couldnt retrieve EventInfo from StoreGateSvc"
	      << endreq ;
	  event = NULL ;
	  return ;
	}

	// Fire the EndEvent incident; this is normally done by the 
	// AthenaEventLoopMgr after calling the "execute" method of all
	// Algorithms
	m_incSvc->fireIncident(EventIncident(*currentEvent, name(),"EndEvent"));

	// Change the EventInfo in StoreGate to the correct info from 
	// the event in the ntuple
	EventInfo * nonconst_currentEvent = 
	  const_cast<EventInfo*>(currentEvent) ;
	nonconst_currentEvent->event_ID()
	  -> set_run_number(event->eventInfo().runNumber());
	nonconst_currentEvent->event_ID()
	  -> set_event_number(event->eventInfo().eventNumber());
	nonconst_currentEvent->event_ID()
	  -> set_time_stamp(event->eventInfo().timeStamp());

	log << MSG::DEBUG << "Hacked Run.Event,Time: "
	      << "[" << currentEvent->event_ID()->run_number()
	      << "." << currentEvent->event_ID()->event_number()
	      << "," << currentEvent->event_ID()->time_stamp()
	      << "]" << endreq;

	// Fire EndRun and BeginRun Incidents if the runnumber changed
	if ( event->eventInfo().runNumber() != m_prev_run_nr ) {
	  m_prev_run_nr = event->eventInfo().runNumber() ;
	  m_incSvc->
	    fireIncident(Incident(name(),"EndRun"));
	  m_incSvc->
	    fireIncident(EventIncident(*currentEvent, name(),"BeginRun"));
	}

	// Fire BeginEvent Incident
	m_incSvc->
	  fireIncident(EventIncident(*currentEvent, name(),"BeginEvent"));


	} // end of prepareSegments

inline StatusCode  CalibNtupleLoader :: read_dictionary(istream &fl, int &tot_count)
	{
	MsgStream log(msgSvc(), name());
	std::string filename;
	fl>>filename;
	if(fl.eof())
		{
		log<<MSG::FATAL<<"Filelist has wrong format"<<endreq;
		return StatusCode::FAILURE;
		}
//read regions
	int count=0;
	std::list<std::string> regions;
	while(1)
		{
		std::string region;
		fl>>region;
		if(region=="]")
			{
			break;
			}
		if(fl.eof())
		
			{
			log<<MSG::FATAL<<"Filelist has wrong format"<<endreq;
			return StatusCode::FAILURE;
			}
		if (region=="PatternNtupleMaker")
			{
			m_chain->AddFile(filename.c_str());
			count++;
			}
		else 
			{
			regions.push_back(region);
			}
		}
	if(regions.size()>0)
		{
		if(p_reg_sel_svc == NULL)
			{
			StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
			if(!sc.isSuccess())
				{
				log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endreq;
				return sc;
				}	
			}
		count+=p_reg_sel_svc->AddRegionNtuples(filename.c_str(), m_chain, regions);
		}
	if (count>0)
		{
		log<<MSG::INFO<<"Added " << count << " regions from "<< filename<<endreq;
		}
	tot_count+=count;
	return StatusCode::SUCCESS;
	}

} //namespace MuonCalib
