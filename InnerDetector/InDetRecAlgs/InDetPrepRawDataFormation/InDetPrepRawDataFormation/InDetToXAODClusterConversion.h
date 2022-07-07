/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///
/// @file InDetToXAODClusterConversion.h
/// Algorithm to convert InDet::PixelClusters and InDet::SCT_Clusters
/// to xAOD::PixelClusters and xAOD::StripClusters.
/// This is a temporary solution before a proper
/// clustering algorithm is implemented.
///

#ifndef INDETRIOMAKER_INDETTOXAODCLUSTERCONVERSION_H
#define INDETRIOMAKER_INDETTOXAODCLUSTERCONVERSION_H
//STL
#include <string>

//Gaudi
#include "GaudiKernel/ToolHandle.h"

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

//InDet
//can't fwd declare this, needed for typedef to Pixel_RDO_Container
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

class PixelID;
class SCT_ID;

namespace InDet {

class InDetToXAODClusterConversion : public AthReentrantAlgorithm {
public:

  /// Constructor with parameters:
  InDetToXAODClusterConversion(const std::string &name,ISvcLocator *pSvcLocator);

  //@name Usual algorithm methods
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  //@}
  InDetToXAODClusterConversion() = delete;
  InDetToXAODClusterConversion(const InDetToXAODClusterConversion&) = delete;
  InDetToXAODClusterConversion &operator=(const InDetToXAODClusterConversion&) = delete;
   //@}

private:
  const PixelID* m_pixelID {};
  const SCT_ID* m_stripID {};

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "ITkPixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_stripDetEleCollKey{this, "StripDetEleCollKey", "ITkStripDetectorElementCollection", "Key of SiDetectorElementCollection for Strip"};

  SG::ReadHandleKey<InDet::PixelClusterContainer> m_inputPixelClusterContainerKey {this, "InputPixelClustersName", "ITkPixelClusters", "name of the input InDet pixel cluster container"};
  SG::ReadHandleKey<InDet::SCT_ClusterContainer>  m_inputStripClusterContainerKey {this, "InputStripClustersName", "ITkStripClusters", "name of the input InDet strip cluster container"};

  SG::WriteHandleKey<xAOD::PixelClusterContainer> m_outputPixelClusterContainerKey {this, "OutputPixelClustersName", "ITkPixelClusters", "name of the output xAOD pixel cluster container"};
  SG::WriteHandleKey<xAOD::StripClusterContainer> m_outputStripClusterContainerKey {this, "OutputStripClustersName", "ITkStripClusters", "name of the output xAOD strip cluster container"};
};

}

#endif // INDETRIOMAKER_INDETTOXAODCLUSTERCONVERSION_H

