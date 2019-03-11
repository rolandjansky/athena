/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "TRT_ConditionsData/StorePIDinfo.h"

StorePIDinfo::StorePIDinfo(){
	m_nbins = 0;
	m_min 	= -9999.9;
	m_max	= 10000000*2;
	m_values.clear();
}

StorePIDinfo::StorePIDinfo(int nbins, float min, float max, std::vector<float> values){
	update (nbins, min, max, values);
}

StorePIDinfo::~StorePIDinfo(){}

void StorePIDinfo::update( int nbins, float min, float max, std::vector<float> values){
	m_nbins = nbins	;
	m_min 	= min	;
	m_max	= max	;
	if (values.size()!=m_nbins){
    		printf("  ERROR: DIFFERENT Values of n_bins and vector size!!!\n");
	}
	m_values.clear();
	for (unsigned int i = 0; i<values.size(); i++ ){
		m_values.push_back( values.at(i));
	}
}

// THIS HAS TO BE CALLED in order!!!
void StorePIDinfo::push_back( float value ){
	// Just to read the DB
      if 	(m_nbins==0) 		{
			m_nbins = int(value)	;
			m_min   = -9999.9	;
			m_max   = 10000000*2	;
			m_values.clear()	;
      }
      else if 	(m_min < -9999) 	m_min 	= value		;
      else if 	(m_max >  10000000) 	m_max   = value		;
      else	 m_values.push_back(value);
}

StatusCode StorePIDinfo::check( int gas, int detpart) const{
	if 	( m_nbins == 0)
	{
		std::cout << "ERROR: PIDDB no bins in the DB!! Gas: " << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
	else if ( m_nbins != m_values.size() )
	{
		std::cout << "ERROR: PIDDB different number of numbers!!!!! " << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
	else if ( (m_max < m_min) || (m_max == m_min) )
	{
	 	std::cout << "ERROR: PIDDB Max is smaller or equal than min!!!" << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
        return StatusCode::SUCCESS;
}


float StorePIDinfo::GetValue 	( float input  ) const {
	return m_values.at(	GetBin(	input	)	);
}

float StorePIDinfo::GetBinValue 	( int bin) const {
	return m_values.at(	bin	);
}

int StorePIDinfo::GetBin	( float input  ) const {
	if (input < m_min) 		return 0;
        else if (input >= m_max) 	return m_nbins-1;
	else{
		float dr = (m_max-m_min)/m_nbins;
		unsigned int bin = int (                       (input - m_min)/dr    ) ;
		if 	(bin >=  m_nbins)  	printf("  ERROR: Bin number is larger than number of bins!!!\n");
		return bin;
	}
	return 0;
}
