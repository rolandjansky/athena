// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER_P2_H
#define INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER_P2_H

#include "Identifier/IdentifierHash.h"

#include <utility>
#include <vector>

class InDetBSErrContainer_p2
{
 public:
  /// Default constructor
  InDetBSErrContainer_p2 () {};
  friend class InDetBSErrContainerCnv_p2;
  friend class IDCInDetBSErrContainerCnv;
 private:
  std::vector<std::pair<IdentifierHash, int>> m_bsErrs;
};

#endif // InDetBSErrContainer_p2
