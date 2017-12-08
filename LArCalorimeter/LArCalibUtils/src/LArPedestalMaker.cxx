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
  : AthAlgorithm(name, pSvcLocator),
    m_groupingType("SubDetector") // SubDetector, Single, FeedThrough
{
  m_nref = 50;
  m_rms_cut = 5.0;
  declareProperty("KeyList",         m_keylist);
  declareProperty("KeyOutput",       m_keyoutput="LArPedestal");
  declareProperty("events_ref",      m_nref);
  declareProperty("nsigma",          m_rms_cut);
  declareProperty("which_sample",    m_which_sample=-1);
  declareProperty("sample_min",      m_sample_min=-1);
  declareProperty("sample_max",      m_sample_max=-1);
  declareProperty("GroupingType",    m_groupingType); 
}

LArPedestalMaker::~LArPedestalMaker()
{}

StatusCode LArPedestalMaker::initialize()
{
  ATH_MSG_INFO ( ">>> Initialize" );

  m_mean.resize(1);
  m_rms.resize(1);  
  
 // m_fullFolderName="/lar/"+m_folderName+"/LArPedestal";
 if (!m_keylist.size()) // Not key list given
   {m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
    m_keylist.push_back("FREE"); //For H6....
   }
  //m_EventCounters.resize(CaloGain::LARNGAIN,0); //Initialize Event counters, one per container
 m_pedestal.setGroupingType(LArConditionsContainerBase::SingleGroup);
 ATH_CHECK( m_pedestal.initialize() );
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalMaker::execute()
//---------------------------------------------------------------------------
{
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR ( "Key list is empty! No containers to process!" );
    return StatusCode::FAILURE;
  } 
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  const LArDigitContainer* larDigitContainer;

  for (;key_it!=key_it_e;key_it++) {
    StatusCode sc= evtStore()->retrieve(larDigitContainer,*key_it);
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
      const HWIdentifier chid=(*it)->hardwareID();
      const CaloGain::CaloGain gain=(*it)->gain();
      //LArPedestal& thisPed=m_pedestal[gain][chid];
      LArPedestal& thisPed=m_pedestal.get(chid,gain);
      //log << MSG::DEBUG << "Cell: " << icell << " with gain " << gain << endmsg;
      if (gain<0 || gain>CaloGain::LARNGAIN) {
	ATH_MSG_ERROR ( "Found odd gain number ("<< (int)gain <<")" );
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
  ATH_MSG_INFO ( ">>> Stop" );

  if (m_keylist.size()==0) {
    ATH_MSG_ERROR ( "Key list is empty! No containers processed!" );
    return StatusCode::FAILURE;
  } 
  
  // Create the LArPedestalComplete object
  LArPedestalComplete* larPedestalComplete = new LArPedestalComplete();

  ATH_CHECK( larPedestalComplete->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larPedestalComplete->initialize() );

 //Outermost loop goes over all gains (different containers).
 for (int gain=0;gain<(int)CaloGain::LARNGAIN;gain++) {
   //log << MSG::INFO << "Gain " << gain << ", m_pedestal size for this gain = " <<  m_pedestal[gain].size() << endmsg;

   LARPEDMAP::ConditionsMapIterator cell_it=m_pedestal.begin(gain);
   LARPEDMAP::ConditionsMapIterator cell_it_e=m_pedestal.end(gain);

   //Inner loop goes over the cells.
   int n_zero,n_min, n_max, n_cur;
   n_zero=0; n_max=n_min=-1;
   for (;cell_it!=cell_it_e;cell_it++) {
     // Check number of entries
     LArPedestal thisPed=*cell_it;
     if(thisPed.get_nentries()==0) { n_zero++; continue; }
     // Get the mean and rms
      if (m_sample_min >=0 ) {
	m_mean[0] = thisPed.get_mean(m_sample_min, m_sample_max);
	m_rms[0]  = thisPed.get_rms(m_sample_max, m_sample_max);
      }
      else if (m_which_sample==-1 || m_which_sample>=(int)thisPed.get_nsamples()) {
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
 
   ATH_MSG_INFO ( "Gain " << gain << " Number of cells with 0 events to compute pedestal: " <<  n_zero );
   ATH_MSG_INFO ( "Gain " << gain << " Minimum number of events|samples to compute pedestal: " <<  n_min );
   ATH_MSG_INFO ( "Gain " << gain << " Maximum number of events|samples to compute pedestal: " <<  n_max );
 }

 ATH_MSG_INFO ( " Summary : Number of cells with a pedestal value computed : " << larPedestalComplete->totalNumberOfConditions()  );
 ATH_MSG_INFO ( " Summary : Number of Barrel PS cells side A or C (connected+unconnected):  4096 " );
 ATH_MSG_INFO ( " Summary : Number of Barrel    cells side A or C (connected+unconnected): 53248 " );
 ATH_MSG_INFO ( " Summary : Number of EMEC      cells side A or C (connected+unconnected): 35328 " );
 ATH_MSG_INFO ( " Summary : Number of HEC       cells side A or C (connected+unconnected):  3072 ");
 ATH_MSG_INFO ( " Summary : Number of FCAL      cells side A or C (connected+unconnected):  1792 " );
 
 // Record LArPedestalComplete
 ATH_CHECK( detStore()->record(larPedestalComplete,m_keyoutput) );
 ATH_CHECK( detStore()->symLink(larPedestalComplete, (ILArPedestal*)larPedestalComplete) );
 
 ATH_MSG_INFO ( ">>> End of finalize" );
 return StatusCode::SUCCESS;
}

