/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__DEPRECATED_H
#define ASG_TOOLS__DEPRECATED_H

#if __cplusplus < 201402L
#define ASG_DEPRECATED(X)			\
  __attribute__((deprecated(X)))
#else
#define ASG_DEPRECATED(X)			\
  [[deprecated(X)]]
#endif

#endif
