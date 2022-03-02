/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_LOCRECO_IAFP_SIDLocRecoTool_H
#define AFP_LOCRECO_IAFP_SIDLocRecoTool_H 1

/// @file   IAFP_SIDLocRecoTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for interface IAFP_SIDLocRecoTool


// STL includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

/// Interface for AFP tools that reconstruct tracks from silicon detector hits
class IAFP_SIDLocRecoTool : virtual public ::IAlgTool
{
 public:
  DeclareInterfaceID(IAFP_SIDLocRecoTool, 1, 0);
  
  /// @brief run tracks reconstruction
  /// 
  /// The method calls tools that reconstruct tracks.
  virtual StatusCode execute(const EventContext& ctx) const = 0;
}; 


#endif //> !AFP_LOCRECO_IAFP_SIDLocRecoTool_H
