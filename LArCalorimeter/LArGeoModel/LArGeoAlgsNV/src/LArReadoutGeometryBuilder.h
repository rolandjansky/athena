/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  LArReadoutGeometryBuilder.h
 *
 * @brief Helper function for building readout geometries of all LAr subsystems
 **/

#ifndef LARGEOALGSNV_LARREADOUTGEOMETRYBUILDER_H
#define LARGEOALGSNV_LARREADOUTGEOMETRYBUILDER_H

#include <tuple>

class StoreGateSvc;
class LArHVManager;
class IMessageSvc;

class EMBDetectorManager;
class EMECDetectorManager;
class HECDetectorManager;
class FCALDetectorManager;

std::tuple<EMBDetectorManager*,EMECDetectorManager*,HECDetectorManager*,FCALDetectorManager*>
  buildLArReadoutGeometry(StoreGateSvc* detStore
			  , const LArHVManager* hvManager
			  , IMessageSvc* msgSvc
			  , int testbeam
			  , double projectivityDisplacement);

#endif 
