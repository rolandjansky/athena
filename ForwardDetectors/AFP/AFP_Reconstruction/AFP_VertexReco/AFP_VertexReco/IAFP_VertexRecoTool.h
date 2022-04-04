/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_VERTEXRECO_IAFP_VertexRecoTool_H
#define AFP_VERTEXRECO_IAFP_VertexRecoTool_H 1

/// @file   IAFP_VertexRecoTool.h
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-12-14
/// 
/// @brief  Header file for interface IAFP_VertexRecoTool


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

/// Interface for AFP tools that reconstruct protons from tracks
class IAFP_VertexRecoTool : virtual public ::IAlgTool
{
 public:
  DeclareInterfaceID(IAFP_VertexRecoTool, 1, 0);
  
  /// @brief run protons reconstruction
  /// 
  /// The method calls tools that reconstruct protons.
  virtual StatusCode execute(const EventContext& ctx) const = 0;
}; 


#endif //> !AFP_VertexRECO_IAFP_VertexRecoTool_H
