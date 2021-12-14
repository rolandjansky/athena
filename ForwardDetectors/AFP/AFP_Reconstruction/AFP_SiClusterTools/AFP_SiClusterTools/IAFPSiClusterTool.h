/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
#define AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H 1

/// @file   IAFPSiClusterTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-04-18
/// 
/// @brief  Header file for interface IAFPSiClusterTool


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"

/// Interface for AFP tools that create silicon pixel hits clusters
class IAFPSiClusterTool : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(IAFPSiClusterTool, 1, 0);

  /// @brief Run clusters reconstruction
  /// 
  /// This method does full reconstruction from xAOD hits to xAOD
  /// clusters. It reads silicon detector hits from StoreGate, runs
  /// clustering algorithm, assigns clusters positions in station
  /// local coordinate system and saves them in xAOD format to
  /// StoreGate.
  virtual StatusCode clusterHits(std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& clustr, const EventContext &ctx) const = 0;

};


#endif //> !AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
