/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <typeinfo>

#include "LArRawEvent/LArRawSC.h"


// default constructor 
LArRawSC::LArRawSC()  {}

LArRawSC::operator std::string() const{

 //char stNumberOfSamples[30];
 //sprintf( stNumberOfSamples , "# of samples = %d " , this->nsamples() ) ;
 
 
 std::string digitString = typeid( *this ).name(); 
 
 return digitString ;


}

// set method
void LArRawSC::setEnergies(const std::vector<int>& energies)
{
  m_energies = energies;
}

void LArRawSC::setBCIds(const std::vector<unsigned short>& bcids)
{
  m_BCId = bcids;
}
