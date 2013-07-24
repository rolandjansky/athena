/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCalibShortCorrector.h"

//#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadChannel.h"
#include <math.h>

LArCalibShortCorrector::LArCalibShortCorrector(const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_storeGateSvc(0),
  m_detStore(0),
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
  m_log=NULL;
  m_shortsCached=false;
}

LArCalibShortCorrector::~LArCalibShortCorrector()  {
  delete m_log;
}

StatusCode LArCalibShortCorrector::initialize() {
  m_log = new MsgStream(msgSvc(),name());
  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {(*m_log) << MSG::FATAL << " Cannot locate StoreGateSvc " << std::endl;
      return StatusCode::FAILURE;
    } 
  
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
    return StatusCode::FAILURE;
  } 
  
  sc = m_detStore->retrieve(m_onlineId, "LArOnlineID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = m_detStore->retrieve(m_caloCellId, "CaloCell_ID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get LArEM_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Retrieve LArCablingService
  sc = m_larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << " Can't get LArCablingSvc " << m_larCablingSvc << endreq;
    return sc;
  }

  // Retrieve LArCablingService
  sc = m_badChannelTool.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << " Can't get LArBadChanelTool " << m_badChannelTool << endreq;
    return sc;
  }


  sc = m_detStore->regHandle(m_larPedestal,m_pedKey);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << "Could not register DataHandle for pedestal with key " << m_pedKey  << endreq;
    return sc;
  }

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
       (*m_log) << MSG::DEBUG << "Channel " << chid1.get_compact() << " marked as short" << endreq;
       //Find neighbor
       std::vector<IdentifierHash> neighbors;
       m_caloCellId->get_neighbours(id1_h,LArNeighbours::faces2D,neighbors);
       HWIdentifier chid2;
       if (neighbors.size()==0) {
	 (*m_log) << MSG::ERROR << "No neighbors found for channel with id " << chid1.get_compact() << endreq;
	 return StatusCode::FAILURE;
       }
       else
	 (*m_log) << MSG::DEBUG << " Found " << neighbors.size() << " neighbors found for channel with id " << chid1.get_compact() << endreq; 

       
       std::vector<IdentifierHash>::const_iterator nbrit=neighbors.begin();
       std::vector<IdentifierHash>::const_iterator nbrit_e=neighbors.end();
       for (;nbrit!=nbrit_e;++nbrit) {
	 const HWIdentifier chid_nbr=m_larCablingSvc->createSignalChannelIDFromHash(*nbrit);
	 if (m_badChannelTool->status(chid_nbr).shortProblem()) { //Found neighbor with 'short'
	   if (chid2.is_valid()) {
	     (*m_log) << MSG::ERROR << "Found more than one neighbor with short bit set! Identifiers: "
		 << chid1.get_compact() << ", " << chid2.get_compact() << ", " << chid_nbr.get_compact() << endreq;
	     return StatusCode::FAILURE;
	   }
	   else {
	     chid2=chid_nbr;
	     (*m_log) << MSG::DEBUG << "Found pair " << chid1.get_compact() << " " << chid2.get_compact()  << endreq;
	   }
	 }
       }//End loop over neighbors
       if (!chid2.is_valid()) {
	 (*m_log) << MSG::ERROR << "No neighbor with 'short' bit set for channel with id: " << chid1.get_compact() << endreq;
	 return StatusCode::FAILURE;
       }
       m_shortedNeighbors.push_back(std::make_pair(chid1,chid2));
     }//end this channel has a short
   }//End loop over all identifiers

   if (this->outputLevel() <= MSG::INFO) {
     (*m_log) << MSG::INFO << "Found " << m_shortedNeighbors.size() << " pairs of shorted neighbors" << endreq;
     std::vector<std::pair<HWIdentifier,HWIdentifier> >::const_iterator itm=m_shortedNeighbors.begin();
     std::vector<std::pair<HWIdentifier,HWIdentifier> >::const_iterator itm_e=m_shortedNeighbors.end();
     for (;itm!=itm_e;itm++) 
       (*m_log) << MSG::INFO << " Shorted pair: " << itm->first << ", " << itm->second << endreq;
   }

   return StatusCode::SUCCESS;
}



StatusCode LArCalibShortCorrector::execute(){
  if (!m_shortsCached){
    StatusCode sc=findShortedNeighbors();
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to build list of shorted neighbors" << endreq;
      return sc;
    }
    m_shortsCached=true;
  }
  const size_t nShorts=m_shortedNeighbors.size();
  
  //Loop over all digits in all containers to find the shorted ones
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  
  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
  
  // now start to deal with digits   
  
  for (;key_it!=key_it_e;key_it++) { // Loop over all containers that are to be processed (e.g. different gains)
    StatusCode sc = m_storeGateSvc->retrieve(larAccumulatedCalibDigitContainer,*key_it);
    if (sc.isFailure()){ 
      (*m_log) << MSG::WARNING << "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it << endreq;
      continue; // Try next container
    }
    

    std::vector<std::pair<LArAccumulatedCalibDigit*, LArAccumulatedCalibDigit*> > shortedDigits;
    shortedDigits.resize(nShorts);

    LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator it_e=larAccumulatedCalibDigitContainer->end();    
    if(it == it_e) {
      (*m_log) << MSG::VERBOSE << "LArAccumulatedCalibDigitContainer with key = " << *key_it << " is empty " << endreq;
      //return StatusCode::SUCCESS;
      continue; // Try next container
    } else {
      (*m_log) << MSG::DEBUG << "Processing LArAccumulatedCalibDigitContainer with key = " << *key_it 
	       << ". Size: " << larAccumulatedCalibDigitContainer->size() << endreq;
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
	(*m_log) << MSG::WARNING << "Second channel of a shorted pair not found in data. Try factor 2 correction." << endreq;
	shortedDigits[ii].second=shortedDigits[ii].first;
      }

      const std::vector<uint32_t>& sampleSumsPulsed=shortedDigits[ii].first->sampleSum();
      const std::vector<uint32_t>& sampleSumsNeighbor=shortedDigits[ii].second->sampleSum();

      std::vector<uint32_t> newSampleSum(sampleSumsPulsed.size());
      

      if (sampleSumsPulsed.size() != sampleSumsNeighbor.size()) {
	(*m_log) << MSG::ERROR << "Inconsistent size of samples-vector!" << endreq;
	return StatusCode::FAILURE;
      }

      if (shortedDigits[ii].first->nTriggers() != shortedDigits[ii].second->nTriggers()) {
	(*m_log) << MSG::ERROR << "Inconsistent number of Triggers!" << endreq;
	return StatusCode::FAILURE;
      }


      float pedestal = m_larPedestal->pedestal(shortedDigits[ii].second->hardwareID(),
								   shortedDigits[ii].second->gain());
      if (pedestal<= (1.0+LArElecCalib::ERRORCODE)) {
	(*m_log) << MSG::ERROR << "Failed to get pedestal for channel with id " << shortedDigits[ii].second->hardwareID().get_compact()
		 << std::endl;
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
  (*m_log) << MSG::INFO << "in stop." << endreq; 
  return StatusCode::SUCCESS;
}

