/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMARECBUILDER_H
#define EGAMMAALGS_EGAMMARECBUILDER_H
/**
  @class egammaRecBuilder 

  This is algorithm produces the initial egammaRecs as a step of the
  egamma supercluster algorithms.

*/

// INCLUDE HEADER FILES:
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "egammaRecEvent/egammaRecContainer.h"

class IEMTrackMatchBuilder;
class IEMConversionBuilder;

class egammaRecBuilder : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  egammaRecBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();
  
 private:

  /** @brief retrieve EMTrackMatchBuilder **/
  StatusCode RetrieveEMTrackMatchBuilder();
  /** @brief retrieve EMConversionBuilder **/
  StatusCode RetrieveEMConversionBuilder();
  /** @brief Key for the topo cluster input collection */
  SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_inputTopoClusterContainerKey {this,
      "InputTopoClusterContainerName", "egammaTopoCluster",
      "Name of input cluster container"};

  /** @brief Key for egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_egammaRecContainerKey {this, 
      "egammaRecContainer", "egammaRecCollection",
      "Output container for egammaRec objects"};

  //
  // The tools
  //
  /** @brief Tool to perform track matching*/
  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder {this,
      "TrackMatchBuilderTool", "EMTrackMatchBuilder",
      "Tool that matches tracks to egammaRecs"};

  /** @brief Tool to perfrom conversion vertex matching*/
  ToolHandle<IEMConversionBuilder> m_conversionBuilder {this,
      "ConversionBuilderTool", "EMConversionBuilder",
      "Tool that matches conversion vertices to egammaRecs"};

  //
  // All booleans
  //
  /** @brief private member flag to do the track matching */
  Gaudi::Property<bool> m_doTrackMatching {this, "doTrackMatching", true,
      "Boolean to do track matching"};

  /** @brief private member flag to do the conversion matching */
  Gaudi::Property<bool> m_doConversions {this, "doConversions", true,
      "Boolean to do conversion matching"};
  //
  // Other properties.
  //
  IChronoStatSvc* m_timingProfile;
};

#endif
