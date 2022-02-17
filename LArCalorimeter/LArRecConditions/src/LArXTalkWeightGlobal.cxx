/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArXTalkWeightGlobal.h"

LArXTalkWeightGlobal::LArXTalkWeightGlobal(const std::vector<const LArXTalkWeight*>& vec){
	m_vector.insert(m_vector.end(),vec.begin(),vec.end() ); 
}

