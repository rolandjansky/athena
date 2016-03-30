/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @file TRT_RIOMaker.h
 *  Header file for class TRT_RIO_Maker
 *  (c) ATLAS Detector software
 *  @author I.Gavrilenko
 **/

#ifndef TRT_RIO_Maker_H
#define TRT_RIO_Maker_H

//STL
#include <string>

//Gaudi
#include "GaudiKernel/ToolHandle.h"

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

//InDet
// no forward declare (typedef?)
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRawData/TRT_RDO_Container.h"
//fwd declarations
class StatusCode;
class ISvcLocator;

namespace InDet{
  class ITRT_DriftCircleTool;

}

namespace InDet{

  /** TRT_DriftCircleContainer production algorithm
   * Top Algorithm for InDetRawDataContainer<TRT_RDORawData> conversion
   * to TRT_DriftCircleContainer
   **/
  class TRT_RIO_Maker : public AthAlgorithm {
  public:
    ///constructor
    TRT_RIO_Maker(const std::string &name, ISvcLocator *pSvcLocator);
    ///no-op destructor
    virtual ~TRT_RIO_Maker()  ;
    /**    @name Usual algorithm methods */
    //@{
    StatusCode initialize ()  ;
    StatusCode execute    ()  ;
    StatusCode finalize   ()  ;
    //@}
  private:
  
    /**    @name Disallow default instantiation, copy, assignment */
    TRT_RIO_Maker() = delete;
    TRT_RIO_Maker(const TRT_RIO_Maker&) = delete;
    TRT_RIO_Maker &operator=(const TRT_RIO_Maker&) = delete;
  
    const TRT_ID* pTRTHelper;
    SG::ReadHandle<TRT_RDO_Container> m_rdo_container;
//    std::string                    m_trt_rdo_location   ;
//    std::string                    m_trt_rio_location   ;
    std::string                    m_trt_mgr_location   ; //REMOVE LATER
    ToolHandle< ITRT_DriftCircleTool > m_driftcircle_tool   ;
    SG::WriteHandle<InDet::TRT_DriftCircleContainer> m_riocontainer       ;
    int                            m_mode_rio_production;
    bool                           m_trtBadChannels;
  };
}//end of namespace
#endif // TRT_RIO_Maker_H
