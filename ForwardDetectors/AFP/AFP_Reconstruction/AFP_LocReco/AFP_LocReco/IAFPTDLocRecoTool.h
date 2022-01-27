/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_LOCRECO_IAFPTDLOCRECOTOOL_H
#define AFP_LOCRECO_IAFPTDLOCRECOTOOL_H 1

/// @file   IAFPTDLocRecoTool.h
/// @author Leszek  Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Header file for interface IAFPTDLocRecoTool


// STL includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

/// Interface for AFP tools that reconstruct tracks from time of flight detector hits
class IAFPTDLocRecoTool : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(IAFPTDLocRecoTool, 1, 0);

  /// @brief run tracks reconstruction
  /// 
  /// The method calls tools that reconstruct tracks.
  virtual StatusCode execute(const EventContext& ctx) const = 0;
};

#endif //> !AFP_LOCRECO_IAFPTDLOCRECOTOOL_H
