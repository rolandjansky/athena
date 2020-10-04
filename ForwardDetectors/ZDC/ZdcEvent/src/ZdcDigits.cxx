/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : ZdcDigit.cxx
//  Author   : Steinberg
//  Created  : March 2009
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "ZdcEvent/ZdcDigits.h"

#include <iostream>
#include <sstream>
#include <iomanip>

ZdcDigits::ZdcDigits( const Identifier& id)

  : ZdcRawData( id )
{
  m_nSamples = 0;
}

/*
ZdcDigits::ZdcDigits( const HWIdentifier& HWid,
                        const std::vector<int>& digits )

  : ZdcRawData( HWid )
  , m_digits ( digits )
{
}

ZdcDigits::ZdcDigits( const HWIdentifier& HWid,
                        const std::vector<short>& digits )

  : ZdcRawData( HWid )
{
  m_digits.reserve(digits.size());

  std::vector<short>::const_iterator it1=digits.begin();
  std::vector<short>::const_iterator it2=digits.end();

  double dig;
  for ( ; it1!=it2; ++it1) {
    dig = (*it1);
    m_digits.push_back(dig);
  }  
}
*/

void ZdcDigits::print() const
{

  std::cout << "Trying to print() ZdcDigits" << std::endl;
  
  //std::cout << (std::string) (*(ZdcRawData *)this);
  //std::cout << "ID=" << identify() << std::endl;
  //std::cout << std::setiosflags( std::ios::fixed );
  //std::cout << std::setiosflags( std::ios::showpoint );
  //std::cout << std::setprecision(2);
  
  std::cout << "gain0/delay0 (" << m_digits_gain0_delay0.size() << " elements): ";
  for (int val : m_digits_gain0_delay0) {
    std::cout << " " << val;
  }
  std::cout << std::endl;

  std::cout << "gain0/delay1 (" << m_digits_gain0_delay1.size() << " elements): ";
  for (int val : m_digits_gain0_delay1) {
    std::cout << " " << val;
  }
  std::cout << std::endl;

  std::cout << "gain1/delay0 (" << m_digits_gain1_delay0.size() << " elements): ";
  for (int val : m_digits_gain1_delay0) {
    std::cout << " " << val;
  }
  std::cout << std::endl;

  std::cout << "gain1/delay1 (" << m_digits_gain1_delay1.size() << " elements): ";
  for (int val : m_digits_gain1_delay1) {
    std::cout << " " << val;
  }
  std::cout << std::endl;


  return;
}

ZdcDigits::operator std::string() const
{
  std::cout << "Trying to print ZdcDigits" << std::endl;
  std::ostringstream text(std::ostringstream::out);
  
  text << (std::string) (*(ZdcRawData *)this);
  //std::cout << "IDo=" << identify() << std::endl;

  text << std::setiosflags( std::ios::fixed );
  text << std::setiosflags( std::ios::showpoint );
  text << std::setprecision(2);
  
  ZdcRawData::print_to_stream(m_digits_gain0_delay0," Val:",text);
  text << std::endl;
  ZdcRawData::print_to_stream(m_digits_gain0_delay1," Val:",text);
  text << std::endl;
  ZdcRawData::print_to_stream(m_digits_gain1_delay0," Val:",text);
  text << std::endl;
  ZdcRawData::print_to_stream(m_digits_gain1_delay1," Val:",text);
  text << std::endl;
  
  return text.str();
}

void ZdcDigits::set_digits_gain0_delay0(const std::vector<int>& v)
{
  m_digits_gain0_delay0 = v; // just copy it
}

void ZdcDigits::set_digits_gain0_delay1(const std::vector<int>& v)
{
  m_digits_gain0_delay1 = v; // just copy it
}

void ZdcDigits::set_digits_gain1_delay0(const std::vector<int>& v)
{
  m_digits_gain1_delay0 = v; // just copy it
}

void ZdcDigits::set_digits_gain1_delay1(const std::vector<int>& v)
{
  m_digits_gain1_delay1 = v; // just copy it
}

void ZdcDigits::set_digits_gain0_delay0(const std::vector<uint16_t>& v)
{
  m_digits_gain0_delay0.clear();
  m_digits_gain0_delay0.resize(v.size());
  for (size_t i=0;i<v.size();i++)
    {
      m_digits_gain0_delay0.at(i) = v.at(i); // just copy it
    }
}

void ZdcDigits::set_digits_gain0_delay1(const std::vector<uint16_t>& v)
{
  m_digits_gain0_delay1.clear();
  m_digits_gain0_delay1.resize(v.size());
  for (size_t i=0;i<v.size();i++)
    {
      m_digits_gain0_delay1.at(i) = v.at(i); // just copy it
    }
}

void ZdcDigits::set_digits_gain1_delay0(const std::vector<uint16_t>& v)
{
  m_digits_gain1_delay0.clear();
  m_digits_gain1_delay0.resize(v.size());
  for (size_t i=0;i<v.size();i++)
    {
      m_digits_gain1_delay0.at(i) = v.at(i); // just copy it
    }
}

void ZdcDigits::set_digits_gain1_delay1(const std::vector<uint16_t>& v)
{
  m_digits_gain1_delay1.clear();
  m_digits_gain1_delay1.resize(v.size());
  for (size_t i=0;i<v.size();i++)
    {
      m_digits_gain1_delay1.at(i) = v.at(i); // just copy it
    }
}

