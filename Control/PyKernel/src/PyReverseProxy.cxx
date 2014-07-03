/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PyKernel/PyReverseProxy.h"

std::map<std::string, PyReverseProxy *> PyReverseProxy::m_proxyMap
= std::map<std::string, PyReverseProxy *>();
