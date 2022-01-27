/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMARECBUILDER_H
#define EGAMMAALGS_EGAMMARECBUILDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthContainers/ConstDataVector.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IEMTrackMatchBuilder;
class IEMConversionBuilder;

/**
  @class egammaRecBuilder

  @brief Produces egammaRec objects starting from clusters.
  The algorithm produces an egammaRec for each input cluster where the matched tracks
  and vertices are linked. These two matchings are done depending on the flags
  doTrackMatching and doConversions, by default true.
  Additionally, if doTrackMatchedView is true a view container of
  the egammaRecs with matched tracks is created.

  Input container:
  - InputClusterContainerName (default=egammaTopoCluster): topo cluster to be
  used to build the egammaRec Output container:
  - egammaRecContainer (default=egammaRecCollection)
  - trackMatchedView   (default = trackMatchedEgammaRecs)

  Note that the vertex and track container are specified in the tools used by
  this algorithm:
  - TrackMatchBuilderTool (default=EMTrackMatchBuilder)
  - ConversionBuilderTool (default=EMConversionBuilder)
  */
class egammaRecBuilder : public AthReentrantAlgorithm
{
public:
  egammaRecBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  /** @brief retrieve TrackMatchBuilderTool (EMTrackMatchBuilder) **/
  StatusCode RetrieveEMTrackMatchBuilder();
  /** @brief retrieve ConversionBuilderTool (EMConversionBuilder) **/
  StatusCode RetrieveEMConversionBuilder();
  /** @brief Key for the topo cluster input collection */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterContainerKey{
    this,
    "InputClusterContainerName",
    "egammaTopoCluster",
    "Name of input cluster container"
  };

  /** @brief Key for egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_egammaRecContainerKey{
    this,
    "egammaRecContainer",
    "egammaRecCollection",
    "Output container for egammaRec objects"
  };

  SG::WriteHandleKey<ConstDataVector<EgammaRecContainer>>
    m_trackMatchedEgammaRecs{
      this,
      "trackMatchedEgammaRecs",
      "trackMatchedEgammaRecs",
      "View container of the track matched egammaRecs"
    };
  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder{
    this,
    "TrackMatchBuilderTool",
    "EMTrackMatchBuilder",
    "Tool that matches tracks to egammaRecs"
  };

  /** @brief Tool to perfrom conversion vertex matching*/
  ToolHandle<IEMConversionBuilder> m_conversionBuilder{
    this,
    "ConversionBuilderTool",
    "EMConversionBuilder",
    "Tool that matches conversion vertices to egammaRecs"
  };

  /** @brief private member flag to do the track matching */
  Gaudi::Property<bool> m_doTrackMatching{ this,
                                           "doTrackMatching",
                                           true,
                                           "Boolean to do track matching" };

  /** @brief private member flag to do the conversion matching */
  Gaudi::Property<bool> m_doConversions{ this,
                                         "doConversions",
                                         true,
                                         "Boolean to do conversion matching" };

  /** @brief private member flag to produce of view of track matched egammaRecs
   */
  Gaudi::Property<bool> m_doTrackMatchedView{
    this,
    "doTrackMatchedView",
    false,
    "Produce a view of the egammaRecs that have an attached track"
  };
};

#endif
