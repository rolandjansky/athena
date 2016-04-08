/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArTBEvent/LArTBCalib.h"
LArTBCalib::LArTBCalib(int boardNumber, int nbPattern, int iPattern, int nbTrigger, 
	   int iTrigger, int nbDAC, int iDAC, int nbDelay, int specDelay, 
	     std::vector < int > DACValues, std::vector < std::vector < int > > 
	   delayValues, std::vector < std::vector < int > > patternValues) :
  m_DACValues(DACValues), m_delayValues(delayValues),
    m_patternValues(patternValues)
{
   m_boardNumber = boardNumber;
   m_nbPattern = nbPattern;
   m_iPattern = iPattern;
   m_nbTrigger = nbTrigger;
   m_iTrigger = iTrigger;
   m_nbDAC = nbDAC;
   m_iDAC = iDAC;
   m_nbDelay = nbDelay;
   m_specDelay = specDelay;
}

LArTBCalib::~LArTBCalib() { }

LArTBCalib::operator std::string() const
{
  std::string error_message = "Unable to get LArTBCalib string" ;
  return error_message ;
}
