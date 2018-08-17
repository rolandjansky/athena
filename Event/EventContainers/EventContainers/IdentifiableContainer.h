/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINER_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINER_H
//IdentifiableContainer is now a wrapper to IDCMT
#include "EventContainers/IdentifiableContainerMT.h"
//template alias
template<class T>
using IdentifiableContainer = IdentifiableContainerMT<T>;
#endif 
