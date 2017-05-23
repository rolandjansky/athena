/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCalibShortCorrector.h"

//#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadChannel.h"
#include <math.h>

LArCalibShortCorrector::LArCalibShortCorrector(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_larCablingSvc("LArCablingService"),
  m_badChannelTool("LArBadChanTool"),
  m_onlineId(0),
  m_caloCellId(0)
{
  declareProperty("KeyList", m_keylist,
		  "List of input keys (normally the 'HIGH','MEDIUM','LOW')"); 
  declareProperty("PedestalKey",m_pedKey="Pedestal",
		  "Key of the pedestal object (to be subtracted)");
  declareProperty("BadChannelTool",m_badChannelTool);
  m_shortsCached=false;
}

LArCalibShortCorrector::~LArCalibShortCorrector()  {
}

StatusCode LArCalibShortCorrector::initialize() {
  ATH_CHECK( detStore()->retrieve(m_onlineId, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_caloCellId, "CaloCell_ID") );
  ATH_CHECK( m_larCablingSvc.retrieve() );
  ATH_CHECK( m_badChannelTool.retrieve() );
  ATH_CHECK( detStore()->regHandle(m_larPedestal,m_pedKey) );
  return StatusCode::SUCCESS;
}

StatusCode LArCalibShortCorrector::findShortedNeighbors() {
  //The current implementation assumes that we have very few shorted cells. 
  //Currently we have 2 pairs (one in EMBA, one in EMBC).
  //If we find more shorts, the simple, un-ordered vector and linear search should be 
  //replaced by something faster

  m_shortedNeighbors.clear();
  //Loop over all identifers (maybe better if we would have a loop only over bad-cahnnels)
   std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
   for(;it!=it_e;it++) {
     if (m_badChannelTool->status(*it).shortProblem()) {
       const HWIdentifier chid1=*it;
       //Already found?
       SHORT_IT sit=m_shortedNeighbors.begin();
       SHORT_IT sit_e=m_shortedNeighbors.end();
       for (;sit!=sit_e && sit->second!=chid1;sit++)
         ;
       if (sit!=sit_e) continue; //This short was already found as neighbor of another shorted cell
	
       const Identifier id1=m_larCablingSvc->cnvToIdentifier(chid1);
       const IdentifierHash id1_h=m_caloCellId->calo_cell_hash(id1);
       ATH_MSG_DEBUG ( "Channel " << chid1.get_compact() << " marked as short" );
       //Find neighbor
       std::vector<IdentifierHash> neighbors;
       m_caloCellId->get_neighbours(id1_h,LArNeighbours::faces2D,neighbors);
       HWIdentifier chid2;
       if (neighbors.size()==0) {
	 ATH_MSG_ERROR ( "No neighbors found for channel with id " << m_onlineId->channel_name(chid1) );
	 return StatusCode::FAILURE;
       }
       else
	 ATH_MSG_DEBUG ( " Found " << neighbors.size() << " neighbors found for channel with id " << m_onlineId->channel_name(chid1) );

       
       std::vector<IdentifierHash>::const_iterator nbrit=neighbors.begin();
       std::vector<IdentifierHash>::const_iterator nbrit_e=neighbors.end();
       for (;nbrit!=nbrit_e;++nbrit) {
	 const HWIdentifier chid_nbr=m_larCablingSvc->createSignalChannelIDFromHash(*nbrit);
	 if (m_badChannelTool->status(chid_nbr).shortProblem()) { //Found neighbor with 'short'
	   if (chid2.is_valid()) {
	     ATH_MSG_ERROR ( "Found more than one neighbor with short bit set! Identifiers: "
                             << m_onlineId->channel_name(chid1) << ", "
                             << m_onlineId->channel_name(chid2) << ", "
                             << m_onlineId->channel_name(chid_nbr) );
	     return StatusCode::FAILURE;
	   }
	   else {
	     chid2=chid_nbr;
	     ATH_MSG_DEBUG ( "Found pair " << m_onlineId->channel_name(chid1) << " " <<  m_onlineId->channel_name(chid2) );
	   }
	 }
       }//End loop over neighbors
       if (!chid2.is_valid()) {
	 ATH_MSG_ERROR ( "No neighbor with 'short' bit set for channel with id: " << chid1.get_compact() );
	 return StatusCode::FAILURE;
       }
       m_shortedNeighbors.push_back(std::make_pair(chid1,chid2));
     }//end this channel has a short
   }//End loop over all identifiers

   if (msgLvl(MSG::INFO)) {
     ATH_MSG_INFO ( "Found " << m_shortedNeighbors.size() << " pairs of shorted neighbors" );
     std::vector<std::pair<HWIdentifier,HWIdentifier> >::const_iterator itm=m_shortedNeighbors.begin();
     std::vector<std::pair<HWIdentifier,HWIdentifier> >::const_iterator itm_e=m_shortedNeighbors.end();
     for (;itm!=itm_e;itm++) 
       ATH_MSG_INFO ( " Shorted pair: " << m_onlineId->channel_name(itm->first) << ", " << m_onlineId->channel_name(itm->second) );
   }

   return StatusCode::SUCCESS;
}



StatusCode LArCalibShortCorrector::execute(){
  if (!m_shortsCached){
    ATH_CHECK( findShortedNeighbors() );
    m_shortsCached=true;
  }
  const size_t nShorts=m_shortedNeighbors.size();
  
  //Loop over all digits in all containers to find the shorted ones
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  
  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
  
  // now start to deal with digits   
  
  for (;key_it!=key_it_e;key_it++) { // Loop over all containers that are to be processed (e.g. different gains)
    StatusCode sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
    if (sc.isFailure()){ 
      ATH_MSG_WARNING ( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it );
      continue; // Try next container
    }
    

    std::vector<std::pair<LArAccumulatedCalibDigit*, LArAccumulatedCalibDigit*> > shortedDigits;
    shortedDigits.resize(nShorts);

    LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator it_e=larAccumulatedCalibDigitContainer->end();    
    if(it == it_e) {
      ATH_MSG_VERBOSE ( "LArAccumulatedCalibDigitContainer with key = " << *key_it << " is empty " );
      //return StatusCode::SUCCESS;
      continue; // Try next container
    } else {
      ATH_MSG_DEBUG ( "Processing LArAccumulatedCalibDigitContainer with key = " << *key_it 
                      << ". Size: " << larAccumulatedCalibDigitContainer->size() );
    }

    for(;it!=it_e;++it) { //Loop over calib-digits
      const HWIdentifier chid=(*it)->hardwareID();
      size_t ii;    
      for (ii=0;ii<nShorts;ii++)
	if (m_shortedNeighbors[ii].first==chid || m_shortedNeighbors[ii].second==chid) break;
      if (ii==nShorts) continue; //Not found on the list of shorts
      
      //Got a channel on the list of shorted channels: Remember their pointers!
      if ((*it)->isPulsed()) 
	shortedDigits[ii].first=*it;
      else
	shortedDigits[ii].second=*it;
    }//end loop over cells
    

    for (size_t ii=0;ii<nShorts;ii++) {
      if (shortedDigits[ii].first==NULL) continue; //Not in the data at all, or only the not-pulsed channel in the data
      if (shortedDigits[ii].second==NULL) { //Only the pulsed channel in the data
	ATH_MSG_WARNING ( "Second channel of a shorted pair not found in data. Try factor 2 correction." );
	shortedDigits[ii].second=shortedDigits[ii].first;
      }

      const std::vector<uint32_t>& sampleSumsPulsed=shortedDigits[ii].first->sampleSum();
      const std::vector<uint32_t>& sampleSumsNeighbor=shortedDigits[ii].second->sampleSum();

      std::vector<uint32_t> newSampleSum(sampleSumsPulsed.size());
      

      if (sampleSumsPulsed.size() != sampleSumsNeighbor.size()) {
	ATH_MSG_ERROR ( "Inconsistent size of samples-vector!" );
	return StatusCode::FAILURE;
      }

      if (shortedDigits[ii].first->nTriggers() != shortedDigits[ii].second->nTriggers()) {
	ATH_MSG_ERROR ( "Inconsistent number of Triggers!" );
	return StatusCode::FAILURE;
      }


      float pedestal = m_larPedestal->pedestal(shortedDigits[ii].second->hardwareID(),
								   shortedDigits[ii].second->gain());
      if (pedestal<= (1.0+LArElecCalib::ERRORCODE)) {
	ATH_MSG_ERROR ( "Failed to get pedestal for channel " <<  m_onlineId->channel_name(shortedDigits[ii].second->hardwareID())
                        << ", gain=" << shortedDigits[ii].second->gain() );
	return StatusCode::FAILURE;
      }
      const unsigned int ped=(unsigned)round(double(pedestal)*shortedDigits[ii].second->nTriggers());

     
      for (size_t is=0;is<sampleSumsPulsed.size();++is) {
	//std::cout << "Sample #"<< is << "=" << sampleSumsPulsed[is] << "+" << sampleSumsNeighbor[ii] << "-" << ped << "=";
	newSampleSum[is]=sampleSumsPulsed[is]+(sampleSumsNeighbor[is]-ped);
	//std::cout << newSampleSum[is] << std::endl;
      }
      shortedDigits[ii].first->setSampleSum(newSampleSum);
    }//end loop over pairs of shorted digits
  }//end loop over container keys
  return StatusCode::SUCCESS;
}


StatusCode LArCalibShortCorrector::stop(){ 
  ATH_MSG_INFO ( "in stop." );
  return StatusCode::SUCCESS;
}

