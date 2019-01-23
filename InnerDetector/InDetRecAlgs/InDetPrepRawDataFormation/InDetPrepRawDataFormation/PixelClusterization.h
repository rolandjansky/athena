/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelClusterization.h
 * (c) ATLAS Detector software
 * @author D.Calvet, N.Nesvadba, T.Lari 
 * (modified s.roe, Dec 2008)
 **/

#ifndef INDETRIOMAKER_PIXELCLUSTERIZATION_H
#define INDETRIOMAKER_PIXELCLUSTERIZATION_H
//STL
#include <string>

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

//InDet
//can't fwd declare this, needed for typedef to Pixel_RDO_Container
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"


// Fwd declarations
class ISvcLocator;
class StatusCode;
class PixelRDORawData;
class PixelID;

namespace InDetDD{
  class PixelDetectorManager;
}
namespace InDet {
  class PixelGangedAmbiguitiesFinder;
}


namespace InDet {
/**
 * Top Algorithm to clusterize the pixel RDOs
 * (InDetRawDataCollection<PixelRDORawData> -> RecInputCollection)
 * The clustering algorithm is actually a private Tool in the
 * SiClusterizationTool package
 **/
class PixelClusterization : public AthAlgorithm {
public:
  typedef InDetRawDataCollection<PixelRDORawData> COLLECTION;

  /// Constructor with parameters:
  PixelClusterization(const std::string &name,ISvcLocator *pSvcLocator);

  //@name Usual algorithm methods 
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  //@}
  PixelClusterization() = delete;
  PixelClusterization(const PixelClusterization&) = delete;
  PixelClusterization &operator=(const PixelClusterization&) = delete;
   //@}
                              
private:
  ToolHandle< IPixelClusteringTool > m_clusteringTool;
  /// class to find out which clusters shares ganged pixels
  ToolHandle< PixelGangedAmbiguitiesFinder > m_gangedAmbiguitiesFinder; 
  SG::ReadHandleKey<PixelRDO_Container> m_rdoContainerKey;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  ServiceHandle<IRegSelSvc>     m_regionSelector;     //!< region selector service
  std::string m_managerName; 		           //!< detector manager name in StoreGate
  bool m_roiSeeded; 		                   //!< detector manager name in StoreGate
  const PixelID* m_idHelper;
  SG::WriteHandleKey<PixelClusterContainer> m_clusterContainerKey;
  SG::WriteHandleKey<SiClusterContainer> m_clusterContainerLinkKey;
  SG::WriteHandleKey<PixelGangedClusterAmbiguities> m_ambiguitiesMapKey;
  SG::UpdateHandleKey<InDet::PixelClusterContainerCache> m_clusterContainercacheKey;
  const InDetDD::PixelDetectorManager* m_manager;
};

}//end of ns

#endif // INDETRIOMAKER_PIXELCLUSTERIZATION_H
