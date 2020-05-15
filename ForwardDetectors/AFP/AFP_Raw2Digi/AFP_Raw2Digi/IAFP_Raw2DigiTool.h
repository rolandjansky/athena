/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAFP_RAW2DIGITOOL_H
#define IAFP_RAW2DIGITOOL_H

/// @file   IAFP_Raw2DigiTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for interface IAFP_Raw2DigiTool 

#include "GaudiKernel/IAlgTool.h"

/// Interface for AFP tool that translates RawData to xAOD::AFPSiHits.
class IAFP_Raw2DigiTool : virtual public IAlgTool {
public:
  // Declaration of the interface ID (interface id, major version, minor version)
  DeclareInterfaceID(IAFP_Raw2DigiTool, 1, 0);
  
  /// Empty destructor
  virtual ~IAFP_Raw2DigiTool() {}

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
};


#endif 
