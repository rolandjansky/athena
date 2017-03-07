/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_IASCIICONDDBSVC_H
#define ATHEXHIVE_IASCIICONDDBSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDRange.h"

#include <string>


class GAUDI_API IASCIICondDbSvc: virtual public IService {
public:
  DeclareInterfaceID(IASCIICondDbSvc, 1, 0);

  typedef float dbData_t;

  virtual ~IASCIICondDbSvc() = default;

  virtual StatusCode getRange(const std::string&, const EventContext& ctx, 
                              EventIDRange& rng, dbData_t&) const = 0;

  virtual void dump() const = 0;

};

#endif
