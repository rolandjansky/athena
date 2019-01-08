/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  SCT_SerialNumber.cxx
 *  SCT_Cabling
 *
 *  Created by sroe on 21/10/2008.
 *
 */

#include "SCT_Cabling/SCT_SerialNumber.h"
#include <sstream>
#include <iostream>
#include <iomanip>

SCT_SerialNumber::SCT_SerialNumber(): m_truncatedNumber(0){
  //nop
}

SCT_SerialNumber::SCT_SerialNumber(const std::string & snString){
  if (snString.substr(0,5)==std::string("20220")){
    m_truncatedNumber=std::stoi(snString.substr(5));
  } else {
    m_truncatedNumber=0;
  }
}

SCT_SerialNumber::SCT_SerialNumber(const unsigned long long fullSerialNumber){
  if (fullSerialNumber > 20220000000000LL){
    m_truncatedNumber=static_cast<unsigned int>(fullSerialNumber - 20220000000000LL);
  } else {
    m_truncatedNumber=0;
  }
}

SCT_SerialNumber::SCT_SerialNumber(const long long fullSerialNumber){
  if (fullSerialNumber > 20220000000000LL){
    m_truncatedNumber=static_cast<unsigned int>(fullSerialNumber - 20220000000000LL);
  } else {
    m_truncatedNumber=0;
  }
}


SCT_SerialNumber::SCT_SerialNumber(const unsigned int truncatedSerialNumber){
  m_truncatedNumber=truncatedSerialNumber;
}

SCT_SerialNumber::SCT_SerialNumber(const int truncatedSerialNumber){
  m_truncatedNumber=static_cast<unsigned int>(truncatedSerialNumber);
}


std::string
SCT_SerialNumber::str() const{
  if (m_truncatedNumber){
    std::ostringstream os;
    os<<"20220"<<std::setfill('0')<<std::setw(9)<<m_truncatedNumber;
    return os.str();
  } else {
    return std::string("0");
  }
}

unsigned int
SCT_SerialNumber::to_uint() const{
  return m_truncatedNumber;
}

unsigned long long
SCT_SerialNumber::to_ulonglong() const{
  return m_truncatedNumber?(20220000000000LL + m_truncatedNumber):0LL;
}

SCT_SerialNumber::operator unsigned int() const{
  return to_uint();
}

bool
SCT_SerialNumber::isWellFormed() const{
  return m_truncatedNumber!=0;
}

