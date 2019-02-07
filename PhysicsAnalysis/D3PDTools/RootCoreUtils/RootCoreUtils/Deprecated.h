/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ROOT_CORE_UTILS__DEPRECATED_H
#define ROOT_CORE_UTILS__DEPRECATED_H

#include <RootCoreUtils/Global.h>

#if __cplusplus < 201402L
#define RCU_DEPRECATED(X)			\
  __attribute__((deprecated(X)))
#else
#define RCU_DEPRECATED(X)			\
  [[deprecated(X)]]
#endif

#endif
