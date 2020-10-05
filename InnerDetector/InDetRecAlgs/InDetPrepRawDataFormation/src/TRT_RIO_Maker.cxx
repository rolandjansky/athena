/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TRT_RIO_Maker.cxx
 * Implementation file for class TRT_RIO_Maker
 * (c) ATLAS Detector software
 * @author I.Gavrilenko
 * @date 20/02/2004 
 * (modified s.roe, 18.12.2008)
 **/

#include "InDetPrepRawDataFormation/TRT_RIO_Maker.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "InDetIdentifier/TRT_ID.h"
//
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/DataHandle.h"



namespace InDet {
  
  ///////////////////////////////////////////////////////////////////
  // Constructior
  ///////////////////////////////////////////////////////////////////
  TRT_RIO_Maker::TRT_RIO_Maker
  (const std::string& name,ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name,pSvcLocator),
  m_pTRTHelper(nullptr),
  m_rdoContainerKey("TRT_RDOs"),
  m_driftcircle_tool("InDet::TRT_DriftCircleTool", this), //made private
  m_rioContainerKey("TRT_DriftCircles"),
  m_mode_rio_production(0),
  m_trtBadChannels(0),
  m_regionSelector("RegSelSvc", name),
  m_rioContainerCacheKey("")
  {
    // Read TRT_RIO_Maker steering parameters
    declareProperty("TRTRDOLocation"      ,m_rdoContainerKey  );
    declareProperty("TrtDescrManageLocation",m_trt_mgr_location   );//Remove later
    declareProperty("ModeRIOProduction"   ,m_mode_rio_production);
    //selection of the TRT bad channels(true/false)
    declareProperty("TRTBadChannels"    ,m_trtBadChannels = true );
    declareProperty("TRT_DriftCircleTool", m_driftcircle_tool);
    declareProperty("TRTRIOLocation",  m_rioContainerKey);
    declareProperty("TRTRIOCache",  m_rioContainerCacheKey);
  }
  
  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::initialize() {
    // Get TRT_DriftCircle tool
    ATH_CHECK(m_driftcircle_tool.retrieve());
    ATH_CHECK(detStore()->retrieve(m_pTRTHelper,"TRT_ID"));
    if (m_roiSeeded.value()) {
      ATH_CHECK(m_roiCollectionKey.initialize());
      ATH_CHECK(m_regionSelector.retrieve());
    }

    ATH_CHECK( m_rdoContainerKey.initialize() );
    ATH_CHECK( m_rioContainerKey.initialize() );
    ATH_CHECK( m_rioContainerCacheKey.initialize(!m_rioContainerCacheKey.key().empty()) );
    msg(MSG::DEBUG) << "getTRTBadChannels on or off (1/0)" <<m_trtBadChannels<< endmsg;
    return StatusCode::SUCCESS;
  }
  
  //no-op destructor
  TRT_RIO_Maker::~TRT_RIO_Maker() {}
  
  ///////////////////////////////////////////////////////////////////
  // Execute
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::execute(const EventContext& ctx) const {
    // TRT_DriftCircle container registration


    SG::WriteHandle<InDet::TRT_DriftCircleContainer> rioContainer(m_rioContainerKey, ctx);
    if(m_rioContainerCacheKey.key().empty()){
      rioContainer = std::make_unique<InDet::TRT_DriftCircleContainer>(m_pTRTHelper->straw_layer_hash_max());
    }else{
      SG::UpdateHandle<TRT_DriftCircleContainerCache> clusterContainercache(m_rioContainerCacheKey, ctx);
      ATH_CHECK( rioContainer.record (std::make_unique<TRT_DriftCircleContainer>(clusterContainercache.ptr()) ));
    }

    ATH_CHECK(rioContainer.isValid());
    ATH_MSG_DEBUG( "Container "<< rioContainer.name() << " initialised" );
    SG::ReadHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey, ctx);
    ATH_CHECK(rdoContainer.isValid());    
    
    // Get TRT_RDO and produce TRT_RIO collections
    if (!m_roiSeeded) {//Full-scan mode

      for(const auto& rdoCollections : *rdoContainer) {
        const InDetRawDataCollection<TRT_RDORawData>* currentCollection(rdoCollections);
        InDet::TRT_DriftCircleContainer::IDC_WriteHandle lock = rioContainer->getWriteHandle(currentCollection->identifyHash());
        if( lock.OnlineAndPresentInAnotherView() ) continue;
        std::unique_ptr<TRT_DriftCircleCollection> p_rio(m_driftcircle_tool->convert(m_mode_rio_production,
          currentCollection, ctx, m_trtBadChannels));
        if(p_rio && !p_rio->empty()) {
           ATH_CHECK(lock.addOrDelete(std::move(p_rio)));
        }
     }
    }else{
      SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey, ctx);
      ATH_CHECK(roiCollection.isValid());
      std::vector<IdentifierHash> listOfTRTIds;
      for(const TrigRoiDescriptor* roi : *roiCollection){
         
         listOfTRTIds.clear(); //Prevents needless memory reallocations
         m_regionSelector->DetHashIDList( TRT, *roi, listOfTRTIds);
#ifndef NDEBUG
          ATH_MSG_VERBOSE(*roi);
          ATH_MSG_VERBOSE( "REGTEST: SCT : Roi contains " 
		     << listOfTRTIds.size() << " det. Elements" );
#endif   
         for(auto &id : listOfTRTIds){
            const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection (rdoContainer->indexFindPtr(id));
            if (!RDO_Collection) continue;
            InDet::TRT_DriftCircleContainer::IDC_WriteHandle lock = rioContainer->getWriteHandle(id);
            if( lock.OnlineAndPresentInAnotherView() ) continue;

            // Use one of the specific clustering AlgTools to make clusters
            std::unique_ptr<TRT_DriftCircleCollection> p_rio(m_driftcircle_tool->convert(m_mode_rio_production,
                RDO_Collection , ctx, m_trtBadChannels));
            if (p_rio && !p_rio->empty()){
#ifndef NDEBUG               
                 ATH_MSG_VERBOSE( "REGTEST: TRT : DriftCircleCollection contains "
                 << p_rio->size() << " clusters" );
#endif
                 ATH_CHECK(lock.addOrDelete(std::move(p_rio)));

            }
         }
      }
    }
    ATH_MSG_DEBUG("rioContainer->numberOfCollections() " <<  rioContainer->numberOfCollections());
    ATH_CHECK(rioContainer.setConst());
    return StatusCode::SUCCESS;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::finalize() {
    return StatusCode::SUCCESS;
  }
}
