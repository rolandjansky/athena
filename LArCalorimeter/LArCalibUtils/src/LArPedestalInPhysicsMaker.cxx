/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArPedestalInPhysicsMaker.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  Remi Lafaye
 CREATED:  Feb. 16, 2005

 PURPOSE:  Computes the pedestal and rms for each cell
 	   at each gain and records them in the TDS
  HISTORY:
          - Jen. 12, 2003: M. Aharrouche: creation
          - March 1st, 2004: S. Laplace: write result into DB instead of ASCII file
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArPedestalInPhysicsMaker.h"

//#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/Property.h"
//#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "StoreGate/DataHandle.h"
#include "LArIdentifier/LArOnlineID.h"
#include "xAODEventInfo/EventInfo.h"

//#include "LArRawEvent/LArFEB_Digit.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBScintillatorCont.h"

#include <math.h>
#include <stdarg.h>
#include <unistd.h>
#include <fstream>
#include <vector>

LArPedestalInPhysicsMaker::LArPedestalInPhysicsMaker(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_groupingType("SubDetector") // SubDetector, Single, FeedThrough
{
  m_count = 0;
  m_nref = 300;
  m_pedestal.resize(CaloGain::LARNGAIN);
  declareProperty("KeyList",m_keylist);
  declareProperty("KeyOutput",m_keyoutput="LArPedestal");
  declareProperty("events_ref", m_nref);
  declareProperty("which_sample", m_which_sample=-1);
  declareProperty("accept", m_accept=14);
  declareProperty("reject", m_reject=1);
  declareProperty("S1cut", m_S1cut=0);
  declareProperty("RunNumber", m_run=0);
  declareProperty("Record", m_record=0);
  m_event=0; m_pevent=0;
  m_scint_names.push_back("S0");
  m_scint_names.push_back("S1");
  declareProperty("GroupingType",   m_groupingType);
}


// Destructor
//---------------------------------------------------------------------------
LArPedestalInPhysicsMaker::~LArPedestalInPhysicsMaker()
//---------------------------------------------------------------------------
{  
}

//---------------------------------------------------------------------------
StatusCode LArPedestalInPhysicsMaker::initialize()
//---------------------------------------------------------------------------
{
  ATH_MSG_INFO ( ">>> Initialize" );

  m_mean.resize(1);
  m_rms.resize(1);  
  
  // create persistent structure vector
  //vPedestal = new LArPedestalP1();

 IIncidentSvc* incSvc = nullptr;
 ATH_CHECK( service("IncidentSvc", incSvc) );

 // m_fullFolderName="/lar/"+m_folderName+"/LArPedestal";
 if (!m_keylist.size()) // No key list given
   {
     //m_keylist.push_back("HIGH");
     //m_keylist.push_back("MEDIUM");
     //m_keylist.push_back("LOW");
     m_keylist.push_back("FREE");
   }
  //m_EventCounters.resize(CaloGain::LARNGAIN,0); //Initialize Event counters, one per container
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LArPedestalInPhysicsMaker::execute()
//---------------------------------------------------------------------------
{
  int write=0;
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR ( "Key list is empty! No containers to process!" );
    return StatusCode::FAILURE;
  } 

  // Retrieve EventInfo
  const DataHandle<xAOD::EventInfo> thisEventInfo;
  StatusCode sc=evtStore()->retrieve(thisEventInfo);
  int eventnumber=0;
  if (sc!=StatusCode::SUCCESS)
    ATH_MSG_WARNING ( "No EventInfo object found!" );
  else {
    if(m_run==0) m_run=thisEventInfo->runNumber();
    eventnumber=thisEventInfo->eventNumber();
  }

//  // Retrieve the TBScintillators
// const TBScintillatorCont * theTBScint;
//  sc = evtStore()->retrieve(theTBScint,"ScintillatorCont");
//  if (sc.isFailure()) 
//    {
//      log << MSG::ERROR << " Cannot read TBScintillatorCont from StoreGate! " << endmsg;
//      //return StatusCode::FAILURE;
//    }
//  const unsigned nScint=m_scint_names.size();
//  unsigned iScint=0;
//  float ScintADC[2]; float ScintTDC[2];
//  ScintADC[1]=0;
//  TBScintillatorCont::const_iterator it_scint = theTBScint->begin();
//  TBScintillatorCont::const_iterator last_scint = theTBScint->end();
//  for(;it_scint!=last_scint;it_scint++) {
//    const TBScintillator * scint = (*it_scint);
//    const std::string name = scint->getDetectorName();
//    for (iScint=0;iScint<nScint;iScint++)
//      if (name==m_scint_names[iScint]) {
//	break;
//      }
//    if (iScint==nScint) 
//      continue;
//    ScintADC[iScint] = scint->getSignal();
//    ScintTDC[iScint] = scint->getTimeSignal();
//  } //end loop over scintillator-container
//  // Cut on S1 signal
//  if(m_S1cut>0) if(ScintADC[1]>m_S1cut) return StatusCode::SUCCESS;
//
//  //Retrieve the TBPhase
//  const TBPhase* theTBPhase;
//  sc = evtStore()->retrieve(theTBPhase, "TBPhase");
//  float time = -1;
//  int  itime = -1;
//
//  if (sc.isFailure()) {
//    log << MSG::ERROR
//	<< "cannot allocate TBPhase "
//	<< endmsg;
//    //return StatusCode::FAILURE;
//  } else {
//    time = theTBPhase->getPhase();
//    itime = theTBPhase->getPhaseind();
//    log << MSG::INFO << "TBPhase retrieved from storegate - time "<< time <<" itime "<< itime << endmsg;
//  }

  //Retrieve the TBTriggerPatternUnit
  const TBTriggerPatternUnit* theTBTriggerPatternUnit;
  sc = evtStore()->retrieve(theTBTriggerPatternUnit, "TBTrigPat");
  unsigned int trigger=1;

  if (sc.isFailure()) {
    ATH_MSG_ERROR( "cannot allocate TBTriggerPatternUnit" );
    //return StatusCode::FAILURE;
  } else {
    trigger = theTBTriggerPatternUnit->getTriggerWord();
    ATH_MSG_INFO ( "TBTriggerPatternUnit retrieved from storegate - " << trigger );
  }

  // Check whether we are in Physics
  if(m_reject>0) if(trigger&m_reject) return StatusCode::SUCCESS;
  // Check whether we have pedestals
  if(m_accept>0) if((trigger&m_accept)==0) return StatusCode::SUCCESS;
  //printf("Trigger=%d [%g %g %g %g]\n",trigger,ScintADC[0],ScintTDC[0],ScintADC[1],ScintTDC[1]);

  m_count++;
  if(m_event==-1) {
    m_event=(eventnumber+m_pevent)/2;
  }
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  const LArDigitContainer* larDigitContainer;
  //std::cout << "Reading LArCalibDigitContainer from StoreGate! key=" << *key_it << endl;
  for (;key_it!=key_it_e;key_it++) {
    sc= evtStore()->retrieve(larDigitContainer,*key_it);
    if (sc.isFailure() || !larDigitContainer) {
      ATH_MSG_DEBUG ( "Cannot read LArCalibDigitContainer from StoreGate! key=" << *key_it );
      continue;
    }
    if(larDigitContainer->size()==0) {
      ATH_MSG_DEBUG ( "Got empty LArDigitContainer (key=" << *key_it << ")." );
      continue;
    }
    LArDigitContainer::const_iterator it=larDigitContainer->begin();
    LArDigitContainer::const_iterator it_end=larDigitContainer->end();
    for (;it!=it_end;it++) {  //Loop over all cells
      HWIdentifier chid=(*it)->hardwareID();
      CaloGain::CaloGain gain=(*it)->gain();
      //log << MSG::DEBUG << "Cell: " << icell << " with gain " << gain << endmsg;
      if (gain<0 || gain>CaloGain::LARNGAIN) {
	ATH_MSG_ERROR ( "Found odd gain number ("<< (int)gain <<")" );
	return StatusCode::FAILURE;
      }
      const std::vector<short> & samples = (*it)->samples();
      if(samples[0]!=0) m_pedestal[gain][chid].add(samples);
      if (m_pedestal[gain][chid].get_nentries()==m_nref && m_nref>0) {
	write=1;
      }
    }//End loop over all cells
  }// End loop over all containers

  if(write) {
    sc=fillDB();
    if(sc.isFailure()) return StatusCode::FAILURE;
    std::vector<std::string>::const_iterator key_it=m_keylist.begin();
    std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
    const LArDigitContainer* larDigitContainer;
    for (;key_it!=key_it_e;key_it++) {
      sc= evtStore()->retrieve(larDigitContainer,*key_it);
      if (sc.isFailure() || !larDigitContainer) {
	ATH_MSG_DEBUG ( "Cannot read LArCalibDigitContainer from StoreGate! key=" << *key_it );
	continue;
      }
      if(larDigitContainer->size()==0) {
	ATH_MSG_DEBUG( "Got empty LArDigitContainer (key=" << *key_it << ")." );
	continue;
      }
      LArDigitContainer::const_iterator it=larDigitContainer->begin();
      LArDigitContainer::const_iterator it_end=larDigitContainer->end();
      for (;it!=it_end;it++) {  //Loop over all cells
	HWIdentifier chid=(*it)->hardwareID();
	CaloGain::CaloGain gain=(*it)->gain();
	if (gain<0 || gain>CaloGain::LARNGAIN) {
	  ATH_MSG_ERROR ( "Found odd gain number ("<< (int)gain <<")" );
	  return StatusCode::FAILURE;
	}
	m_pedestal[gain][chid].zero();
      }//End loop over all cells
    }// End loop over all containers
    m_pevent=eventnumber;
    m_event=-1;
    write=0;
  } //end if nentries==m_nref
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalInPhysicsMaker::stop()
//---------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Stop" << endmsg;

  if (m_keylist.size()==0) {
    ATH_MSG_ERROR ( "Key list is empty! No containers processed!" );
    return StatusCode::FAILURE;
  } 

  ATH_MSG_INFO ( "########################################################" );
  ATH_MSG_INFO ( "Number of random events found: " << m_count );
  ATH_MSG_INFO ( "########################################################" );

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalInPhysicsMaker::fillDB()
//---------------------------------------------------------------------------
{
  ATH_MSG_INFO ( ">>> Fill DB" );

  // Create the LArPedestalComplete object
  LArPedestalComplete* larPedestalComplete = new LArPedestalComplete();

  ATH_CHECK( larPedestalComplete->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larPedestalComplete->initialize() );

  ATH_MSG_INFO ( "########################################################" );
  ATH_MSG_INFO ( "Number of randoms:              " << m_count );
  ATH_MSG_INFO ( "Creating new IOV at run event:  " << m_run << " " << m_event );
  ATH_MSG_INFO ( "########################################################" );

  //Outermost loop goes over all gains (different containers).
  for (int gain=0;gain<(int)CaloGain::LARNGAIN;gain++) {
  
    ATH_MSG_INFO ( "Gain " << gain << ", m_pedestal size for this gain = " 
                   <<  m_pedestal[gain].size() );
   if(m_pedestal[gain].size()<=1) continue;    //No data for this gain
   LARPEDMAP::const_iterator cell_it=m_pedestal[gain].begin();
   LARPEDMAP::const_iterator cell_it_e=m_pedestal[gain].end();
   
   //Inner loop goes over the cells.
   for (;cell_it!=cell_it_e;cell_it++) {
     // Check number of entries
     if(cell_it->second.get_nentries()==0) continue;
     // Get the mean and rms
     if (m_which_sample==-1 || m_which_sample>=(int)cell_it->second.get_nsamples()) {
       m_mean[0] = cell_it->second.get_mean();
       m_rms[0]  = cell_it->second.get_rms();
     }
     else {
       m_mean[0] = cell_it->second.get_mean(m_which_sample);
       m_rms[0]  = cell_it->second.get_rms(m_which_sample);
     }

     HWIdentifier ch_id = cell_it->first;
     // Fill the data class with pedestal and rms values
     if (ch_id!=0)
       larPedestalComplete->set(ch_id,gain,m_mean[0],m_rms[0]);

   }
 }

 if(m_record) {
   ATH_CHECK( detStore()->record(larPedestalComplete, m_keyoutput) );
   ATH_CHECK( detStore()->symLink(larPedestalComplete, (ILArPedestal*)larPedestalComplete) );
   m_record=0;
 }

 return StatusCode::SUCCESS;
}


