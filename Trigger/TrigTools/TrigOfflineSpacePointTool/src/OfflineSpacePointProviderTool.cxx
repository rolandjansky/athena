/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// OfflineSpacePointProviderTool
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
//
// Author : Nikos Konstantinidis  
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <algorithm>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "DataModel/DataVector.h"

// EDM & Identifier
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetPrepRawData/SiCluster.h"

// SpacePoint creation
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigOfflineSpacePointTool/OfflineSpacePointProviderTool.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"

// Offline spacepoints

#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"


OfflineSpacePointProviderTool::OfflineSpacePointProviderTool(const std::string& t, 
							     const std::string& n,
							     const IInterface*  p ): 
  AlgTool(t,n,p),
  m_numberingTool("TrigL2LayerNumberTool")
{
  declareInterface< ISpacePointProvider >( this );
  
  declareProperty( "RegionSelectorToolName", m_regionSelectorToolName = "RegSelSvc" );
  declareProperty( "SctSpContainerName", m_sctSpContainerName = "TrigSCT_SpacePoints" );
  declareProperty( "PixelSpContainerName", m_pixelSpContainerName = "TrigPixelSpacePoints" );

}

StatusCode OfflineSpacePointProviderTool::finalize()
{
  m_pixelSpContainer->cleanup();
  delete m_pixelSpContainer;
  m_sctSpContainer->cleanup();
  delete m_sctSpContainer;
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}
 

StatusCode OfflineSpacePointProviderTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  athenaLog << MSG::DEBUG << "OfflineSpacePointProviderTool: in initialize" << endreq;

//   Get StoreGate service
//
  sc = service( "StoreGateSvc", m_StoreGate );
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL 
	      << "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }

 // Get DetectorStore service
  StoreGateSvc * detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  // Get the SCT Detector Description Manager
  const InDetDD::SiDetectorManager * SCT_manager;
  sc=detStore->retrieve(SCT_manager, "SCT");

  if (sc.isFailure()) {
      athenaLog << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
  }

  // Get the Pixel Detector Description Manager
  const InDetDD::SiDetectorManager * pixelManager;
  sc=detStore->retrieve(pixelManager, "Pixel");

  if (sc.isFailure()) {
    athenaLog << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  // Get SCT & pixel Identifier helpers

  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
  }

  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq;
     return StatusCode::FAILURE;
  }

  sc=m_numberingTool.retrieve();
  if (sc.isFailure()){
    athenaLog << MSG::FATAL << "Tool " << m_numberingTool
	      << " not found " << endreq;
    return StatusCode::FAILURE;
  } 

//   Get the region selector tool
//
  sc = serviceLocator()->service( m_regionSelectorToolName, m_regionSelector);
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL 
	      << "Unable to retrieve RegionSelector Service " << m_regionSelectorToolName << endreq;
    return sc;
  };

  athenaLog << MSG::INFO << "PixelSP_ContainerName: " << m_pixelSpContainerName << endreq;
  m_pixelSpContainer = new TrigSiSpacePointContainer(m_pixelId->wafer_hash_max());
  m_pixelSpContainer->addRef();
  athenaLog << MSG::INFO << "SCT_SP_ContainerName: " << m_sctSpContainerName << endreq;
  m_sctSpContainer = new TrigSiSpacePointContainer(m_sctId->wafer_hash_max());
  m_sctSpContainer->addRef();

  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();

  return sc;
}


const std::vector<int>* OfflineSpacePointProviderTool::fillPixelDataErrors()
{
  return &m_pixelDataErrors;
}

const std::vector<int>* OfflineSpacePointProviderTool::fillSCT_DataErrors()
{
  return &m_sctDataErrors;
}


StatusCode OfflineSpacePointProviderTool ::fillCollections(bool getPixelSP, bool getSCT_SP,
							   std::vector<int>& PixelIDs,
							   std::vector<int>& SCT_IDs)
{
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  PixelIDs.clear();SCT_IDs.clear();
  int nPixSp=0,nSCT_SP=0;
  StatusCode sc;

//   Get the Pixel SpacePoints 
//

  if(getPixelSP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_pixelSpContainerName))
	{
	  m_pixelSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_pixelSpContainer,m_pixelSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "Pixel TrigSP Container " << m_pixelSpContainerName 
		  <<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(outputLevel <= MSG::DEBUG)
		athenaLog << MSG::DEBUG << "Pixel TrigSP Container " << m_pixelSpContainerName
			  << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Pixel TrigSP Container " <<m_pixelSpContainerName 
		      << " is found in StoreGate" << endreq;
	}
      std::vector<IdentifierHash> listOfPixIds;
      listOfPixIds.clear();
      m_regionSelector->DetHashIDList(PIXEL, listOfPixIds);
    
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
					       << listOfPixIds.size() << " det. Elements" << endreq;
    
      const SpacePointContainer* container; 
      sc = m_StoreGate->retrieve(container, "PixelSpacePoints");

      if (sc.isFailure())
	{
	  athenaLog << MSG::DEBUG << "cound not retrieve PixelSpacePoints container" << endreq;
	  return sc;
	}
      for(unsigned int i=0; i<listOfPixIds.size(); i++) 
	{
	  SpacePointContainer::const_iterator it = container->indexFind(listOfPixIds[i]);
	  if(it==container->end()) continue;
	  if((*it)->size()==0) continue;

	  IdentifierHash idHash=(*it)->identifyHash();
	  TrigSiSpacePointContainer::const_iterator spCollIt;
	  spCollIt = m_pixelSpContainer->indexFind(idHash);
	  if(spCollIt!=m_pixelSpContainer->end()) continue;

	  const SpacePointCollection* colNext=&(**it);

	  TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);
	  PixelIDs.push_back((int)(idHash));

	  nPixSp+=colNext->size();

	  DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
	  for(nextSpacePoint=colNext->begin();nextSpacePoint!=colNext->end();++nextSpacePoint) 
	    {
	      const Trk::SpacePoint* sp = &(**nextSpacePoint);
	      TrigSiSpacePoint* trigSp=makeTrigPixSP(sp);
	      spacePointColl->push_back(trigSp);
	      if(outputLevel < MSG::DEBUG) 
		athenaLog << MSG::VERBOSE << "PixOff: phi/r/z/dphi/dr/dz " 
			  << trigSp->phi() <<  " / " 
			  << trigSp->r() << " / " 
			  << trigSp->z() << " / "
			  << trigSp->dphi() << " / " 
			  << trigSp->dr() << " / " 
			  << trigSp->dz() << endreq; 
	    }
	  StatusCode sc=m_pixelSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());	  
	  if (sc.isFailure()) 
	    {
	      athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in Pixel with hash "
			<< idHash << endreq;
	    } 
	  else 
	    {
	      if(outputLevel<=MSG::DEBUG)
		athenaLog << MSG::DEBUG << " Pixel TrigSiSpacePoint collection is stored in IDC with hashId "
			  <<spacePointColl->identifyHash()<<endreq;	      
	    }
	}
    }
  if(!getSCT_SP) return StatusCode::SUCCESS;

//   Get the SCT SpacePoints
//  
  if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_sctSpContainerName))
    {
      m_sctSpContainer->cleanup();
      
      sc=m_StoreGate->record(m_sctSpContainer,m_sctSpContainerName,false);
      if(sc.isFailure())
	{
	  athenaLog << MSG::WARNING << "SCT TrigSP Container " << m_sctSpContainerName 
		    <<" cannot be recorded in StoreGate !"<< endreq;
	  return sc;
	}
      else 
	{
	  if(outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "SCT TrigSP Container " << m_sctSpContainerName
		      << " is recorded in StoreGate" << endreq;
	}
    }
  else 
    {    
      if(outputLevel <= MSG::DEBUG)
	athenaLog << MSG::DEBUG << "SCT TrigSP Container " <<m_sctSpContainerName 
		  << " is found in StoreGate" << endreq;
    }
  std::vector<IdentifierHash> listOfSctIds;
  listOfSctIds.clear();
  m_regionSelector->DetHashIDList(SCT, listOfSctIds);

  if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
	    << listOfSctIds.size() << " det. Elements" << endreq;

  const SpacePointContainer* container; 
  sc = m_StoreGate->retrieve(container, "SCT_SpacePoints");
  
  if(sc.isFailure())
    {
      athenaLog << MSG::DEBUG << "cound not retrieve SCT_SpacePoints container" << endreq;
      return sc;
    }
  
  for(unsigned int i=0; i<listOfSctIds.size(); i++) 
    {
      SpacePointContainer::const_iterator it = container->indexFind(listOfSctIds[i]);
      if(it==container->end()) continue;
      if((*it)->size()==0) continue;

      IdentifierHash idHash=(*it)->identifyHash();
      TrigSiSpacePointContainer::const_iterator spCollIt;
      spCollIt = m_sctSpContainer->indexFind(idHash);
      if(spCollIt!=m_sctSpContainer->end()) continue;

      const SpacePointCollection* colNext=&(**it);
      SCT_IDs.push_back((int) idHash);

      nSCT_SP+=(*it)->size();

      TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);

      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
      for(nextSpacePoint=colNext->begin();nextSpacePoint!=colNext->end();++nextSpacePoint) 
	{
	  const Trk::SpacePoint* sp = &(**nextSpacePoint);
	  TrigSiSpacePoint* trigSp=makeTrigSctSP(sp);
	  spacePointColl->push_back(trigSp);
	  if (outputLevel < MSG::DEBUG) 
	    athenaLog << MSG::VERBOSE << "SCTOff: phi/r/z/dphi/dr/dz " 
		      << trigSp->phi() <<  " / " 
		      << trigSp->r() << " / " 
		      << trigSp->z() << " / "
		      << trigSp->dphi() << " / " 
		      << trigSp->dr() << " / " 
		      << trigSp->dz() << endreq; 
	    
	}
      sc=m_sctSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
      if (sc.isFailure()) 
	{
	  athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in SCT with hash "
		    << idHash << endreq;
	  break;
	} 
      else 
	{
	  if(outputLevel<=MSG::DEBUG)
	    athenaLog << MSG::DEBUG << " SCT TrigSiSpacePoint collection is stored in IDC with hashId "
		      <<spacePointColl->identifyHash()<<endreq;	      
	}
    }
  if(outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of SCT + Pixel SP = " << nPixSp+nSCT_SP
	      << endreq;
  return sc;
}

StatusCode OfflineSpacePointProviderTool ::fillCollections(const IRoiDescriptor& roi, bool getPixelSP, bool getSCT_SP,
							   std::vector<int>& PixelIDs,
							   std::vector<int>& SCT_IDs)
{
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  PixelIDs.clear();SCT_IDs.clear();
  int nPixSp=0,nSCT_SP=0;
  StatusCode sc;

//   Get the Pixel SpacePoints 
//

  if(getPixelSP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_pixelSpContainerName))
	{
	  m_pixelSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_pixelSpContainer,m_pixelSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "Pixel TrigSP Container " << m_pixelSpContainerName 
		  <<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(outputLevel <= MSG::DEBUG)
		athenaLog << MSG::DEBUG << "Pixel TrigSP Container " << m_pixelSpContainerName
			  << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Pixel TrigSP Container " <<m_pixelSpContainerName 
		      << " is found in StoreGate" << endreq;
	}
      std::vector<IdentifierHash> listOfPixIds;
      listOfPixIds.clear();
      m_regionSelector->DetHashIDList(PIXEL, roi, listOfPixIds);
    
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
					       << listOfPixIds.size() << " det. Elements" << endreq;
    
      const SpacePointContainer* container; 
      sc = m_StoreGate->retrieve(container, "PixelSpacePoints");

      if (sc.isFailure())
	{
	  athenaLog << MSG::DEBUG << "cound not retrieve PixelSpacePoints container" << endreq;
	  return sc;
	}
      for(unsigned int i=0; i<listOfPixIds.size(); i++) 
	{
	  SpacePointContainer::const_iterator it = container->indexFind(listOfPixIds[i]);
	  if(it==container->end()) continue;
	  if((*it)->size()==0) continue;

	  IdentifierHash idHash=(*it)->identifyHash();
	  TrigSiSpacePointContainer::const_iterator spCollIt;
	  spCollIt = m_pixelSpContainer->indexFind(idHash);
	  if(spCollIt!=m_pixelSpContainer->end()) continue;

	  const SpacePointCollection* colNext=&(**it);

	  TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);
	  PixelIDs.push_back((int)(idHash));

	  nPixSp+=colNext->size();

	  DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
	  for(nextSpacePoint=colNext->begin();nextSpacePoint!=colNext->end();++nextSpacePoint) 
	    {
	      const Trk::SpacePoint* sp = &(**nextSpacePoint);
	      TrigSiSpacePoint* trigSp=makeTrigPixSP(sp);
	      spacePointColl->push_back(trigSp);
	      if(outputLevel < MSG::DEBUG) 
		athenaLog << MSG::VERBOSE << "PixOff: phi/r/z/dphi/dr/dz " 
			  << trigSp->phi() <<  " / " 
			  << trigSp->r() << " / " 
			  << trigSp->z() << " / "
			  << trigSp->dphi() << " / " 
			  << trigSp->dr() << " / " 
			  << trigSp->dz() << endreq; 
	    }
	  StatusCode sc=m_pixelSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());	  
	  if (sc.isFailure()) 
	    {
	      athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in Pixel with hash "
			<< idHash << endreq;
	    } 
	  else 
	    {
	      if(outputLevel<=MSG::DEBUG)
		athenaLog << MSG::DEBUG << " Pixel TrigSiSpacePoint collection is stored in IDC with hashId "
			  <<spacePointColl->identifyHash()<<endreq;	      
	    }
	}
    }
  if(!getSCT_SP) return StatusCode::SUCCESS;

//   Get the SCT SpacePoints
//  
  if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_sctSpContainerName))
    {
      m_sctSpContainer->cleanup();
      
      sc=m_StoreGate->record(m_sctSpContainer,m_sctSpContainerName,false);
      if(sc.isFailure())
	{
	  athenaLog << MSG::WARNING << "SCT TrigSP Container " << m_sctSpContainerName 
		    <<" cannot be recorded in StoreGate !"<< endreq;
	  return sc;
	}
      else 
	{
	  if(outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "SCT TrigSP Container " << m_sctSpContainerName
		      << " is recorded in StoreGate" << endreq;
	}
    }
  else 
    {    
      if(outputLevel <= MSG::DEBUG)
	athenaLog << MSG::DEBUG << "SCT TrigSP Container " <<m_sctSpContainerName 
		  << " is found in StoreGate" << endreq;
    }
  std::vector<IdentifierHash> listOfSctIds;
  listOfSctIds.clear();
  m_regionSelector->DetHashIDList(SCT, roi, listOfSctIds);

  if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
	    << listOfSctIds.size() << " det. Elements" << endreq;

  const SpacePointContainer* container; 
  sc = m_StoreGate->retrieve(container, "SCT_SpacePoints");
  
  if(sc.isFailure())
    {
      athenaLog << MSG::DEBUG << "cound not retrieve SCT_SpacePoints container" << endreq;
      return sc;
    }
  
  for(unsigned int i=0; i<listOfSctIds.size(); i++) 
    {
      SpacePointContainer::const_iterator it = container->indexFind(listOfSctIds[i]);
      if(it==container->end()) continue;
      if((*it)->size()==0) continue;

      IdentifierHash idHash=(*it)->identifyHash();
      TrigSiSpacePointContainer::const_iterator spCollIt;
      spCollIt = m_sctSpContainer->indexFind(idHash);
      if(spCollIt!=m_sctSpContainer->end()) continue;

      const SpacePointCollection* colNext=&(**it);
      SCT_IDs.push_back((int) idHash);

      nSCT_SP+=(*it)->size();

      TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);

      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
      for(nextSpacePoint=colNext->begin();nextSpacePoint!=colNext->end();++nextSpacePoint) 
	{
	  const Trk::SpacePoint* sp = &(**nextSpacePoint);
	  TrigSiSpacePoint* trigSp=makeTrigSctSP(sp);
	  spacePointColl->push_back(trigSp);
	  if (outputLevel < MSG::DEBUG) 
	    athenaLog << MSG::VERBOSE << "SCTOff: phi/r/z/dphi/dr/dz " 
		      << trigSp->phi() <<  " / " 
		      << trigSp->r() << " / " 
		      << trigSp->z() << " / "
		      << trigSp->dphi() << " / " 
		      << trigSp->dr() << " / " 
		      << trigSp->dz() << endreq; 
	    
	}
      sc=m_sctSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
      if (sc.isFailure()) 
	{
	  athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in SCT with hash "
		    << idHash << endreq;
	  break;
	} 
      else 
	{
	  if(outputLevel<=MSG::DEBUG)
	    athenaLog << MSG::DEBUG << " SCT TrigSiSpacePoint collection is stored in IDC with hashId "
		      <<spacePointColl->identifyHash()<<endreq;	      
	}
    }
  if(outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of SCT + Pixel SP = " << nPixSp+nSCT_SP
	      << endreq;
  return sc;
}

spacePointVec OfflineSpacePointProviderTool::getSpacePoints(bool getPixelSP, bool getSctSP)
{
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

//   Get the Pixel SpacePoints 
//
  spacePointVec spVec;
  if (getPixelSP) {
    const SpacePointContainer* container; 
    StatusCode sc = m_StoreGate->retrieve(container, "PixelSpacePoints");

    if (sc.isFailure()){
      athenaLog << MSG::DEBUG << "cound not retrieve PixelSpacePoints container" << endreq;
      return spVec;
    }

    SpacePointContainer::const_iterator it = container->begin();
    SpacePointContainer::const_iterator itEnd = container->end();
    for ( ; it != itEnd; ++it ) {
      
      const SpacePointCollection* colNext=&(**it);
      
      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
      for(nextSpacePoint=colNext->begin() ; 
	  nextSpacePoint!=colNext->end() ; ++nextSpacePoint) {
	
	const Trk::SpacePoint* sp = &(**nextSpacePoint);
	spVec.push_back( this->makeTrigPixSP( sp ) );
      }
    }

    if (outputLevel <= MSG::DEBUG) {    
      for (unsigned int i=0; i<spVec.size(); i++) {
	athenaLog << MSG::DEBUG << "pixOff: " << spVec[i]->z() << " / " 
		  << spVec[i]->phi() << " / "
		  << spVec[i]->dr() << " / " 
		  << spVec[i]->dphi() << " / " 
		  << spVec[i]->dz() << endreq; 
      }
    }
  }
  if ( !getSctSP) return spVec;


//   Get the SCT SpacePoints
//  
  const SpacePointContainer* container; 
  StatusCode sc = m_StoreGate->retrieve(container, "SCT_SpacePoints");
  
  if (sc.isFailure()){
    athenaLog << MSG::DEBUG << "cound not retrieve SCT_SpacePoints container" << endreq;
    return spVec;
  }
    
  std::vector< TrigSiSpacePoint* > spVecSCT;
  SpacePointContainer::const_iterator it = container->begin();
  SpacePointContainer::const_iterator itEnd = container->end();
  for ( ; it != itEnd; ++it ) {

    const SpacePointCollection* colNext=&(**it);
    
    DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
    for(nextSpacePoint=colNext->begin() ; 
	nextSpacePoint!=colNext->end() ; ++nextSpacePoint) {
      
      const Trk::SpacePoint* sp = &(**nextSpacePoint);
      spVecSCT.push_back( this->makeTrigSctSP( sp ) );
    }
  }
  
  if (outputLevel <= MSG::DEBUG) {    
    for (unsigned int i=0; i<spVecSCT.size(); i++) {
      athenaLog << MSG::DEBUG << "pixOff: " << spVecSCT[i]->z() << " / " 
		<< spVecSCT[i]->phi() << " / "
		<< spVecSCT[i]->dr() << " / " 
		<< spVecSCT[i]->dphi() << " / " 
		<< spVecSCT[i]->dz() << endreq; 
    }
  }
    
  int nPixSP = spVec.size(); 
  spVec.resize(spVec.size()+spVecSCT.size());
  std::copy(spVecSCT.begin(), spVecSCT.end(), spVec.begin()+nPixSP);
  
  return spVec;

}

//-------------------------------------------------------------------------
TrigSiSpacePoint* OfflineSpacePointProviderTool::makeTrigPixSP( const Trk::SpacePoint* sp )  const
//-------------------------------------------------------------------------
{
  const Identifier elementId = m_pixelId->wafer_id( (sp->elementIdList()).first );
  bool isEndCap = m_pixelId->barrel_ec(elementId);

  double dr,df,dz;
  if ( isEndCap ){
    dz=0.01;
    dr=0.13;
  }else{
    dr=0.01;
    dz=0.13;
  }
  df=0.0001;
  
  long layerId = m_pixelId->layer_disk(elementId);
  if ( isEndCap ) { // endcap
    layerId += m_numberingTool->offsetEndcapPixels();
  } 

  return new TrigSiSpacePoint( dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).first), 
			       elementId, layerId,
			       (sp->globalPosition()).perp(), (sp->globalPosition()).phi(), (sp->globalPosition()).z(),
			       dr, df, dz );
}

//-------------------------------------------------------------------------

TrigSiSpacePoint* OfflineSpacePointProviderTool::makeTrigSctSP( const Trk::SpacePoint* sp )  const
//-------------------------------------------------------------------------
{
  const Identifier elementId = m_sctId->wafer_id( (sp->elementIdList()).first );
  bool isBarrel = m_sctId->is_barrel(elementId);

  double dr,df,dz;
  if ( isBarrel ){
    dr=0.01;
    dz=0.82;
  }else{
    dz=0.01;
    dr=0.82;
  }
  df=0.00005;
  
  long layerId = m_sctId->layer_disk(elementId);

  if (isBarrel){
    layerId += m_numberingTool->offsetBarrelSCT();
  }
  else{
    layerId += m_numberingTool->offsetEndcapSCT();
  }

  return new TrigSiSpacePoint( dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).first), 
			       dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).second),
			       elementId, layerId,
			       (sp->globalPosition()).perp(), (sp->globalPosition()).phi(), (sp->globalPosition()).z(),
			       dr, df, dz );
}



