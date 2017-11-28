/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAFPSIDLOCRECOTRACKALGTOOL_H
#define IAFPSIDLOCRECOTRACKALGTOOL_H 1

/// @file   IAFPSiDLocRecoTrackAlgTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-09-14
/// 
/// @brief  Header file for interface IAFPSiDLocRecoTrackAlgTool


// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "xAODForward/AFPTrackContainer.h"

static const InterfaceID IID_IAFPSiDLocRecoTrackAlgTool("IAFPSiDLocRecoTrackAlgTool", 1, 0);

/// Interface for AFP tools that reconstruct tracks from silicon detector hits
class IAFPSiDLocRecoTrackAlgTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPSiDLocRecoTrackAlgTool() {}

  static const InterfaceID& interfaceID();

  /// @brief run tracks reconstruction
  /// 
  /// The method that does the actual tracks reconstruction. It reads
  /// silicon detector hits from StoreGate, reconstructs tracks and
  /// saves to the container passed in the argument.
  ///
  /// @param outputcontainer pointer to the container in which reconstructed tracks will be saved
  virtual StatusCode reconstructTracks(xAOD::AFPTrackContainer* outputContainer) = 0;

  /// @brief StoreGate name of the container where the reconstructed will be saved 
  virtual const std::string& outputContainerName () const = 0;
}; 

inline const InterfaceID& IAFPSiDLocRecoTrackAlgTool::interfaceID() 
{ 
   return IID_IAFPSiDLocRecoTrackAlgTool; 
}


#endif //>  AFP_LOCRECO_IAFPSIDLOCRECOTOOL_H
