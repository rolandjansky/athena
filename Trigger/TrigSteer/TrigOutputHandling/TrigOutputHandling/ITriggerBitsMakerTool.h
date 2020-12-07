/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_ITRIGGERBITSMAKERTOOL_H
#define TRIGOUTPUTHANDLING_ITRIGGERBITSMAKERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include <boost/dynamic_bitset.hpp>

/**
 * @class ITriggerBitsMakerTool
 * @brief Base class for AlgTools providing a getBits interface to generate trigger bits
 **/
class ITriggerBitsMakerTool : virtual public IAlgTool {
public: 
  DeclareInterfaceID(ITriggerBitsMakerTool, 1, 0);

  virtual StatusCode getBits(boost::dynamic_bitset<uint32_t>& passRaw,
    boost::dynamic_bitset<uint32_t>& prescaled,
    boost::dynamic_bitset<uint32_t>& rerun,
    const EventContext& ctx) const = 0;

  virtual ~ITriggerBitsMakerTool() override = default;
}; 

#endif // TRIGOUTPUTHANDLING_ITRIGGERBITSMAKERTOOL_H
