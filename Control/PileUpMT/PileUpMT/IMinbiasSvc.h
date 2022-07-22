/* -*- C++ -*- */
/*
  Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMINBIASSVC_H
#define IMINBIASSVC_H

#include "GaudiKernel/IService.h"
#include "StoreGate/StoreGateSvc.h"

#include <cstdint>
#include <vector>

class IMinbiasSvc : virtual public IService {
  public:
    virtual StatusCode beginHardScatter(std::uint64_t hs_id) = 0; /// Tell IMinbiasSvc we're
                                                                  /// starting a hard scatter
    virtual StoreGateSvc* getMinbias(std::uint64_t hs_id,
                                     std::uint64_t mb_id) = 0;  /// Get a minbias event
    virtual StatusCode endHardScatter(std::uint64_t hs_id) = 0; /// Tell IMinbiasSvc we're done with
                                                                /// a hard scatter

    virtual const std::vector<std::uint64_t>& indexList() = 0;

    /// Create InterfaceID
    DeclareInterfaceID(IMinbiasSvc, 1, 0);
};

#endif // IMINBIASSVC_H
