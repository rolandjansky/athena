/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     LArPedestalAutoCorrBuilder.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils

 AUTHORS:  W. Lampl
 CREATED:  Aug 17th, 2009, merging LArPedestalBuilder & LArAutoCorrBuilder

 PURPOSE:  Create LArPedestalComplete and LArAutoCorrComplete objects
           out of pre-accmulated LArAccumulatedDigits

********************************************************************/

// Include files
#include "LArCalibUtils/LArPedestalAutoCorrBuilder.h"

#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArRawConditions/LArAutoCorrComplete.h"

#include "LArIdentifier/LArOnlineID.h"


LArPedestalAutoCorrBuilder::LArPedestalAutoCorrBuilder(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_onlineHelper(0),
    m_event_counter(0),
    m_fatalFebErrorPattern(0xffff)
{
  declareProperty("KeyList",         m_keylist);
  declareProperty("PedestalKey",     m_pedContName="LArPedestal");
  declareProperty("normalize",       m_normalize=1); 
  declareProperty("AutoCorrKey",     m_acContName="LArAutoCorr");
  declareProperty("GroupingType",    m_groupingType="ExtendedFeedThrough"); 
  declareProperty("doPedestal",      m_doPedestal=true);
  declareProperty("doAutoCorr",      m_doAutoCorr=true);
  declareProperty("sample_min",      m_sample_min=-1);
  declareProperty("sample_max",      m_sample_max=-1);
}

LArPedestalAutoCorrBuilder::~LArPedestalAutoCorrBuilder()
{}

StatusCode LArPedestalAutoCorrBuilder::initialize()
{
  StatusCode sc;
 
  sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }


  if (!m_doPedestal && !m_doAutoCorr) {
    msg(MSG::ERROR) << "Configuration Problem: Neither doPedstal nor doAutoCorr set!" << endmsg;
    return StatusCode::FAILURE;
  }

 // m_fullFolderName="/lar/"+m_folderName+"/LArPedestal";
 if (!m_keylist.size()) // Not key list given
   {m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
    m_keylist.push_back("FREE"); //For H6....
   }

 //Container for internal accumulation
 m_accu.setGroupingType(LArConditionsContainerBase::SingleGroup);
 sc=m_accu.initialize(); 
 if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize LArConditionsContainer 'm_accu'" << endmsg;
    return sc;
  }
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalAutoCorrBuilder::execute()
//---------------------------------------------------------------------------
{

  StatusCode sc;
  ++m_event_counter;
  if (m_keylist.size()==0) {
    msg(MSG::ERROR) << "Key list is empty! No containers processed!" << endmsg;
    return StatusCode::FAILURE;
  } 
  
  
  const LArFebErrorSummary* febErrSum=NULL;
  if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
    sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve FebErrorSummary object!" << endmsg;
      return sc;
    }
  }
  else
    if (m_event_counter==1)
      msg(MSG::WARNING) << "No FebErrorSummaryObject found! Feb errors not checked!" << endmsg;


  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  const LArAccumulatedDigitContainer* container;
  
  //Outermost loop goes over all gains (different containers).
  for (;key_it!=key_it_e;key_it++) {
    
    sc= evtStore()->retrieve(container,*key_it);
    if (sc.isFailure() || !container) {
      ATH_MSG_DEBUG("Cannot read LArAccumulatedDigitContainer from StoreGate! key=" << *key_it);
      return StatusCode::SUCCESS; 
    }
    
    // check that container is not empty
    if(container->size()==0 ) {
      ATH_MSG_DEBUG("LArAccumulatedDigitContainer (key=" << *key_it << ") is empty ");
      continue;
    }
    
    HWIdentifier  lastFailedFEB(0);
    //Inner loop goes over the cells.
    LArAccumulatedDigitContainer::const_iterator it=container->begin();
    LArAccumulatedDigitContainer::const_iterator it_end=container->end();
    for (;it!=it_end;it++) {  //Loop over all cells
      const LArAccumulatedDigit* dg = *it;
      if (dg->nTrigger()==0) continue; //Don't care about empty digits
      const HWIdentifier chid=dg->hardwareID();
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (febErrSum) {
	const uint16_t febErrs=febErrSum->feb_error(febid);
	if (febErrs & m_fatalFebErrorPattern) {
	  if (febid!=lastFailedFEB) {
	    lastFailedFEB=febid;
	    msg(MSG::ERROR) << "Event " << m_event_counter << " Feb " <<  m_onlineHelper->channel_name(febid) 
		<< " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored." << endmsg;
	  }
	  continue;
	} //end if fatal feb error
      }//end if check feb error summary

      const CaloGain::CaloGain gain=dg->gain();

      LArAccumulatedDigit& accDg=m_accu.get(chid,gain);
      if (!accDg.setAddSubStep(*dg)) 
	msg(MSG::ERROR) << "Failed to accumulate sub-steps! Inconsistent number of ADC samples" << endmsg;
    } //end loop over input container
  }//end loop over keys
  return StatusCode::SUCCESS;
}



StatusCode LArPedestalAutoCorrBuilder::stop() {

  LArAutoCorrComplete* larAutoCorrComplete = NULL;
  LArPedestalComplete* larPedestalComplete = NULL;

  if (m_doAutoCorr) { 
    //Book and initialize LArAutoCorrComplete object
    larAutoCorrComplete = new LArAutoCorrComplete();
    StatusCode sc=larAutoCorrComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to set groupingType for LArAutoCorrComplete object" << endmsg;
      return sc;
    }
    sc=larAutoCorrComplete->initialize(); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed initialize LArAutoCorrComplete object" << endmsg;
      return sc;
    }
  }

  if (m_doPedestal) {
    //Book and initialize LArPedestalComplete object
    larPedestalComplete = new LArPedestalComplete();
    StatusCode sc=larPedestalComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to set groupingType for LArPedestalComplete object" << endmsg;
      return sc;
    }
    sc=larPedestalComplete->initialize(); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed initialize LArPedestalComplete object" << endmsg;
      return sc;
    }
  }
    
  //For the log output further down
  std::string objName;
  if (m_doPedestal)
    objName="pedestal";
  if (m_doAutoCorr)
    objName="autocorr";
  if (m_doAutoCorr && m_doPedestal)
    objName="pedestal & autocorr";

  int n_zero,n_min, n_max, n_cur;
  n_zero=0; n_max=n_min=-1;
  unsigned NCells=0;
  std::vector<float> cov;
  //Loop over gains
  for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)k;
    //Loop over cells
    ACCU::ConditionsMapIterator cell_it=m_accu.begin(gain);
    ACCU::ConditionsMapIterator cell_it_e=m_accu.end(gain);
    if (cell_it==cell_it_e) continue; //No data for this gain
    for (;cell_it!=cell_it_e;cell_it++) {
      const LArAccumulatedDigit& dg=*cell_it;
      n_cur = dg.nTrigger();
      if(n_cur==0) { n_zero++; continue; }

      HWIdentifier chid = cell_it.channelId();     
      
      if(n_cur<n_min || n_min<0) n_min=n_cur;
      if(n_cur>n_max || n_max<0) n_max=n_cur;
      
      // Fill the data class with pedestal and rms values
      if (larPedestalComplete)
	larPedestalComplete->set(chid,gain,dg.mean(m_sample_min, m_sample_max),dg.RMS(m_sample_min, m_sample_max));
      
      if (larAutoCorrComplete) {
	dg.getCov(cov,m_normalize);
	larAutoCorrComplete->set(chid,gain,cov);
      }
      NCells++;
    }//end loop over all cells	

    msg(MSG::INFO) << "Gain " << gain << " Number of cells with 0 events to compute "<<objName<< ": " << n_zero << endmsg;
    msg(MSG::INFO) << "Gain " << gain << " Minimum number of events*samples to compute " <<objName<<": "<< n_min << endmsg;
    msg(MSG::INFO) << "Gain " << gain << " Maximum number of events*samples to compute " <<objName<<": " <<n_max << endmsg;
  }// End loop over all containers
  
  msg(MSG::INFO) << " Summary : Number of cells with " <<objName<<" value computed : " << NCells  << endmsg;
  msg(MSG::INFO) << " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 " << endmsg;
  msg(MSG::INFO) << " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 " << endmsg;
  msg(MSG::INFO) << " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 " << endmsg;
  msg(MSG::INFO) << " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 " << endmsg;
  msg(MSG::INFO) << " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 " << endmsg;
    
  if (larPedestalComplete) {
    // Record LArPedestalComplete
    StatusCode sc = detStore()->record(larPedestalComplete,m_pedContName);
    if (sc != StatusCode::SUCCESS) {
      msg(MSG::ERROR)	 << " Cannot store LArPedestalComplete in TDS " << endmsg;
      delete larPedestalComplete;
      delete larAutoCorrComplete;
      return sc;
    }
    else
      msg(MSG::INFO) << "Recorded LArPedestalComplete object with key " << m_pedContName << endmsg;
    
    // Make symlink
    sc = detStore()->symLink(larPedestalComplete, (ILArPedestal*)larPedestalComplete);
    if (sc != StatusCode::SUCCESS) {
      msg(MSG::ERROR)  << " Cannot make link for Data Object " << endmsg;
      return sc;
    }
  } // end if LArPedestal


  if (larAutoCorrComplete) {
    StatusCode sc = detStore()->record(larAutoCorrComplete,m_acContName);
    if (sc != StatusCode::SUCCESS) { 
      msg(MSG::ERROR)  << " Cannot store LArAutoCorrComplete in TDS "<< endmsg;
      delete larAutoCorrComplete;
      return sc;
    }
    else
      msg(MSG::INFO) << "Recorded LArAutCorrComplete object with key " << m_acContName << endmsg;

    // Make symlink
    sc = detStore()->symLink(larAutoCorrComplete, (ILArAutoCorr*)larAutoCorrComplete);
    if (sc != StatusCode::SUCCESS)  {
      msg(MSG::ERROR)  << " Cannot make link for Data Object " << endmsg;
      return sc;
    }
  } // end if have AutoCorr

  return StatusCode::SUCCESS;
}

