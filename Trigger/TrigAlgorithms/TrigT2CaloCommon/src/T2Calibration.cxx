/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <TrigT2CaloCommon/T2Calibration.h>
#ifndef NDEBUG
#include <iostream>
#endif

T2Calibration::T2Calibration(): m_factor(0) {
}

T2Calibration::~T2Calibration(){
}

void T2Calibration::initialize(const std::vector<float>& limit, const
	std::vector<int>& dimension, const std::vector<float>& correction){


  // if no calibration constants were provided, no calibration
  // will ever happen
  if ( dimension.size() == 0 ) return;
  for(std::vector<float>::const_iterator i=limit.begin(); i != limit.end(); i++)
	m_limit.push_back(*i);
  for(std::vector<int>::const_iterator i=dimension.begin(); 
			i != dimension.end(); i++)
	m_dimension.push_back(*i);
  for(std::vector<float>::const_iterator i=correction.begin();
			i!=correction.end();i++)
	m_correction.push_back(*i);

#ifndef NDEBUG
  // Shows the corrections table for the ql
  int jump = m_dimension[0];
  std::cout << m_dimension[0] << " " << m_dimension[1] << std::endl;
  std::cout << m_limit[0] << " " << m_limit[1] << std::endl;
  for ( int j=0;j<m_dimension[1];j++){
          std::cout << m_correction[jump*j] << " "
                  << m_correction[jump*j+1] << std::endl;
  }
#endif
  m_factor = m_dimension[1];
  m_factor /= (m_limit[1] - m_limit[0]);
} // end of method initialize
