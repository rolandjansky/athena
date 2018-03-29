/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINER_P1_H


#include <vector>
#include <stdint.h>
#include "Identifier/IdentifierHash.h"

class InDetBSErrContainer_p1
{
 public:
/// Default constructor
  InDetBSErrContainer_p1 ();
  friend class InDetBSErrContainerCnv_p1;
 private:
  std::vector<std::pair<IdentifierHash, int32_t>* > m_bsErrs;
};

// inlines

inline
InDetBSErrContainer_p1::InDetBSErrContainer_p1 () {}

#endif
