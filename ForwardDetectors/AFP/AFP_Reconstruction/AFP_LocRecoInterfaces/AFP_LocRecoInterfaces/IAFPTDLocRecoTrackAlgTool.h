/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAFPTDLOCRECOTRACKALGTOOL_H
#define IAFPTDLOCRECOTRACKALGTOOL_H 1

/// @file   IAFPTDLocRecoTrackAlgTool.h
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Header file for interface IAFPTDLocRecoTrackAlgTool


// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPToFTrackContainer.h"

// STL includes
#include <string>



static const InterfaceID IID_IAFPTDLocRecoTrackAlgTool("IAFPTDLocRecoTrackAlgTool", 1, 0);

/// Interface for AFP tools that reconstruct tracks from time of fight detector hits
class IAFPTDLocRecoTrackAlgTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPTDLocRecoTrackAlgTool() {}

  static const InterfaceID& interfaceID();

  /// @brief run tracks reconstruction
  /// 
  /// The method that does the actual tracks reconstruction. It reads
  /// time of flight detector hits from StoreGate, reconstructs tracks and
  /// saves to the container passed in the argument.
  ///
  /// @param outputcontainer pointer to the container in which reconstructed tracks will be saved
  virtual StatusCode reconstructTracks(xAOD::AFPToFTrackContainer* outputContainer) = 0;

  /// @brief StoreGate name of the container where the reconstructed will be saved 
  virtual const std::string& outputContainerName () const = 0;
}; 

inline const InterfaceID& IAFPTDLocRecoTrackAlgTool::interfaceID() 
{ 
   return IID_IAFPTDLocRecoTrackAlgTool; 
}


#endif //>  AFP_LOCRECO_IAFPTDLOCRECOTOOL_H
