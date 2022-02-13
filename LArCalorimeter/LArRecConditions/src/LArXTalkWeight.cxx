/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArXTalkWeight.h"

LArXTalkWeight::LArXTalkWeight(const std::vector<float>& vec, const float factor, const std::vector<int>& table){
	m_vector.insert(m_vector.end(),vec.begin(),vec.end() ); 
	bool factor1;
	if ( fabsf(factor-1.0) < 0.001 ) factor1=true;
	else factor1=false;

	if ( !factor1 ) for(auto& el : m_vector) el*=factor;
	m_table.insert(m_table.end(), table.begin(), table.end() );
}
