/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAFP_RAW2DIGITOOL_H
#define IAFP_RAW2DIGITOOL_H

/// @file   IAFP_Raw2DigiTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for interface IAFP_Raw2DigiTool 



#include "GaudiKernel/IAlgTool.h"

// Forward declarations

static const InterfaceID IID_IAFP_Raw2DigiTool ("IAFP_Raw2DigiTool", 1, 0);


/// Interface for AFP tool that translates RawData to xAOD::AFPSiHits.
class IAFP_Raw2DigiTool : virtual public ::IAlgTool {
public:
  static const InterfaceID &interfaceID();

  /// Empty destructor
  virtual ~IAFP_Raw2DigiTool() {}

  /// Tool initialisation
  virtual StatusCode initialize() = 0;

  /// @brief Translates RawData to xAOD::AFPSiHit and saves it to StoreGate
  ///
  /// This is the method that should be called in order to obtain AFP
  /// hits in form of xAOD::AFPSiHitContainer. It should read input raw data from StoreGate and save output also to StoreGate.
  virtual StatusCode recoSiHits() = 0;

  /// @brief Translates RawData to xAOD::AFPToFHit and saves it to StoreGate
  ///
  /// This is the method that should be called in order to obtain AFP
  /// hits in form of xAOD::AFPToFHitContainer. It should read input
  /// raw data from StoreGate and save output also to StoreGate.
  virtual StatusCode recoToFHits() = 0;

  /// Call all reconstruction steps
  virtual StatusCode recoAll() = 0;

  virtual StatusCode finalize() = 0;
};

inline const InterfaceID& IAFP_Raw2DigiTool::interfaceID() 
{ 
  return IID_IAFP_Raw2DigiTool;
}

#endif 
