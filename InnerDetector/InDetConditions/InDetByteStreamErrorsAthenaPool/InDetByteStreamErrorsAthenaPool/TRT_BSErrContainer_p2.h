/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P2_H
#define INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P2_H


#include <vector>
#include <stdint.h>

class TRT_BSErrContainer_p2
{
 public:
/// Default constructor
  TRT_BSErrContainer_p2 () = default;
  std::vector<uint32_t> m_missingError;
  std::vector<uint32_t> m_errorError;
  std::vector<uint32_t> m_sidError;
};


#endif // Not INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINER_P1_H

