/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARXTALKWEIGHT_H
#define LARRECCONDITIONS_LARXTALKWEIGHT_H

#include <vector>

class LArXTalkWeight {
  public :
   LArXTalkWeight(const std::vector<float>& vec, const float factor, const std::vector<int>& table);
   float get_xtalk(const int eta) const;
   float get_xtalk(const int region, const int eta) const;
  private:
   std::vector<float> m_vector;
   std::vector<int> m_table;
};

inline float LArXTalkWeight::get_xtalk(const int eta) const {
	return m_vector.at(eta);
}
inline float LArXTalkWeight::get_xtalk(const int region, const int eta) const{
	if ( m_table.empty() ) return 0.;
        const int eta2 = eta+m_table.at(region);
	return m_vector.at(eta2);
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArXTalkWeight , 154494079, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArXTalkWeight> , 180267049, 1)
#endif
