/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
 
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "Identifier/Identifier.h"

//----------------------------------------------------------------//
#include "StoreGate/ActiveStoreSvc.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyGetPrepDataFromEventTool.h"

MboyGetPrepDataFromEventTool::MboyGetPrepDataFromEventTool(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyGetPrepDataFromEventTool>(this);

//Declare the properties

  declareProperty( "CollectMDT"                       , m_CollectMDT                       = 1                  ) ;
  declareProperty( "CollectRPC"                       , m_CollectRPC                       = 1                  ) ;
  declareProperty( "CollectTGC"                       , m_CollectTGC                       = 1                  ) ;
  declareProperty( "CollectCSC"                       , m_CollectCSC                       = 0                  ) ;
  declareProperty( "CollectCscCluster"                , m_CollectCscCluster                = 1                  ) ;

  declareProperty( "MdtPrepDataContainerName"         , m_MdtPrepDataContainerName         = "MDT_DriftCircles" ) ;
  declareProperty( "RpcPrepDataContainerName"         , m_RpcPrepDataContainerName         = "RPC_Measurements" ) ;

  m_TgcPrepDataContainerNameList.clear();
  m_TgcPrepDataContainerNameList.push_back( "TGC_Measurements" ) ;
  declareProperty("TgcPrepDataContainerNameList" , m_TgcPrepDataContainerNameList ) ;
  
  declareProperty("TgcUsePrevBC", m_TGC_use_prevBC=true);
  declareProperty("TgcUseNextBC", m_TGC_use_nextBC=true);
  declareProperty("TgcUseCurrentBC", m_TGC_use_currentBC=true);
  m_TGC_use_allBC=true;
  

  declareProperty( "CscPrepDataContainerName"         , m_CscPrepDataContainerName         = "CSC_Measurements" ) ;
  declareProperty( "CscClustersPrepDataContainerName" , m_CscClustersPrepDataContainerName = "CSC_Clusters"     ) ;
  declareProperty( "ContainerAccessMethodMDT"         , m_ContainerAccessMethodMDT         = 0                  ) ;
  declareProperty( "ContainerAccessMethodRPC"         , m_ContainerAccessMethodRPC         = 0                  ) ;
  declareProperty( "ContainerAccessMethodTGC"         , m_ContainerAccessMethodTGC         = 0                  ) ;
  declareProperty( "ContainerAccessMethodCSC"         , m_ContainerAccessMethodCSC         = 0                  ) ;

  declareProperty( "DoDbg"         , m_DoDbg         = 0                  ) ;

}

MboyGetPrepDataFromEventTool::~MboyGetPrepDataFromEventTool(){}

// Initialize
StatusCode MboyGetPrepDataFromEventTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;
  
  m_TGC_use_allBC=m_TGC_use_prevBC && m_TGC_use_nextBC && m_TGC_use_currentBC;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return StatusCode::FAILURE;
  }


  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= CollectMDT                        " << m_CollectMDT         ) ;
  ATH_MSG_INFO(  "= CollectRPC                        " << m_CollectRPC         ) ;
  ATH_MSG_INFO(  "= CollectTGC                        " << m_CollectTGC         ) ;
  ATH_MSG_INFO(  "= CollectCSC                        " << m_CollectCSC         ) ;
  ATH_MSG_INFO(  "= CollectCscCluster                 " << m_CollectCscCluster  ) ;
  ATH_MSG_INFO(  "= ContainerAccessMethodMDT          " << m_ContainerAccessMethodMDT ) ;
  ATH_MSG_INFO(  "= ContainerAccessMethodRPC          " << m_ContainerAccessMethodRPC ) ;
  ATH_MSG_INFO(  "= ContainerAccessMethodTGC          " << m_ContainerAccessMethodTGC ) ;
  ATH_MSG_INFO(  "= ContainerAccessMethodCSC          " << m_ContainerAccessMethodCSC ) ;
  ATH_MSG_INFO(  "= MdtPrepDataContainerName          " << m_MdtPrepDataContainerName         ) ;
  ATH_MSG_INFO(  "= RpcPrepDataContainerName          " << m_RpcPrepDataContainerName         ) ;

  ATH_MSG_INFO(  "= TgcPrepDataContainerNameList                 " ) ;
  int TgcPrepDataContainerNameListSize =  m_TgcPrepDataContainerNameList.size() ;
  for (int TgcPrepDataContainerNameListItem=0; TgcPrepDataContainerNameListItem<TgcPrepDataContainerNameListSize ; TgcPrepDataContainerNameListItem++){
    ATH_MSG_INFO(  "=                                                " << m_TgcPrepDataContainerNameList[TgcPrepDataContainerNameListItem] ) ;
  }

  ATH_MSG_INFO(  "= CscPrepDataContainerName          " << m_CscPrepDataContainerName         ) ;
  ATH_MSG_INFO(  "= CscClustersPrepDataContainerName  " << m_CscClustersPrepDataContainerName ) ;
  ATH_MSG_INFO(  "= DoDbg  " << m_DoDbg ) ;
  ATH_MSG_INFO(  "================================" ) ;

  //Locate the active store service
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " Cannot get ActiveStoreSvc " ) ;
    return StatusCode::FAILURE;
  }


//Set pointer on AtlasDetectorID
  sc = detStore()->retrieve(p_AtlasDetectorID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get AtlasDetectorID ") ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Found AtlasDetectorID ") ;

  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyGetPrepDataFromEventTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepData(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_DoDbg == 1 ) ATH_MSG_INFO(  "*Dump*************************************"  ) ;
  if ( m_DoDbg == 1 ) ATH_MSG_INFO(  "*Dump  CollectPrepData"  ) ;
  
//Collect MDT
  if (m_CollectMDT!=0) {
    sc=CollectPrepDataMDT(VectorOfpPrepData);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL(  " CollectPrepDataMDT failed" ) ;
      return StatusCode::FAILURE;
    }
  }

//Collect  RPC
  if (m_CollectRPC!=0){
    sc=CollectPrepDataRPC(VectorOfpPrepData);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL(  " CollectPrepDataRPC failed" ) ;
      return StatusCode::FAILURE;
    }
  }

//Collect  TGC
  if (m_CollectTGC!=0){
    sc=CollectPrepDataTGC(VectorOfpPrepData);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL(  " CollectPrepDataTGC failed" ) ;
      return StatusCode::FAILURE;
    }
  }

//Collect  CSC
  if (m_CollectCSC!=0){
    sc=CollectPrepDataCSC(VectorOfpPrepData);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL(  " CollectPrepDataCSC failed" ) ;
      return StatusCode::FAILURE;
    }
  }

//Collect  CSC Clusters
  if (m_CollectCscCluster!=0){
    sc=CollectPrepDataCSCClu(VectorOfpPrepData);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL(  " CollectPrepDataCSCClu failed" ) ;
      return StatusCode::FAILURE;
    }
  }
  
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " CollectPrepData failed" ) ;
    return StatusCode::FAILURE;
  }

  if ( m_DoDbg == 1 ) ATH_MSG_INFO(  "*Dump*************************************"  ) ;

  return StatusCode::SUCCESS;
}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataMDT(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  if (m_ContainerAccessMethodMDT == 0 ) return CollectPrepDataMDT_Method0(VectorOfpPrepData);
  if (m_ContainerAccessMethodMDT == 1 ) return CollectPrepDataMDT_Method1(VectorOfpPrepData);
  
  return StatusCode::SUCCESS;
}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataMDT_Method0(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;

  std::string key = m_MdtPrepDataContainerName;
  const Muon::MdtPrepDataContainer * mdt_container=0;
  sc = evtStore()->retrieve(mdt_container, key);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( " Cannot retrieve " << key << " Container " ) ;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( " Container " << key << " retrieved "  ) ;
  }
  
  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;

    Muon::MdtPrepDataContainer::const_iterator c = mdt_container->begin();
    for ( ; c != mdt_container->end() ; ++c) {
      if ((*c)->size()>0) {

        Muon::MdtPrepDataCollection::const_iterator it = (*c)->begin();
        for ( ; it != (*c)->end() ; ++it) {
        
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ; 
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections

  return StatusCode::SUCCESS;

}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataMDT_Method1(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;

  if ((*m_activeStore)->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {
    for (; mdtCollection != lastColl; ++mdtCollection){
      if (mdtCollection->size()>0) {

        Muon::MdtPrepDataCollection::const_iterator it = mdtCollection->begin();
        for ( ; it != mdtCollection->end() ; ++it) {
          
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections
  } // if stg retrieve was successful

  return StatusCode::SUCCESS;

}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataRPC(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  if (m_ContainerAccessMethodRPC == 0 ) return CollectPrepDataRPC_Method0(VectorOfpPrepData);
  if (m_ContainerAccessMethodRPC == 1 ) return CollectPrepDataRPC_Method1(VectorOfpPrepData);
  
  return StatusCode::SUCCESS;
}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataRPC_Method0(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;

  std::string key = m_RpcPrepDataContainerName;
  const Muon::RpcPrepDataContainer * rpc_container=0;
  sc = evtStore()->retrieve(rpc_container, key);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( " Cannot retrieve " << key << " Container " ) ;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( " Container " << key << " retrieved "  ) ;
  }
  
  const DataHandle<Muon::RpcPrepDataCollection> rpcCollection;
  const DataHandle<Muon::RpcPrepDataCollection> lastColl;

    Muon::RpcPrepDataContainer::const_iterator c = rpc_container->begin();
    for ( ; c != rpc_container->end() ; ++c) {
      if ((*c)->size()>0) {

        Muon::RpcPrepDataCollection::const_iterator it = (*c)->begin();
        for ( ; it != (*c)->end() ; ++it) {
        
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections

  return StatusCode::SUCCESS;

}
StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataRPC_Method1(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  const DataHandle<Muon::RpcPrepDataCollection> rpcCollection;
  const DataHandle<Muon::RpcPrepDataCollection> lastColl;

  if ((*m_activeStore)->retrieve(rpcCollection,lastColl) ==StatusCode::SUCCESS) {
    for (; rpcCollection != lastColl; ++rpcCollection){
      if (rpcCollection->size()>0) {

        Muon::RpcPrepDataCollection::const_iterator it = rpcCollection->begin();
        for ( ; it != rpcCollection->end() ; ++it) {
        
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections
  } // if stg retrieve was successful

  return StatusCode::SUCCESS;

}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataTGC(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  if (m_ContainerAccessMethodTGC == 0 ) return CollectPrepDataTGC_Method0(VectorOfpPrepData);
  if (m_ContainerAccessMethodTGC == 1 ) return CollectPrepDataTGC_Method1(VectorOfpPrepData);
  
  return StatusCode::SUCCESS;
}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataTGC_Method0(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;

  int TgcPrepDataContainerNameListSize =  m_TgcPrepDataContainerNameList.size() ;
  for (int TgcPrepDataContainerNameListItem=0; TgcPrepDataContainerNameListItem<TgcPrepDataContainerNameListSize ; TgcPrepDataContainerNameListItem++){
  
  std::string key = m_TgcPrepDataContainerNameList[TgcPrepDataContainerNameListItem] ;

  const Muon::TgcPrepDataContainer * tgc_container=0;
  sc = evtStore()->retrieve(tgc_container, key);
  if (sc.isFailure()) ATH_MSG_DEBUG( " Cannot retrieve " << key << " Container " ) ;
  if (sc.isFailure()) continue ;
  ATH_MSG_DEBUG( " Container " << key << " retrieved "  ) ;

  const DataHandle<Muon::TgcPrepDataCollection> tgcCollection;
  const DataHandle<Muon::TgcPrepDataCollection> lastColl;

    Muon::TgcPrepDataContainer::const_iterator c = tgc_container->begin();
    for ( ; c != tgc_container->end() ; ++c) {
      if ((*c)->size()>0) {

        Muon::TgcPrepDataCollection::const_iterator it = (*c)->begin();
        for ( ; it != (*c)->end() ; ++it) {
        
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
	    if(!m_TGC_use_allBC)
	    	{
		const Muon::TgcPrepData * tgcPrepData(dynamic_cast<const Muon::TgcPrepData *>(pPrepData));
		if(!tgcPrepData)
			{
			ATH_MSG_WARNING("Element in "<<key<<" is not a TgcPrepData");
			continue;
			}
		uint16_t bc_bitmap=tgcPrepData->getBcBitMap();
		bool use(false);
		use = use || ((Muon::TgcPrepData::BCBIT_PREVIOUS & bc_bitmap) && m_TGC_use_prevBC);
		use = use || ((Muon::TgcPrepData::BCBIT_NEXT & bc_bitmap) && m_TGC_use_nextBC);
		use = use || ((Muon::TgcPrepData::BCBIT_CURRENT & bc_bitmap) && m_TGC_use_currentBC);
		if(!use) continue;
		}
            VectorOfpPrepData.push_back(pPrepData);
            
        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections
    
  } // loop on container names

  return StatusCode::SUCCESS;

}
StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataTGC_Method1(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  const DataHandle<Muon::TgcPrepDataCollection> tgcCollection;
  const DataHandle<Muon::TgcPrepDataCollection> lastColl;

  if ((*m_activeStore)->retrieve(tgcCollection,lastColl) ==StatusCode::SUCCESS) {
    for (; tgcCollection != lastColl; ++tgcCollection){
      if (tgcCollection->size()>0) {

        Muon::TgcPrepDataCollection::const_iterator it = tgcCollection->begin();
        for ( ; it != tgcCollection->end() ; ++it) {

            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections
  } // if stg retrieve was successful

  return StatusCode::SUCCESS;

}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataCSC(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  if (m_ContainerAccessMethodCSC == 0 ) return CollectPrepDataCSC_Method0(VectorOfpPrepData);
  if (m_ContainerAccessMethodCSC == 1 ) return CollectPrepDataCSC_Method1(VectorOfpPrepData);
  
  return StatusCode::SUCCESS;
}

StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataCSC_Method0(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;

  std::string key = m_CscPrepDataContainerName;
  const Muon::CscStripPrepDataContainer * csc_container=0;
  sc = evtStore()->retrieve(csc_container, key);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( " Cannot retrieve " << key << " Container " ) ;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( " Container " << key << " retrieved "  ) ;
  }
  
  const DataHandle<Muon::CscStripPrepDataCollection> cscCollection;
  const DataHandle<Muon::CscStripPrepDataCollection> lastColl;

    Muon::CscStripPrepDataContainer::const_iterator c = csc_container->begin();
    for ( ; c != csc_container->end() ; ++c) {
      if ((*c)->size()>0) {

        Muon::CscStripPrepDataCollection::const_iterator it = (*c)->begin();
        for ( ; it != (*c)->end() ; ++it) {
        
          if ( m_DoDbg == 1 )
          ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
          Amg::Vector3D globalPosition = (*it)->globalPosition();
          if ( m_DoDbg == 1 )
          ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                         << " x  " << globalPosition.x()  
                         << " y  " << globalPosition.y()  
                         << " z  " << globalPosition.z()  
                         ) ;
            
          const Trk::PrepRawData* pPrepData = (*it) ;
          VectorOfpPrepData.push_back(pPrepData);
            
        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections

  return StatusCode::SUCCESS;

}
StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataCSC_Method1(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  const DataHandle<Muon::CscStripPrepDataCollection> cscCollection;
  const DataHandle<Muon::CscStripPrepDataCollection> lastColl;

  if ((*m_activeStore)->retrieve(cscCollection,lastColl) ==StatusCode::SUCCESS) {
    for (; cscCollection != lastColl; ++cscCollection){
      if (cscCollection->size()>0) {

        Muon::CscStripPrepDataCollection::const_iterator it = cscCollection->begin();
        for ( ; it != cscCollection->end() ; ++it) {

          if ( m_DoDbg == 1 )
          ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
          Amg::Vector3D globalPosition = (*it)->globalPosition();
          if ( m_DoDbg == 1 )
          ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                         << " x  " << globalPosition.x()  
                         << " y  " << globalPosition.y()  
                         << " z  " << globalPosition.z()  
                         ) ;
            
          const Trk::PrepRawData* pPrepData = (*it) ;
          VectorOfpPrepData.push_back(pPrepData);
            
        } // loop on digits in collection
      } // if collection size > 0
    } // loop on collections
  } // if stg retrieve was successful

  return StatusCode::SUCCESS;

}


StatusCode MboyGetPrepDataFromEventTool::CollectPrepDataCSCClu(
                                         std::vector<const Trk::PrepRawData*>& VectorOfpPrepData
){

  StatusCode sc = StatusCode::SUCCESS;
  
  std::string key = m_CscClustersPrepDataContainerName;
  const Muon::CscPrepDataContainer* csc_container = 0;
  sc = evtStore()->retrieve(csc_container, key);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( " Cannot retrieve " << key << " Container " ) ;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( " Container " << key << " retrieved "  ) ;
  }
  
  if (csc_container != 0) {
    Muon::CscPrepDataContainer::const_iterator c = csc_container->begin();
    for ( ; c != csc_container->end() ; ++c) {
      
      if ((*c)->size() > 0) {
	
	Muon::CscPrepDataCollection::const_iterator it = (*c)->begin();
	for ( ; it != (*c)->end() ; ++it) {
	  
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify()) ) ;  
            Amg::Vector3D globalPosition = (*it)->globalPosition();
            if ( m_DoDbg == 1 )
            ATH_MSG_INFO(  "*Dump Id " << p_AtlasDetectorID->show_to_string((*it)->identify())  
                           << " x  " << globalPosition.x()  
                           << " y  " << globalPosition.y()  
                           << " z  " << globalPosition.z()  
                           ) ;
            
            const Trk::PrepRawData* pPrepData = (*it) ;
            VectorOfpPrepData.push_back(pPrepData);

	}
      }
    }
  }
  
  return StatusCode::SUCCESS;

}
