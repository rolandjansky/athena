/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBALRECOTOOLS_IAFPLOCALTOGLOBALCSTOOL_H
#define AFP_GLOBALRECOTOOLS_IAFPLOCALTOGLOBALCSTOOL_H 1

/// @file   IAFPLocalToGlobalCSTool.h
/// @author Grzegorz Gach <grzegorz.gach@cern.ch>
/// @date   2017-05-13
/// 
/// @brief  Header file for interface IAFPLocalToGlobalCSTool


// STL includes

#include<TVector3.h>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

#include "xAODForward/AFPTrack.h"

static const InterfaceID IID_IAFPLocalToGlobalCSTool("IAFPLocalToGlobalCSTool", 1, 0);

/// Interface for AFP tools transforming tracks from local to global coordinates.
class IAFPLocalToGlobalCSTool : virtual public ::IAlgTool
{ 
 public: 
  virtual ~IAFPLocalToGlobalCSTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  /// @brief Calculate track coordinates in global system
  /// 
  /// Using AFP track with position saved in station coordinate
  /// system, position of the track in LHC coordinate system is
  /// calculated and returned in form of TVector3.
  virtual TVector3 localToGlobalCS(const xAOD::AFPTrack* localTrack) const = 0;

  /// @brief Calculate track coordinates in global system
  ///
  /// @param localX position of the track along X axis in station coordinate system
  /// @param localY position of the track along Y axis in station coordinate system
  /// @param localZ position of the track along Z axis in station coordinate system
  /// @param stationID station ID number where track is reconstructed
  virtual TVector3 localToGlobalCS(const double localX, const double localY, const double localZ, const int stationID) const = 0;

}; 

inline const InterfaceID& IAFPLocalToGlobalCSTool::interfaceID() 
{ 
   return IID_IAFPLocalToGlobalCSTool; 
}


#endif //> !AFP_GLOBALRECOTOOLS_IAFPLOCALTOGLOBALCSTOOL_H
