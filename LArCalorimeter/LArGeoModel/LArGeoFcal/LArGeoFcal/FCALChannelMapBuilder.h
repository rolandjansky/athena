/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  FCALChannelMapBuilder.h
 *
 * @brief Helper function for building FCAL Channel Map
 *
 **/

#ifndef LARGEOFCAL_FCALCHANNELMAPBUILDER_H
#define LARGEOFCAL_FCALCHANNELMAPBUILDER_H

#include "GaudiKernel/StatusCode.h"

class StoreGateSvc;
class IRDBAccessSvc;
class IMessageSvc;

namespace LArGeo {

  StatusCode buildFcalChannelMap(StoreGateSvc* detStore
				 , IRDBAccessSvc* paramSvc
				 , IMessageSvc* msgSvc);
}

#endif
