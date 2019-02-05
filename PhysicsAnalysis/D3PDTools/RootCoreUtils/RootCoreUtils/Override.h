/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ROOT_CORE_UTILS__OVERRIDE_H
#define ROOT_CORE_UTILS__OVERRIDE_H

#include <RootCoreUtils/Global.h>

#if __cplusplus < 201402L
#define RCU_OVERRIDE
#else
#define RCU_OVERRIDE				\
  override
#endif

#endif
