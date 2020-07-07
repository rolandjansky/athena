/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/RegionLogicalOperation.h"

#include "iostream"
#include <cstdlib>

namespace MuonCalib {

bool RegionLogicalOperation::AddRegion(const RegionSelectorBase *region, bool invert)
	{
	if(m_regions.size() > m_operator.size()) return false;
	m_regions.push_back(region);
	m_inverse.push_back(invert);
	return true;
	}
	
bool RegionLogicalOperation::AddOperator(bool op)
	{
	if((m_regions.size() <= m_operator.size())) return false;
	m_operator.push_back(op);
	return true;
	}
	
bool RegionLogicalOperation::Result(const MuonFixedId & id) const
	{
	if(m_regions.size()==0) return true;
	bool ret(m_inverse[0] xor m_regions[0]->Result(id));
	for(unsigned int i=0; i<m_operator.size(); i++)
		{
		bool this_region_result(m_inverse[i+1] xor m_regions[i+1]->Result(id));
		if(m_operator[i])
			{
			ret = ret || this_region_result;
			}
		else
			{
			ret = ret && this_region_result;
			}
		}
	return ret;
	}

void RegionLogicalOperation::Print(std::ostream & os) const
	{
	os<<"(";
	for(unsigned int i=0; i<m_regions.size(); i++)
		{
		if(m_inverse[i]) os<<"!";
		m_regions[i]->Print(os);
		if(i<m_operator.size())
			{
			if(m_operator[i]) os<<" | ";
			else os<<" & ";
			}
		}
	os<<")";
	}

}
