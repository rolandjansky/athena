/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P1_H


#include <vector>
#include <stdint.h>

class TRT_BSErrContainer_p1
{
 public:
/// Default constructor
  TRT_BSErrContainer_p1 ();
  friend class TRT_BSErrContainerCnv_p1;
 private:
  std::vector<std::pair<uint8_t, uint32_t>* > m_bsErrs;
};

// inlines

inline
TRT_BSErrContainer_p1::TRT_BSErrContainer_p1 () {}

#endif // Not INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P1_H

