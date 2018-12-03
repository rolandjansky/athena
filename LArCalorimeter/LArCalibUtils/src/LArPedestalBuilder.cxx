/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArPedestalBuilder.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  R. Lafaye
 CREATED:  Jun. 12, 2008 from LArPedestalMaker
  
 PURPOSE:  Get the pedestal and rms for each cell from LArAccumulatedDigits
           at each gain and records them in TDS

 HISTORY:
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArPedestalBuilder.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArIdentifier/LArOnlineID.h"

//#include <math.h>
//#include <unistd.h>
#include <vector>

#include "LArRawEvent/LArFebErrorSummary.h"

LArPedestalBuilder::LArPedestalBuilder(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_onlineHelper(0),
    m_event_counter(0),
    m_fatalFebErrorPattern(0xffff)
{
  declareProperty("KeyList",         m_keylist);
  declareProperty("PedestalKey",     m_pedContName="LArPedestal");
  declareProperty("GroupingType",    m_groupingType="ExtendedFeedThrough"); 
}

LArPedestalBuilder::~LArPedestalBuilder()
{}

StatusCode LArPedestalBuilder::initialize()
{
  StatusCode sc;
  ATH_MSG_INFO( ">>> Initialize" );

  //m_mean.resize(1);
  //m_rms.resize(1);  
  
  sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
  


 // m_fullFolderName="/lar/"+m_folderName+"/LArPedestal";
 if (!m_keylist.size()) // Not key list given
   {m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
    m_keylist.push_back("FREE"); //For H6....
   }

 //m_pedestal.setGroupingType(LArConditionsContainerBase::SingleGroup);

 m_accu.setGroupingType(LArConditionsContainerBase::SingleGroup);
 m_accu.initialize(); 
 if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed initialize LArConditionsContainer 'm_accu'" );
    return sc;
  }
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArPedestalBuilder::execute()
//---------------------------------------------------------------------------
{

  StatusCode sc;
  ++m_event_counter;
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR( "Key list is empty! No containers processed!" );
    return StatusCode::FAILURE;
  } 
  
  
  const LArFebErrorSummary* febErrSum=NULL;
  if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
    sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve FebErrorSummary object!" );
      return sc;
    }
  }
  else
    if (m_event_counter==1)
      ATH_MSG_WARNING( "No FebErrorSummaryObject found! Feb errors not checked!" );
 




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
	    ATH_MSG_ERROR( "Event " << m_event_counter << " Feb " <<  m_onlineHelper->channel_name(febid) 
		<< " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored." );
	  }
	  continue;
	} //end if fatal feb error
      }//end if check feb error summary

      const CaloGain::CaloGain gain=dg->gain();

      LArAccumulatedDigit& accDg=m_accu.get(chid,gain);
      if (!accDg.setAddSubStep(*dg)) 
	ATH_MSG_ERROR( "Failed to accumulate sub-steps! Inconsistent number of ADC samples" );
    } //end loop over input container
  }//end loop over keys
  return StatusCode::SUCCESS;
}



 StatusCode LArPedestalBuilder::stop() {
  // Create the LArPedestalComplete object
  LArPedestalComplete* larPedestalComplete = new LArPedestalComplete();
  // Initialize LArPedestalComplete 
  StatusCode sc=larPedestalComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArPedestalComplete object" );
    return sc;
  }
  sc=larPedestalComplete->initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed initialize LArPedestalComplete object" );
    return sc;
  }


  int n_zero,n_min, n_max, n_cur;
  n_zero=0; n_max=n_min=-1;
  unsigned NPedestal=0;
  //Loop over gains
  for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)k;
    //Loop over cells
    ACCU::ConstConditionsMapIterator cell_it=m_accu.begin(gain);
    ACCU::ConstConditionsMapIterator cell_it_e=m_accu.end(gain);
    if (cell_it==cell_it_e) continue; //No data for this gain
    for (;cell_it!=cell_it_e;cell_it++) {
      const LArAccumulatedDigit& dg=*cell_it;
      n_cur = dg.nTrigger();
      if(n_cur==0) { n_zero++; continue; }

      HWIdentifier chid = cell_it.channelId();
     
      
      if(n_cur<n_min || n_min<0) n_min=n_cur;
      if(n_cur>n_max || n_max<0) n_max=n_cur;
      
      // Fill the data class with pedestal and rms values
      larPedestalComplete->set(chid,gain,dg.mean(),dg.RMS());
      NPedestal++;
    }//end loop over all cells

    ATH_MSG_DEBUG( "Gain " << gain << " Number of cells with 0 events to compute pedestal: " <<  n_zero );
    ATH_MSG_DEBUG( "Gain " << gain << " Minimum number of events*samples to compute pedestal: " <<  n_min );
    ATH_MSG_DEBUG( "Gain " << gain << " Maximum number of events*samples to compute pedestal: " <<  n_max );
  }// End loop over all containers
  
  //ATH_MSG_INFO(" Summary : Number of cells with a pedestal value computed : " << larPedestalComplete->totalNumberOfConditions() );
  ATH_MSG_INFO(" Summary : Number of cells with a pedestal value computed : " << NPedestal );
  ATH_MSG_INFO(" Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 ");
  ATH_MSG_INFO(" Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 ");
  ATH_MSG_INFO(" Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 ");
  ATH_MSG_INFO(" Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 ");
  ATH_MSG_INFO(" Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 ");
    
  // Record LArPedestalComplete
  sc = detStore()->record(larPedestalComplete,m_pedContName);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( " Cannot store LArPedestalComplete in TDS " );
    delete larPedestalComplete;
    return sc;
  }
  else
    ATH_MSG_INFO( "Recorded LArPedestalComplete object with key " << m_pedContName );
    
  // Make symlink
  sc = detStore()->symLink(larPedestalComplete, (ILArPedestal*)larPedestalComplete);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( " Cannot make link for Data Object " );
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

