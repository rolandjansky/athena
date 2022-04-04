/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_IAFP_GlobRecoTool_H
#define AFP_GLOBRECO_IAFP_GlobRecoTool_H 1

/// @file   IAFP_GlobRecoTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2021-11-24
/// 
/// @brief  Header file for interface IAFP_GlobRecoTool


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

/// Interface for AFP tools that reconstruct protons from tracks
class IAFP_GlobRecoTool : virtual public ::IAlgTool
{
 public:
  DeclareInterfaceID(IAFP_GlobRecoTool, 1, 0);
  
  /// @brief run protons reconstruction
  /// 
  /// The method calls tools that reconstruct protons.
  virtual StatusCode execute(const EventContext& ctx) const = 0;
}; 


#endif //> !AFP_GLOBRECO_IAFP_GlobRecoTool_H
