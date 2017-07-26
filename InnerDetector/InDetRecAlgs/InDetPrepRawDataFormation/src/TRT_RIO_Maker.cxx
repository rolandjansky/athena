/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/make_unique.h"


namespace InDet {
  
  ///////////////////////////////////////////////////////////////////
  // Constructior
  ///////////////////////////////////////////////////////////////////
  TRT_RIO_Maker::TRT_RIO_Maker
  (const std::string& name,ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  pTRTHelper(nullptr),
  m_rdoContainerKey("TRT_RDOs"),
  m_driftcircle_tool("InDet::TRT_DriftCircleTool", this), //made private
  m_rioContainerKey("TRT_DriftCircles"),
  m_mode_rio_production(0),
  m_trtBadChannels(0),
  m_roiSeeded(false),
  m_roiCollectionKey(""),
  m_regionSelector("RegSelSvc", name)
  {
    // Read TRT_RIO_Maker steering parameters
    declareProperty("TRTRDOLocation"      ,m_rdoContainerKey  );
    declareProperty("TrtDescrManageLocation",m_trt_mgr_location   );//Remove later
    declareProperty("ModeRIOProduction"   ,m_mode_rio_production);
    //selection of the TRT bad channels(true/false)
    declareProperty("TRTBadChannels"    ,m_trtBadChannels = true );
    declareProperty("TRT_DriftCircleTool", m_driftcircle_tool);
    declareProperty("TRTRIOLocation",  m_rioContainerKey);    
  }
  
  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::initialize() {
    // Get TRT_DriftCircle tool
    ATH_CHECK(m_driftcircle_tool.retrieve());
    ATH_CHECK(detStore()->retrieve(pTRTHelper,"TRT_ID"));

    ATH_CHECK( m_rdoContainerKey.initialize() );
    ATH_CHECK( m_rioContainerKey.initialize() );
    msg(MSG::DEBUG) << "getTRTBadChannels on or off (1/0)" <<m_trtBadChannels<< endmsg;
    return StatusCode::SUCCESS;
  }
  
  //no-op destructor
  TRT_RIO_Maker::~TRT_RIO_Maker() {}
  
  ///////////////////////////////////////////////////////////////////
  // Execute
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::execute() { 
    // TRT_DriftCircle container registration


    SG::WriteHandle<InDet::TRT_DriftCircleContainer> rioContainer(m_rioContainerKey);
    rioContainer = CxxUtils::make_unique<InDet::TRT_DriftCircleContainer>(pTRTHelper->straw_layer_hash_max());

    ATH_CHECK(rioContainer.isValid());
    ATH_MSG_DEBUG( "Container "<< rioContainer.name() << " initialised" );
    SG::ReadHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey);
    ATH_CHECK(rdoContainer.isValid());    
    
    // Get TRT_RDO and produce TRT_RIO collections
    if (!m_roiSeeded) {//Full-scan mode

      for(const auto& rdoCollections : *rdoContainer) {
        const InDetRawDataCollection<TRT_RDORawData>* currentCollection(rdoCollections);
        std::unique_ptr<TRT_DriftCircleCollection> p_rio(m_driftcircle_tool->convert(m_mode_rio_production,
          currentCollection , m_trtBadChannels));
        if(p_rio && !p_rio->empty()) {
           ATH_CHECK(rioContainer->addCollection(p_rio.get(), p_rio->identifyHash()));
           p_rio.release();//Release ownership if sucessfully added to collection
        }
     }
    }else{
      SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
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
            TRT_RDO_Container::const_iterator RDO_collection_iter = rdoContainer->indexFind(id);
            if (RDO_collection_iter == rdoContainer->end()) continue;
            const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection (*RDO_collection_iter);
            if (!RDO_Collection) continue;
            // Use one of the specific clustering AlgTools to make clusters
            std::unique_ptr<TRT_DriftCircleCollection> p_rio(m_driftcircle_tool->convert(m_mode_rio_production,
                RDO_Collection , m_trtBadChannels));    
            if (p_rio && !p_rio->empty()){
#ifndef NDEBUG               
                 ATH_MSG_VERBOSE( "REGTEST: TRT : DriftCircleCollection contains "
                 << p_rio->size() << " clusters" );
#endif
                 ATH_CHECK(rioContainer->addCollection(p_rio.get(), p_rio->identifyHash()));
                 p_rio.release();//Release ownership if sucessfully added to collection
            }
         }
      }
    }
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
