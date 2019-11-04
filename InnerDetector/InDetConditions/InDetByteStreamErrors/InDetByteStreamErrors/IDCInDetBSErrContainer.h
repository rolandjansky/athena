/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef INDETBYTESTREAMERRORS_IDCINDETBSERRCONTAINER_H
#define INDETBYTESTREAMERRORS_IDCINDETBSERRCONTAINER_H


#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h"
#include "EventContainers/IdentifiableCache.h"

/**
 * IDC like storage for BS errors,
 * TODO, customise implementation further so that we store int rather than int*
 **/

class IDCInDetBSErrContainer : public IdentifiableContainer<int>  {
public:
  typedef int ErrorCode;
  using IdentifiableContainer<ErrorCode>::IdentifiableContainer;
};

CLASS_DEF( IDCInDetBSErrContainer , 1083695787 , 1 )

typedef EventContainers::IdentifiableCache< IDCInDetBSErrContainer::ErrorCode > IDCInDetBSErrContainer_Cache;

CLASS_DEF( IDCInDetBSErrContainer_Cache , 1268216266 , 1 )





#endif // INDETBYTESTREAMERRORS_IDCINDETBSERRCONTAINER_H
