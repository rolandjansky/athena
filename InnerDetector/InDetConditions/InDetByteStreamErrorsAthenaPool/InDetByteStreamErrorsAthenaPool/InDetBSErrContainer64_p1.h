// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER64BIT_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER64BIT_P1_H

#include "Identifier/IdentifierHash.h"

#include <vector>

class InDetBSErrContainer64_p1
{
 public:
  /// Default constructor
  friend class IDCInDetBSErrContainerCnv_p1;
  InDetBSErrContainer64_p1 () = default;
  std::vector<std::pair<IdentifierHash, uint64_t>> m_bsErrs;
};

#endif // InDetBSErrContainer64_p1
