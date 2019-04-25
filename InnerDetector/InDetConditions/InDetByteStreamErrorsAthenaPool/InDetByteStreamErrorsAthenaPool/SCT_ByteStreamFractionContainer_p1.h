// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINER_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINER_P1_H

#include <vector>
#include <cstdint>
#include <set>

class SCT_ByteStreamFractionContainer_p1
{
 public:
  /// Default constructor
  SCT_ByteStreamFractionContainer_p1();
  friend class SCT_ByteStreamFractionContainerCnv_p1;
 private:
  std::vector<std::set<uint32_t> > m_trueSets;
  std::vector<std::set<uint32_t> > m_falseSets;
};

inline
SCT_ByteStreamFractionContainer_p1::SCT_ByteStreamFractionContainer_p1() {}

#endif // INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINER_P1_H
