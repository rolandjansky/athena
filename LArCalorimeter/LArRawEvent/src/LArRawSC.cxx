/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <typeinfo>

#include "LArRawEvent/LArRawSC.h"


// default constructor 
LArRawSC::LArRawSC()  {}

LArRawSC::operator std::string() const{

 char * stSamples = new char[20] ;
 char * stNumberOfSamples = new char[30] ;
 char * classNameOfDigit = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , classNameOfDigit ) ;
 
 
 sprintf( stNumberOfSamples , "# of samples = %d " , this->nsamples() ) ;
 
 
 std::string digitString = classNameOfDigit ;
 delete[] stSamples ;
 delete[] classNameOfDigit ;
 delete[] stNumberOfSamples ;
 
 return digitString ;


}

// set method
void LArRawSC::setEnergies(std::vector<int> energies)
{
  m_energies.clear();
  m_energies = energies;
}

void LArRawSC::setBCIds(std::vector<unsigned short> bcids)
{
  m_BCId.clear();
  m_BCId = bcids;
}
