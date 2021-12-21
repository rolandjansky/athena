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


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

static const InterfaceID IID_IAFPTDLocRecoTool("IAFPTDLocRecoTool", 1, 0);

/// Interface for AFP tools that reconstruct tracks from time of flight detector hits
class IAFPTDLocRecoTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPTDLocRecoTool() {}

  static const InterfaceID& interfaceID();

  /// @brief run tracks reconstruction
  /// 
  /// The method calls tools that reconstruct tracks.
  virtual StatusCode execute() = 0;
}; 

inline const InterfaceID& IAFPTDLocRecoTool::interfaceID() 
{ 
   return IID_IAFPTDLocRecoTool; 
}


#endif //> !AFP_LOCRECO_IAFPTDLOCRECOTOOL_H
