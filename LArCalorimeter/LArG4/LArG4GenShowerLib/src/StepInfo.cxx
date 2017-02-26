/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4GenShowerLib/StepInfo.h"

#include "GaudiKernel/MsgStream.h"

//namespace ShowerLib {

	ShowerLib::StepInfo::StepInfo(const StepInfo& first, const StepInfo& second)
	{
		double esum = first.m_dep + second.m_dep;
		double w1 = 0;
		double w2 = 0;

		if (esum > 0) {
			w1 =  first.m_dep/esum;
			w2 =  second.m_dep/esum;
		}

		m_pos = w1*first.m_pos + w2*second.m_pos;
		m_time = w1*first.m_time + w2*second.m_time;
		m_dep = esum;
		m_valid = true;

	}

  double ShowerLib::StepInfo::diff2(const StepInfo& other) const
  {
    return (this->position().diff2(other.position()));
  }

  ShowerLib::StepInfo& ShowerLib::StepInfo::operator+=(const ShowerLib::StepInfo& other)
  {
    double esum = m_dep + other.m_dep; 
    double w1 = 0;
    double w2 = 0;

    if (esum > 0) {	
      w1 =  m_dep/esum;
      w2 =  other.m_dep/esum;
    } else {
      std::cout << "Error: hit combination: sum of deposited energy is zero!" << std::endl;
    }

    m_pos = w1*m_pos + w2*other.m_pos;
    m_dep += other.m_dep;

    return *this;
  }
