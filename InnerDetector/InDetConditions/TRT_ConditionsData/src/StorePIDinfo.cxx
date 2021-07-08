/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TRT_ConditionsData/StorePIDinfo.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <utility>



StorePIDinfo::StorePIDinfo(){
	m_nbins = 0;
	m_min 	= -9999.9;
	m_max	= 10000000*2;
	m_values.clear();
}

StorePIDinfo::StorePIDinfo(int nbins, float min, float max, std::vector<float> values){
	update (nbins, min, max, std::move(values));
}

StorePIDinfo::~StorePIDinfo(){}

void StorePIDinfo::update( int nbins, float min, float max, std::vector<float> values){
	m_nbins = nbins	;
	m_min 	= min	;
	m_max	= max	;
	if (values.size()!=m_nbins){
          MsgStream log(Athena::getMessageSvc(),"StorePIDinfo");
	  log << MSG::ERROR << " Different Values of n_bins and vector size!!!" << endmsg;
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
        MsgStream log(Athena::getMessageSvc(),"StorePIDinfo");
	if 	( m_nbins == 0)
	{

	  log << MSG::ERROR << " StorePIDinfo: No bins in the DB!! Gas: " << gas << " detpart " << detpart << endmsg;
          return StatusCode::FAILURE;
	}
	else if ( m_nbins != m_values.size() )
	{
	  log << MSG::ERROR << " Different number of PID numbers!!!!! " << endmsg;
          return StatusCode::FAILURE;
	}
	else if ( (m_max < m_min) || (m_max == m_min) )
	{
	  log << MSG::ERROR << " Max is smaller or equal than min!!!" << endmsg;
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
		return bin;
	}
	return 0;
}
