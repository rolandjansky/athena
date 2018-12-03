/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSIDERRCONTAINER_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSIDERRCONTAINER_P1_H

#include <stdint.h>
#include <vector>

class TRT_BSIdErrContainer_p1
{
 public:
/// Default constructor
  TRT_BSIdErrContainer_p1 ();
  friend class TRT_BSIdErrContainerCnv_p1;
 private:
  std::vector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> >* > m_bsErrs;
};

// inlines

inline
TRT_BSIdErrContainer_p1::TRT_BSIdErrContainer_p1 () {}

#endif // Not INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSIDERRCONTAINER_P1_H

