// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_INAVIGABLE4MOMENTUM_H
#define EVENTKERNEL_INAVIGABLE4MOMENTUM_H 1
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//             Interface for navigable 4-momentum data objects               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "EventKernel/I4Momentum.h"
#include "Navigation/INavigable.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "Navigation/IAthenaBarCode.h"

class INavigable4Momentum : public virtual IAthenaBarCode, public virtual INavigable, public virtual I4Momentum
{
 public:

  virtual ~INavigable4Momentum();
};

CLASS_DEF(INavigable4Momentum,36127793, 1)

#endif
