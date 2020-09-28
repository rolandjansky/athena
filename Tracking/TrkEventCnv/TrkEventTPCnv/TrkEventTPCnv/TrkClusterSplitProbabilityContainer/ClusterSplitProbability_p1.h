/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ClusterSplitProbability_p1_H_
#define _ClusterSplitProbability_p1_H_

#include "Identifier/Identifier.h"

namespace Trk {
   struct ClusterSplitProbability_p1
   {
      ClusterSplitProbability_p1() : m_id{}, m_splitPrpbability{}, m_isSplit{} {}
      ClusterSplitProbability_p1(unsigned int id, float probability1, float probability2, bool is_split)
         : m_id(id),
           m_splitPrpbability{probability1,probability2},
           m_isSplit(is_split)
      {}

      unsigned int m_id;
      float m_splitPrpbability[2];
      bool m_isSplit;
   };
}
#endif
