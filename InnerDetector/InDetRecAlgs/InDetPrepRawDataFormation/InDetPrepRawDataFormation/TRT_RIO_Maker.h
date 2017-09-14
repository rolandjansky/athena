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
#include "StoreGate/ReadHandleKey.h"

//InDet
// no forward declare (typedef?)
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "IRegionSelector/IRegSelSvc.h"
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
    StatusCode initialize ()  override;
    StatusCode execute    ()  override;
    StatusCode finalize   ()  override;
    //@}
  private:
  
    /**    @name Disallow default instantiation, copy, assignment */
    TRT_RIO_Maker() = delete;
    TRT_RIO_Maker(const TRT_RIO_Maker&) = delete;
    TRT_RIO_Maker &operator=(const TRT_RIO_Maker&) = delete;
  
    const TRT_ID* m_pTRTHelper;
    SG::ReadHandleKey<TRT_RDO_Container> m_rdoContainerKey;
    ToolHandle< ITRT_DriftCircleTool > m_driftcircle_tool;
    SG::WriteHandleKey<InDet::TRT_DriftCircleContainer> m_rioContainerKey;
    std::string                    m_trt_mgr_location   ; //REMOVE LATER
    int                            m_mode_rio_production;
    bool                           m_trtBadChannels;

    bool m_roiSeeded;                                //!< detector manager name in StoreGate
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
    ServiceHandle<IRegSelSvc>     m_regionSelector;     //!< region selector service
    SG::UpdateHandleKey<InDet::TRT_DriftCircleContainerCache> m_rioContainerCacheKey;

  };
}//end of namespace
#endif // TRT_RIO_Maker_H
