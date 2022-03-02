/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARXTALKWEIGHTGLOBAL_H
#define LARRECCONDITIONS_LARXTALKWEIGHTGLOBAL_H

#include <vector>
#include "LArRecConditions/LArXTalkWeight.h"

class LArXTalkWeightGlobal {
  public :
  enum XTALK_TYPE { STRIP=0, STRIP_EC, MIDDLEBACK, MIDDLEBACK_ECOW, MIDDLEBACK_ECIW, STRIPMIDDLE, STRIPMIDDLE_EC, TWOSTRIP, TWOSTRIP_EC, MIDDLE1, MIDDLE2, MIDDLE1_EC, MIDDLE2_EC };
   LArXTalkWeightGlobal(const std::vector<const LArXTalkWeight*>&);
   inline float get_xtalk(const XTALK_TYPE, const int eta) const;
   inline float get_xtalk(const XTALK_TYPE, const int region, const int eta) const;
  private:
   std::vector<const LArXTalkWeight*> m_vector;
};

float LArXTalkWeightGlobal::get_xtalk(const XTALK_TYPE type,const int eta) const {
	return m_vector.at(type)->get_xtalk(eta);
}
float LArXTalkWeightGlobal::get_xtalk(const XTALK_TYPE type, const int region, const int eta) const{
	return m_vector.at(type)->get_xtalk(region,eta);
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArXTalkWeightGlobal, 156852140 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArXTalkWeightGlobal> , 245834740, 1)
#endif
