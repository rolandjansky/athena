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

namespace InDet {
  
  ///////////////////////////////////////////////////////////////////
  // Constructior
  ///////////////////////////////////////////////////////////////////
  TRT_RIO_Maker::TRT_RIO_Maker
  (const std::string& name,ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  m_trt_rdo_location("TRT_RDOs"),
  m_trt_rio_location("TRT_DriftCircles"),
  m_trt_mgr_location("TRT"),
  m_driftcircle_tool("InDet::TRT_DriftCircleTool"),
  p_riocontainer(0),
  m_mode_rio_production(0),
  m_trtBadChannels(0)
  {
    // Read TRT_RIO_Maker steering parameters
    declareProperty("TRTRIOLocation"      ,m_trt_rio_location   );
    declareProperty("TRTRDOLocation"      ,m_trt_rdo_location   );
    declareProperty("TrtDescrManageLocation",m_trt_mgr_location   );
    declareProperty("ModeRIOProduction"   ,m_mode_rio_production);
    //selection of the TRT bad channels(true/false)
    declareProperty("TRTBadChannels"    ,m_trtBadChannels = true );
    declareProperty("TRT_DriftCircleTool", m_driftcircle_tool);
  }
  
  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::initialize() {
    // Get TRT_DriftCircle tool
    if ( m_driftcircle_tool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << m_driftcircle_tool.propertyName() << ": Failed to retrieve tool " << m_driftcircle_tool << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << m_driftcircle_tool.propertyName() << ": Retrieved tool " << m_driftcircle_tool << endreq;
    }
    const TRT_ID* pTRTHelper;
    if (detStore()->retrieve(pTRTHelper,"TRT_ID").isFailure()) return msg(MSG::ERROR)<<"Could not retrieve the TRT helper"<< endreq, StatusCode::FAILURE;
    
    // Create TRT RIO container
    p_riocontainer= new InDet::TRT_DriftCircleContainer(pTRTHelper->straw_layer_hash_max());
    if(!p_riocontainer) {
      msg(MSG::FATAL)<<"Could not creat TRT_DriftCircleContainer"<<endreq; 
      return StatusCode::FAILURE;
    }
    p_riocontainer->addRef();
    
    msg(MSG::DEBUG) << "getTRTBadChannels on or off (1/0)" <<m_trtBadChannels<< endreq;
    return StatusCode::SUCCESS;
  }
  
  //no-op destructor
  TRT_RIO_Maker::~TRT_RIO_Maker() {}
  
  ///////////////////////////////////////////////////////////////////
  // Execute
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::execute() { 
    // TRT_DriftCircle container registration
    p_riocontainer->cleanup();
    StatusCode s=evtStore()->record(p_riocontainer,m_trt_rio_location);
    if(s.isFailure())   {
      msg(MSG::ERROR)<<"Error while registering TRT_DriftCircle container"<<endreq; 
      return s;
    }
    // Get TRT_RDO and produce TRT_RIO collections
    const DataHandle<TRT_RDO_Container> rdo_container;
    StatusCode st = evtStore()->retrieve(rdo_container,m_trt_rdo_location);
    if(st.isFailure() || !rdo_container.isValid()) {
      if (msgLvl(MSG::DEBUG)) msg()<<"Could not retrieve "<<m_trt_rdo_location<<endreq; 
    }else{
      rdo_container->clID();
      TRT_RDO_Container::const_iterator rdoCollections    = rdo_container->begin();
      TRT_RDO_Container::const_iterator rdoCollectionsEnd = rdo_container->end();
      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections) {
        const InDetRawDataCollection<TRT_RDORawData>* currentCollection(*rdoCollections);
        const InDet::TRT_DriftCircleCollection* p_rio=
        m_driftcircle_tool->convert(m_mode_rio_production, currentCollection , m_trtBadChannels);
        if(p_rio) {
          if (p_rio->size() != 0) {
            s = p_riocontainer->addCollection(p_rio, p_rio->identifyHash()); 
            if(s.isFailure()) {
              msg(MSG::ERROR)<<"Error while registering TRT_DriftCircle collection"<<endreq; 
              return s;
            }
          } else {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Don't write empty collections" << endreq;
            // -me- cleanup memory
            delete (p_rio);
          }
        }
      }
    }
    s = evtStore()->setConst(p_riocontainer);
    if(s.isFailure()) {
      msg(MSG::ERROR)<<" ??? "<<endreq; 
      return s;
    }
    return StatusCode::SUCCESS;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  StatusCode TRT_RIO_Maker::finalize() {
    p_riocontainer->cleanup();
    p_riocontainer->release();
    return StatusCode::SUCCESS;
  }
}
