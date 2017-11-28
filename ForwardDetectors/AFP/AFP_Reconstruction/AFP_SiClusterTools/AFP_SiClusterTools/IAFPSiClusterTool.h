/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
#define AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H 1

/// @file   IAFPSiClusterTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-04-18
/// 
/// @brief  Header file for interface IAFPSiClusterTool


// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

static const InterfaceID IID_IAFPSiClusterTool("IAFPSiClusterTool", 1, 0);

/// Interface for AFP tools that create silicon pixel hits clusters
class IAFPSiClusterTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPSiClusterTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  /// @brief Run clusters reconstruction
  /// 
  /// This method does full reconstruction from xAOD hits to xAOD
  /// clusters. It reads silicon detector hits from StoreGate, runs
  /// clustering algorithm, assigns clusters positions in station
  /// local coordinate system and saves them in xAOD format to
  /// StoreGate.
  virtual StatusCode clusterHits() = 0;

}; 

inline const InterfaceID& IAFPSiClusterTool::interfaceID() 
{ 
   return IID_IAFPSiClusterTool; 
}


#endif //> !AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
