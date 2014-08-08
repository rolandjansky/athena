/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TBScintillatorMonTool.cxx
// PACKAGE:  TBMonitoring
//
// AUTHOR:   Pierre-Antoine Delsart
//
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBScintillatorMonTool.h"


#include "TBEvent/TBScintillatorCont.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <vector>


/*---------------------------------------------------------*/
TBScintillatorMonTool::TBScintillatorMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat"); //Already definde in base class
  declareProperty("histoPath",m_histoPath="/BeamDetectors/Scintillator/");

  declareProperty("FakeDetectors",m_fake_detector=false);

  declareProperty("SGScintkey",m_SGkeyscint = "ScintillatorCont");

  declareProperty("ScintNames",m_scint_names);

  declareProperty("BookAtFirstEvent",m_bookatfirstevent = true);

  declareProperty("ADCBinNum",m_adcbin=100);
  declareProperty("ADCMax",m_adcmax=1024);
  declareProperty("ADCMin",m_adcmin=0);

 
  declareProperty("TDCBinNum",m_tdcbin=100);
  declareProperty("TDCMax",m_tdcmax=1024);
  declareProperty("TDCMin",m_tdcmin=0);
  m_histo_scint=NULL;
  m_histo_scint_time=NULL;
 
}

/*---------------------------------------------------------*/
TBScintillatorMonTool::~TBScintillatorMonTool()
/*---------------------------------------------------------*/
{ 
  if (m_histo_scint)
    delete[] m_histo_scint;
  if (m_histo_scint_time)
    delete[] m_histo_scint_time;

}

/*---------------------------------------------------------*/
StatusCode TBScintillatorMonTool:: initialize()
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc;

  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
	<< ": Unable to locate Service StoreGateSvc" 
	<< endreq;
    return sc;
  }



  //set to true whitin bookHist() 
  m_isBooked = false;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBScintillatorMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in bookHists()" << endreq;
  log << MSG::DEBUG << "Base path:" << m_path << endreq;
  log << MSG::DEBUG << "Histo path:" << m_histoPath << endreq;
  log << MSG::DEBUG << "Full path: " <<   m_path+m_histoPath << endreq;
  if(!m_bookatfirstevent)
    { 
      //m_path += "/BeamDetectors/Scintillator/";
      const std::string path=m_path+m_histoPath;
      log << MSG::INFO << " Booking histos now" << endreq;
      m_scint_map.clear();
      m_scintnum = m_scint_names.size();
      m_histo_scint = new IHistogram1D*[m_scintnum];
      m_histo_scint_time = new IHistogram1D*[m_scintnum];
      for(unsigned int nameind=0;nameind<m_scint_names.size();nameind++){ // (Nfound = m_scint_map.size())
	m_scint_map.push_back(nameind);
	std::string hname = path+m_scint_names[nameind];
	std::string htitle =  "Scintillator  "+m_scint_names[nameind]+ " Signal";
	log << MSG::DEBUG << "            histo scint "<< hname << endreq;
	m_histo_scint[nameind] = ToolHistoSvc()->book(hname,htitle,m_adcbin,m_adcmin,m_adcmax);
	
	hname =path+"Time"+m_scint_names[nameind];
	htitle =  "Scintillator  "+m_scint_names[nameind]+ " Time signal";
	m_histo_scint_time[nameind] = ToolHistoSvc()->book(hname,htitle,m_tdcbin,m_tdcmin,m_tdcmax);
	
      }
      log << MSG::DEBUG << "histo path: " << path  << endreq;
      
      log << MSG::INFO << " \t Monitoring Scintillators  " ;
      log << " \t : YES "<< endreq;
      
      
      
      log << MSG::INFO << " \t Fake Detectors  \t  " ;
      if(m_fake_detector) log << " \t : YES "<< endreq;
      else log << " \t : NO "<< endreq;
      SetBookStatus(true);
      return StatusCode::SUCCESS;      
    }
  else return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TBScintillatorMonTool::mybookHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in mybookHists()" << endreq;


  StatusCode sc;
  
   //Get Run number
  std::stringstream rn_stream;
  EventID *thisEvent;           //EventID is a part of EventInfo
  const EventInfo* thisEventInfo;
  sc=m_StoreGate->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS)
    log << MSG::WARNING << "No EventInfo object found! Can't read run number!" << endreq;
  else
    {thisEvent=thisEventInfo->event_ID();
     rn_stream << "Run " << thisEvent->run_number() << " ";
    }

  const std::string runnumber=rn_stream.str();
  
  // Scintillator histos ----------------------------

  TBScintillatorCont *scintCont;
  sc = m_StoreGate->retrieve(scintCont, m_SGkeyscint);
  if (sc.isFailure()){
    log << MSG::INFO << "BeamDetectorMonitoring: Retrieval of Scintillators failed" << endreq;
  } else {
    m_scintnum = scintCont->size() > m_scint_names.size() ? m_scint_names.size():scintCont->size();
    // if container empty don't book histo yet.
    if(m_scintnum==0) {
      log << MSG::INFO << " Scintillator Cont is empty"<< endreq;
      return StatusCode::SUCCESS;
    }
    if(scintCont->size() != m_scint_names.size()){
      log << MSG::INFO << "!! Warning !! Number of SCINTRaw in SG="
	  <<scintCont->size()<<" differs from number in jobOptions=" << m_scint_names.size()
	  << endreq;}

    // Map histo number to a scint name
    m_scint_map.clear();
    int Nfound=0;
    for(unsigned j=0;j<m_scint_names.size();j++){
      TBScintillatorCont::const_iterator it_bc   = scintCont->begin();
      TBScintillatorCont::const_iterator last_bc   = scintCont->end();
      while(it_bc!=last_bc) {
	if((*it_bc)->getDetectorName()==m_scint_names[j]) break;
	it_bc++;
      }
      if(it_bc!=last_bc){
	m_scint_map.push_back(j);
	log << MSG::DEBUG << "histo "<<Nfound<< " --> SCINT "<< m_scint_names[m_scint_map[Nfound]] << endreq;
	Nfound++;
      }
      else {log << MSG::DEBUG << "SCINT "<< m_scint_names[j]<< " Not Found"  << endreq;}
    }
       
    // Now there are Nfound matching between SCINT in Cont and the joboption list
    //       m_scint_map.resize(Nfound);
    m_scintnum=Nfound;

    m_histo_scint = new IHistogram1D*[m_scintnum];
    m_histo_scint_time = new IHistogram1D*[m_scintnum];
 
//     TBScintillatorCont::const_iterator it_scint   = scintCont->begin();
//     TBScintillatorCont::const_iterator last_scint   = scintCont->end();

    //m_path += "/BeamDetectors/Scintillator/";
    const std::string path=m_path+m_histoPath;
    for(int nameind=0;nameind<Nfound;nameind++){ // (Nfound = m_scint_map.size())
      std::string hname = path+m_scint_names[m_scint_map[nameind]];
      std::string htitle = runnumber + "Scintillator  "+m_scint_names[m_scint_map[nameind]]+ " Signal";
      log << MSG::DEBUG << "            histo scint "<< hname << endreq;
      m_histo_scint[nameind] = ToolHistoSvc()->book(hname,htitle,m_adcbin,m_adcmin,m_adcmax);
	
      hname =path+"Time"+m_scint_names[m_scint_map[nameind]];
      htitle = runnumber + "Scintillator  "+m_scint_names[m_scint_map[nameind]]+ " Time signal";
      m_histo_scint_time[nameind] = ToolHistoSvc()->book(hname,htitle,m_tdcbin,m_tdcmin,m_tdcmax);
    }
    log << MSG::DEBUG << "histo path: " << path  << endreq;
  }

  log << MSG::INFO << " \t Monitoring Scintillators  " ;
  log << " \t : YES "<< endreq;
  


  log << MSG::INFO << " \t Fake Detectors  \t  " ;
  if(m_fake_detector) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;
  
  SetBookStatus(true);

  return StatusCode::SUCCESS;
}

// /*---------------------------------------------------------*/
// bool TBScintillatorMonTool::histsNotBooked()
// /*---------------------------------------------------------*/
// {

//   if(m_isBooked) return false;
//   else return true;
// }

/*---------------------------------------------------------*/
StatusCode TBScintillatorMonTool::fillHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());


  log << MSG::DEBUG << "in fillHists()" << endreq;

 
  // Fill some bpc and stuff (testing) 
  if(m_fake_detector) FillRandomDetect();

  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  if(m_scintnum==0) {
    log << MSG::INFO << " Nothing to monitor"<< endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;
 
  // Scint monitor ----------------------------------------------------------------

  TBScintillatorCont * scintCont;
  sc = m_StoreGate->retrieve(scintCont, m_SGkeyscint);
  if (sc.isFailure()){
    log << MSG::DEBUG 
	<< "BeamDetectorMonitoring: Retrieval of Scintillators failed" 
	<< endreq;
  }else {
    TBScintillatorCont::const_iterator it_scint   = scintCont->begin();
    TBScintillatorCont::const_iterator last_scint   = scintCont->end();
      
    // loop over list of scint to monitor :
    for(int nameind=0; nameind<m_scintnum;nameind++){

      // if we got out of sync, find the scint named m_scint_names[m_scint_map[nameind]]
      if((it_scint!=last_scint)&&((*it_scint)->getDetectorName() != m_scint_names[m_scint_map[nameind]])){
	it_scint=scintCont->begin();
	while(it_scint!=last_scint){
	  log << MSG::DEBUG << (*it_scint)->getDetectorName() << " ";
	  if ((*it_scint)->getDetectorName() != m_scint_names[m_scint_map[nameind]]) it_scint++;
	  else break;
	}
	log << MSG::DEBUG << endreq;
      }
      if(it_scint==last_scint) {
	// did not find the scint
	log << MSG::INFO 
	    << "BeamDetectorMonitoring: could not find scintillator "
	    << "named "
	    << m_scint_names[m_scint_map[nameind]] 
	    << endreq;
	it_scint= scintCont->begin();
	continue;
      }

      log << MSG::DEBUG << "Scint " <<(*it_scint)->getDetectorName() << " Sig= "<< (*it_scint)->getSignal() <<" Time Sig= " <<(*it_scint)->getTimeSignal()<< endreq;	
      // now it_scint contains the right scint
      const TBScintillator * scint = (*it_scint);


      if(!scint->isSignalOverflow()) m_histo_scint[nameind]->fill(scint->getSignal(),1.0);
      if(!scint->isTimeOverflow()) m_histo_scint_time[nameind]->fill(scint->getTimeSignal(),1.0);
      it_scint++;

    }
  }

  log << MSG::DEBUG << "fillHists() ended" << endreq;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBScintillatorMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
  // Fake different beam detectors/data classes

  MsgStream log(msgSvc(), name());

  

}
