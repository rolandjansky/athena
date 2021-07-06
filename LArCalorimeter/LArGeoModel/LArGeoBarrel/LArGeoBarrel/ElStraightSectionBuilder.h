/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  ElStraightSectionBuilder.h
 *
 * @brief Helper function for constructing the instances of GeoStraightAccSection
 *        when LAr GeoModel description is built from the SQLite database.
 *        The code for this function has been extracted from 
 *        LArGeoBarrel/BarrelConstruction
 **/

#ifndef LARGEOBARREL_ELSTRAIGHTSECTIONBUILDER_H
#define LARGEOBARREL_ELSTRAIGHTSECTIONBUILDER_H

#include "GaudiKernel/StatusCode.h"

class StoreGateSvc;
class IRDBAccessSvc;
class IMessageSvc;

namespace LArGeo {

  StatusCode buildElStraightSections(StoreGateSvc* detStore
				     , IRDBAccessSvc* paramSvc
				     , IMessageSvc* msgSvc
				     , bool sagging);
  
}

#endif 
