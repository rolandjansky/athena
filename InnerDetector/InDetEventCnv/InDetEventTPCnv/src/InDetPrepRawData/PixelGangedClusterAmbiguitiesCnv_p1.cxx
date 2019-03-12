/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguitiesCnv_p1.h"
#include "AthLinks/tools/IdentContIndex.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IThinningSvc.h"


void PixelGangedClusterAmbiguitiesCnv_p1::transToPers ATLAS_NOT_THREAD_SAFE
(const InDet::PixelGangedClusterAmbiguities* transObj, InDet::PixelGangedClusterAmbiguities_p1* persObj, MsgStream &log)
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing InDet::PixelGangedClusterAmbiguities" << endmsg;
  
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PixelGangedClusterAmbiguitiesCnv_p1 " << endmsg;
    } 
  }
  
  if (transObj->size() == 0)  {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << "No ganged pixels in this event. PixelAmbiMap has size 0. This happens often on single particle files." << endmsg;
  } 
  else {     
    InDet::PixelGangedClusterAmbiguities::const_iterator itr =   transObj->begin();
    InDet::PixelGangedClusterAmbiguities::const_iterator itrE =  transObj->end();
    InDet::PixelGangedClusterAmbiguities::const_iterator previous_itr = transObj->begin();
    
    const DataHandle<InDet::PixelClusterContainer> dh, dhEnd;
    StatusCode sc = m_storeGate->retrieve(dh, dhEnd);
    if (sc.isFailure()){
      log << MSG::WARNING <<"No containers found!"<< endmsg;
      return;
    }
    
    //Declare a vector of IdentifierHash to store those that survive thinning
    std::vector<IdentifierHash> idhashes_kept_after_thinning;
    
    //Check to see if thinning service has been envoked
    IThinningSvc* g_thinningSvc = IThinningSvc::instance();
    const InDet::SiCluster* ExamplePixelCluster(0);
    
    if ( g_thinningSvc != NULL ) {
      //If so loop over pixel links in the map and look for idhashes that are not to be thinned
      const InDet::PixelClusterContainer* pixelCont = &(*dh);
      InDet::PixelGangedClusterAmbiguities::const_iterator Sampleitr =   transObj->begin();
      InDet::PixelGangedClusterAmbiguities::const_iterator SampleitrE =  transObj->end();
      for( ; Sampleitr != SampleitrE ; Sampleitr++ ) 	  {
	IdentifierHash          SampleidHash  = Sampleitr->first->getHashAndIndex().collHash(); // idHash of collection
	if( g_thinningSvc->index(pixelCont,(std::size_t) SampleidHash) != IThinningSvc::RemovedIdx ) {
	  idhashes_kept_after_thinning.push_back(SampleidHash);
	  if(ExamplePixelCluster == NULL) ExamplePixelCluster = Sampleitr->first; //Only set pixel cluster once
	}
      }
      if ( ExamplePixelCluster == NULL ) {
	if (log.level() <= MSG::DEBUG) log << MSG::DEBUG <<"No ganged SiClusters found that survive thinning!"<< endmsg;
	return;
      }
    }
    else { //Default case of standard ganged pixel map processing
      ExamplePixelCluster = itr->first;  
    }
      
    // only need to check in which container ONE PixelCluster in the map is:
    // there is ONE ambiguity map per CONTAINER
    persObj->m_pixelClusterContainerName = "";
    const InDet::SiCluster* pixelCluster = ExamplePixelCluster;
    unsigned int            index   = pixelCluster->getHashAndIndex().objIndex(); // prd index within collection
    IdentifierHash          idHash  = pixelCluster->getHashAndIndex().collHash(); // idHash of collection
    
    // loop over dhs
    for ( ; dh!=dhEnd; dh++ ) 		  {
      InDet::PixelClusterContainer::const_iterator coll = dh->indexFind(idHash); //matching collection
      // does coll exist?
      // check prd exists in collection
      // check idhaspointer value the same.
      if ( (coll!=dh->end())&& ((*coll)->size()>index) && (pixelCluster==(**coll)[index]) ){
	// okay, so we found the correct PRD in the container.
	// Now set the name to the container correctly
	persObj->m_pixelClusterContainerName = dh.key();
	break; //exit loop, name found
      }
    }
    
    if (persObj->m_pixelClusterContainerName=="")  {
      log << MSG::ERROR<<"Could not find matching PRD container for this PixelCluster! Dumping PRD: "<<*pixelCluster<<endmsg;
    }
    
    std::vector<unsigned int> uintvector;
    const InDet::SiCluster* keyPixelCluster(0);
    const InDet::SiCluster* gangedPixelCluster(0);
    unsigned int count(1);
    for( ; itr != itrE ; itr++ ) {
    
      //Special instruction in the event of thinning, i.e. skip ambiguity map entries which are associated to id hashes destined to be thinned
      //If idhash is not in list to be kept then don't persistify this entry
      //of the pixel map
      if ( g_thinningSvc != NULL &&
	   std::find(idhashes_kept_after_thinning.begin(), 
		     idhashes_kept_after_thinning.end(), 
		     itr->first->getHashAndIndex().collHash() ) == idhashes_kept_after_thinning.end() ) {
	if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Skip persistifying ambiguity map entry for cluster destined to be thinned" << endmsg;
	continue;
      }
      // for clarity assign the elements
      keyPixelCluster            = itr->first;
      const InDet::SiCluster* keyPreviousPixelCluster    = previous_itr->first;
      gangedPixelCluster         = itr->second;
      
      if (keyPixelCluster == keyPreviousPixelCluster) uintvector.push_back(gangedPixelCluster->getHashAndIndex().hashAndIndex());
      else if (keyPixelCluster != keyPreviousPixelCluster)
	{
	  persObj->m_ambiguityMap.push_back(std::pair<unsigned int, std::vector<unsigned int> >(keyPreviousPixelCluster->getHashAndIndex().hashAndIndex(), uintvector));
	  uintvector.clear();
	  uintvector.push_back(gangedPixelCluster->getHashAndIndex().hashAndIndex());
	}
      previous_itr = itr;
      
      //    std::cout << count << "\t" << (itr->first)->identify() << "\tGangedPixel: " << (itr->second)->identify() << std::endl;
      count++;
      //     std::cout << "TPCnv Writ " << keyPixelCluster->getHashAndIndex().hashAndIndex() << "\t" << keyPixelCluster->localPosition()[Trk::x] 
      //               << "\tGangedPixel: " << gangedPixelCluster->getHashAndIndex().hashAndIndex() << "\t" << gangedPixelCluster->localPosition()[Trk::x] << std::endl;
    }
    // pushback the last one!
    persObj->m_ambiguityMap.push_back(std::pair<unsigned int, std::vector<unsigned int> >(keyPixelCluster->getHashAndIndex().hashAndIndex(), uintvector));
  }
}

void  PixelGangedClusterAmbiguitiesCnv_p1::persToTrans(const InDet::PixelGangedClusterAmbiguities_p1* persObj, InDet::PixelGangedClusterAmbiguities* transObj, MsgStream &log) 
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading InDet::PixelGangedClusterAmbiguities" << endmsg;
  
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PixelGangedClusterAmbiguitiesCnv_p1 " << endmsg;
    } 
  }
  
  if (persObj->m_pixelClusterContainerName == "")
  {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << "Name of the pixel cluster container is empty. Most likely cause: input is a single particle file and there were no pixel ambiguities." << endmsg;
  } else
  {    
  const InDet::PixelClusterContainer* pCC(0);
  if (m_storeGate->retrieve(pCC, persObj->m_pixelClusterContainerName).isFailure()) {
    log << MSG::FATAL << "PixelGangedClusterAmbiguitiesCnv_p1: Cannot retrieve "
        << persObj->m_pixelClusterContainerName << endmsg;
  }
    
  std::vector< std::pair<uint32_t, std::vector<uint32_t> > >::const_iterator persItr  = persObj->m_ambiguityMap.begin();
  std::vector< std::pair<uint32_t, std::vector<uint32_t> > >::const_iterator persItrE = persObj->m_ambiguityMap.end();
  
  const InDet::PixelCluster* keyPixelCluster(0);
  const InDet::PixelCluster* gangedPixelCluster(0);
  
  for( ; persItr != persItrE ; persItr++ )
  {
    const std::pair<uint32_t, std::vector<uint32_t> >& mapElement = (*persItr);
    for (std::vector<uint32_t>::const_iterator newItr = (mapElement.second).begin();
                                              newItr != (mapElement.second).end(); ++newItr)
    {
      IdentContIndex keyIdentContIndex(mapElement.first);
      IdentContIndex gangedIdentContIndex(*newItr);
      for (InDet::PixelClusterContainer::const_iterator contItr  = pCC->begin();
                                                        contItr != pCC->end(); ++contItr)
      {
        const InDet::PixelClusterCollection* coll = (*contItr);
        if (coll->identifyHash() == keyIdentContIndex.collHash())
        {
          keyPixelCluster    = coll->at(keyIdentContIndex.objIndex());
          gangedPixelCluster = coll->at(gangedIdentContIndex.objIndex());
          transObj->insert(std::pair<const InDet::PixelCluster*, const InDet::PixelCluster*>(keyPixelCluster,gangedPixelCluster));
//           std::cout << "TPCnv Read "<< keyPixelCluster->getHashAndIndex().hashAndIndex() << "\t" << keyPixelCluster->localPosition()[Trk::x] 
//                     << "\tGangedPixel: " << gangedPixelCluster->getHashAndIndex().hashAndIndex() << "\t" << gangedPixelCluster->localPosition()[Trk::x] << std::endl;
          break; // get out of loop
        }
      }
    }
  }
}
}

StatusCode PixelGangedClusterAmbiguitiesCnv_p1::initialize(MsgStream &log)
{
   // Do not initialize again:
   m_isInitialized=true;
   //   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelGangedClusterAmbiguitiesCnv_p1::initialize called " << endmsg;
   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endmsg;
      return StatusCode::FAILURE;
   }

   //   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
   return StatusCode::SUCCESS;
}
