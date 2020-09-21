/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArFebErrorSummary.h"

// static constants
const std::string LArFebErrorSummary::m_errorStrings[N_LArFebErrorType] = {
    "Parity","BCID","SampleHeader","EVTID","ScacStatus","ScaOutOfRange",
    "GainMismatch","TypeMismatch","NumOfSamples","EmptyDataBlock","DspBlockSize","CheckSum", "MissingHeader",
    "BadGain" } ;


//Constructor
LArFebErrorSummary::LArFebErrorSummary() {

}

LArFebErrorSummary::~LArFebErrorSummary() 
{}

uint16_t LArFebErrorSummary::feb_error(HWIdentifier id) const 
{ 

  std::map<unsigned int ,uint16_t>::const_iterator it =  m_feb_errors.find(id.get_identifier32().get_compact() ) ;  

  if (it!=m_feb_errors.end() ) {
    return (*it).second ; 
  }

  return 0;

}


bool LArFebErrorSummary::set_feb_error( unsigned int id, uint16_t error) {

  std::map<unsigned int, uint16_t>::const_iterator it =  m_feb_errors.find(id) ;  

  if (it!=m_feb_errors.end() ) {
    return false;  
  }

  m_feb_errors[id]= error; 

  return true ;
}

const std::map<unsigned int,uint16_t>& LArFebErrorSummary::get_all_febs() const 
{
  return m_feb_errors; 
}

void  LArFebErrorSummary::clear()
{
  m_feb_errors.clear(); 
  return; 
}


std::string LArFebErrorSummary::error_to_string(uint16_t error )
{
  std::string str_err; 
  for (int i=0;i<N_LArFebErrorType;i++)
    {
      if ( error & (1<<i) )
	{
	  if (str_err!="") str_err+=":" ; 
	  str_err = str_err+m_errorStrings[i];
	}
    }

  return str_err; 

}
