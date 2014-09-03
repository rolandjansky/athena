/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CLONESERVICE_H
#define ATHENAKERNEL_CLONESERVICE_H
#include "GaudiKernel/StatusCode.h"
#include <string>
class IService;
class Service;
namespace CloneService {
  /** given a reference to a parent svc sets a reference to a cloned child */
  StatusCode clone(const IService* parent, const std::string& childName,
		   Service*& child);
}
#endif
