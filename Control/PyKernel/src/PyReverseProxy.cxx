/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PyKernel/PyReverseProxy.h"

std::unordered_map<std::string, PyReverseProxy *> PyReverseProxy::s_proxyMap;
