/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  MbtsReadoutBuilder.h
 *
 * @brief Helper function for building readout geometry of the MBTS
 *
 **/

#ifndef LARGEOENDCAP_MBTSREADOUTBUILDER_H
#define LARGEOENDCAP_MBTSREADOUTBUILDER_H

#include "GaudiKernel/StatusCode.h"
#include <string>
#include <map>

class StoreGateSvc;
class IRDBAccessSvc;
class IMessageSvc;

namespace LArGeo {

  StatusCode buildMbtsReadout(StoreGateSvc* detStore
			      , IRDBAccessSvc* paramSvc
			      , IMessageSvc* msgSvc
			      , double zposMM
			      , const std::map<std::string,unsigned>& trdMap
			      , const std::string& detKey
			      , const std::string& detNode);
  
}

#endif 
