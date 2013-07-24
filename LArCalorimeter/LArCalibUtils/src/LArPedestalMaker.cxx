/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArPedestalMaker.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Jan. 12, 2004  
 UPDATED:  Mar. 10, 2004 by Remi Lafaye and later

 PURPOSE:  Computes the pedestal and rms for each cell
 	   at each gain and records them in the TDS
  HISTORY:
          - Jen. 12, 2003: M. Aharrouche: creation
          - March 1st, 2004: S. Laplace: write result into DB instead of ASCII file
          - October 4th, 2007: M. Delmastro: removed NovaDB support
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArPedestalMaker.h"

//#include "GaudiKernel/AlgFactory.h"
#include "LArIdentifier/LArOnlineID.h"

#include <math.h>
#include <unistd.h>
#include <vector>


LArPedestalMaker::LArPedestalMaker(const std::string& name, ISvcLocator* pSvcLocator) 
  : Algorithm(name, pSvcLocator),
    m_storeGateSvc(0),
    m_detStore(0),
    m_groupingType("SubDetector") // SubDetector, Single, FeedThrough
{
  m_nref = 50;
  m_rms_cut = 5.0;
  declareProperty("KeyList",         m_keylist);
  declareProperty("KeyOutput",       m_keyoutput="LArPedestal");
  declareProperty("events_ref",      m_nref);
  declareProperty("nsigma",          m_rms_cut);
  declareProperty("which_sample",    m_which_sample=-1);
  declareProperty("GroupingType",    m_groupingType); 
}

LArPedestalMaker::~LArPedestalMaker()
{}

StatusCode LArPedestalMaker::initialize()
{
  StatusCode sc;
  
  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;

  m_mean.resize(1);
  m_rms.resize(1);  
  
  // StoreGate service
  sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {
      log << MSG::FATAL << " StoreGate service not found " << std::endl;
      sc = StatusCode::FAILURE; 
      return sc;
    }

  // Get DetectorStore service
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) 
    {
      log << MSG::FATAL << " DetectorStore service not found " << std::endl;
      sc = StatusCode::FAILURE; 
      return sc;
    }
  
 // m_fullFolderName="/lar/"+m_folderName+"/LArPedestal";
 if (!m_keylist.size()) // Not key list given
   {m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
    m_keylist.push_back("FREE"); //For H6....
   }
  //m_EventCounters.resize(CaloGain::LARNGAIN,0); //Initialize Event counters, one per container
 m_pedestal.setGroupingType(LArConditionsContainerBase::SingleGroup);
 sc=m_pedestal.initialize();
 if (sc.isFailure()) {
   log << MSG::ERROR << "Failed initialize intermediate Pedestal object" << endreq;
   return sc;
 }
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalMaker::execute()
//---------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  if (m_keylist.size()==0) {
    log << MSG::ERROR << "Key list is empty! No containers to process!" << endreq;
    return StatusCode::FAILURE;
  } 
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  const LArDigitContainer* larDigitContainer;

  for (;key_it!=key_it_e;key_it++) {
    sc= m_storeGateSvc->retrieve(larDigitContainer,*key_it);
    if (sc.isFailure() || !larDigitContainer) {
      log << MSG::DEBUG << "Cannot read LArCalibDigitContainer from StoreGate! key=" << *key_it << endreq;
      continue;
    }
    if(larDigitContainer->size()==0) {
      log << MSG::DEBUG << "Got empty LArDigitContainer (key=" << *key_it << ")." <<endreq;
      continue;
    }
    LArDigitContainer::const_iterator it=larDigitContainer->begin();
    LArDigitContainer::const_iterator it_end=larDigitContainer->end();
    for (;it!=it_end;it++) {  //Loop over all cells
      const HWIdentifier chid=(*it)->hardwareID();
      const CaloGain::CaloGain gain=(*it)->gain();
      //LArPedestal& thisPed=m_pedestal[gain][chid];
      LArPedestal& thisPed=m_pedestal.get(chid,gain);
      //log << MSG::DEBUG << "Cell: " << icell << " with gain " << gain << endreq;
      if (gain<0 || gain>CaloGain::LARNGAIN) {
	log << MSG::ERROR << "Found odd gain number ("<< (int)gain <<")" << endreq;
	return StatusCode::FAILURE;
      }
      const std::vector<short> & samples = (*it)->samples();
      if (thisPed.get_max()!=-1) //Have already boundaries set
	{//Check samples are in range
	  std::vector<short>::const_iterator s_it=samples.begin();
	  std::vector<short>::const_iterator s_it_e=samples.end();
	  const short& min=thisPed.get_min();
	  const short& max=thisPed.get_max();
	  for (;s_it!=s_it_e && *s_it>=min && *s_it<=max;s_it++)
            ; //Loop over samples to check....
	  if (s_it==s_it_e) //Reached end of loop?
	    thisPed.add(samples);
	}
      else {//number of entries smaller or equal to than m_nref
	thisPed.add(samples);
	if (thisPed.get_nentries()==m_nref && m_nref>0) { //Set window size
	  const double mean  = thisPed.get_mean();
	  const double noise = thisPed.get_rms();
	  const short min = (short)floor(mean - m_rms_cut*noise);
	  const short max =  (short)ceil(mean + m_rms_cut*noise);
	  thisPed.set_min(min);
	  thisPed.set_max(max);
	  thisPed.zero();
	} //end if nentries==m_nref
      } // end else
    }//End loop over all cells
  }// End loop over all containers
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalMaker::stop()
//---------------------------------------------------------------------------
{
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Stop" << endreq;

  if (m_keylist.size()==0) {
    log << MSG::ERROR << "Key list is empty! No containers processed!" << endreq;
    return StatusCode::FAILURE;
  } 
  
  // Create the LArPedestalComplete object
  LArPedestalComplete* larPedestalComplete = new LArPedestalComplete();

  sc=larPedestalComplete->setGroupingType(m_groupingType,log);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArPedestalComplete object" << endreq;
    return sc;
  }

  sc=larPedestalComplete->initialize(); 
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed initialize LArPedestalComplete object" << endreq;
    return sc;
  }

 //Outermost loop goes over all gains (different containers).
 for (int gain=0;gain<(int)CaloGain::LARNGAIN;gain++) {
   //log << MSG::INFO << "Gain " << gain << ", m_pedestal size for this gain = " <<  m_pedestal[gain].size() << endreq;

   LARPEDMAP::ConstConditionsMapIterator cell_it=m_pedestal.begin(gain);
   LARPEDMAP::ConstConditionsMapIterator cell_it_e=m_pedestal.end(gain);

   //Inner loop goes over the cells.
   int n_zero,n_min, n_max, n_cur;
   n_zero=0; n_max=n_min=-1;
   for (;cell_it!=cell_it_e;cell_it++) {
     // Check number of entries
     LArPedestal thisPed=*cell_it;
     if(thisPed.get_nentries()==0) { n_zero++; continue; }
     // Get the mean and rms
      if (m_which_sample==-1 || m_which_sample>=(int)thisPed.get_nsamples()) {
	m_mean[0] = thisPed.get_mean();
	m_rms[0]  = thisPed.get_rms();
      }
      else {
	m_mean[0] = thisPed.get_mean(m_which_sample);
	m_rms[0]  = thisPed.get_rms(m_which_sample);
      }
      n_cur = thisPed.get_nentries();
      if(n_cur<n_min || n_min<0) n_min=n_cur;
      if(n_cur>n_max || n_max<0) n_max=n_cur;

     HWIdentifier ch_id = cell_it.channelId();
	    
     // Fill the data class with pedestal and rms values
     if (ch_id!=0)
       larPedestalComplete->set(ch_id,gain,m_mean[0],m_rms[0]);
   }
 
   log << MSG::INFO << "Gain " << gain << " Number of cells with 0 events to compute pedestal: " <<  n_zero << endreq;
   log << MSG::INFO << "Gain " << gain << " Minimum number of events|samples to compute pedestal: " <<  n_min << endreq;
   log << MSG::INFO << "Gain " << gain << " Maximum number of events|samples to compute pedestal: " <<  n_max << endreq;
 }

 log << MSG::INFO << " Summary : Number of cells with a pedestal value computed : " << larPedestalComplete->totalNumberOfConditions()  << endreq;
 log << MSG::INFO << " Summary : Number of Barrel PS cells side A or C (connected+unconnected):  4096 " << endreq;
 log << MSG::INFO << " Summary : Number of Barrel    cells side A or C (connected+unconnected): 53248 " << endreq;
 log << MSG::INFO << " Summary : Number of EMEC      cells side A or C (connected+unconnected): 35328 " << endreq;
 log << MSG::INFO << " Summary : Number of HEC       cells side A or C (connected+unconnected):  3072 "<< endreq;
 log << MSG::INFO << " Summary : Number of FCAL      cells side A or C (connected+unconnected):  1792 " << endreq;
 
 // Record LArPedestalComplete
 sc = m_detStore->record(larPedestalComplete,m_keyoutput);
 if (sc != StatusCode::SUCCESS) 
   {
     log << MSG::ERROR	
	 << " Cannot store LArPedestalComplete in TDS " 
	 << endreq;
     return sc;
   }
 
 // Make symlink
 sc = m_detStore->symLink(larPedestalComplete, (ILArPedestal*)larPedestalComplete);
 if (sc != StatusCode::SUCCESS) 
   {
     log << MSG::ERROR 
	 << " Cannot make link for Data Object "
	  << endreq;
     return sc;
   }
 
 log << MSG::INFO << ">>> End of finalize" << endreq;
 
 return StatusCode::SUCCESS;
}

