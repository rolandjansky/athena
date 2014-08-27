/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/CTP_RDO.h"

// STL includes:
#include <iostream>
#include <iomanip>
#include <sstream>

// tdaq-common includes for format definition
#include "CTPfragment/CTPdataformat.h"

// local includes
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

CTP_RDO::CTP_RDO(const uint32_t nBCs, uint32_t nExtraWords)
   : m_l1AcceptPosition(0), 
     m_turnCounter(0),
     m_numberOfBunches(nBCs), 
     m_activeBunch(0), 
     m_numberOfAdditionalWords(nExtraWords)
{
   // create correct length, zero filled data member
   m_dataWords.resize(CTPdataformat::NumberTimeWords+(nBCs*CTPdataformat::DAQwordsPerBunch)+nExtraWords);
}

CTP_RDO::CTP_RDO(const std::vector<uint32_t> data, uint32_t nExtraWords)
  : m_dataWords(data), 
    m_l1AcceptPosition(0), 
    m_turnCounter(0),
    m_activeBunch(0)
    
{
   m_numberOfAdditionalWords=nExtraWords;
   if(!m_dataWords.size()) {
      m_numberOfBunches = 0u;
   } else {
      uint32_t dataWords = static_cast<uint32_t>(m_dataWords.size()) - CTPdataformat::NumberTimeWords - nExtraWords;
      m_numberOfBunches = dataWords / CTPdataformat::DAQwordsPerBunch;
   }
}

CTP_RDO::~CTP_RDO(){}

void CTP_RDO::setTimeSec(const uint32_t sec) 
{
   if(CTPdataformat::TimeSecondsPos < m_dataWords.size()) {
      m_dataWords[CTPdataformat::TimeSecondsPos] = sec;
   }
}

void CTP_RDO::setTimeNanoSec(const uint32_t nano)
{
   if(CTPdataformat::TimeNanosecondsPos < m_dataWords.size()) {
      m_dataWords[CTPdataformat::TimeNanosecondsPos] = 
	 (nano/CTPdataformat::TimeNanosecondsTicks) << CTPdataformat::TimeNanosecondsOffset;
   }
}

uint32_t CTP_RDO::getTimeSec() const 
{
   if(m_dataWords.size() <= CTPdataformat::TimeSecondsPos) return 0;  
   return m_dataWords.at(CTPdataformat::TimeSecondsPos);
}

uint32_t CTP_RDO::getTimeNanoSec() const 
{
   if(m_dataWords.size() <= CTPdataformat::TimeNanosecondsPos) return 0;  
   return (m_dataWords.at(CTPdataformat::TimeNanosecondsPos) >> CTPdataformat::TimeNanosecondsOffset) * CTPdataformat::TimeNanosecondsTicks;
}

uint32_t CTP_RDO::getNumberOfBunches() const 
{
   return m_numberOfBunches;
}

uint32_t CTP_RDO::getL1AcceptBunchPosition() const
{
   return m_l1AcceptPosition;
}

uint32_t CTP_RDO::getTimeSinceLastL1A() const
{
  if(getEXTRAWords().size())
    return getEXTRAWords().at(0);
  return -999;
}

uint32_t CTP_RDO::getTurnCounter() const
{
  return m_turnCounter;
}



uint32_t CTP_RDO::getNumberOfAdditionalWords() const 
{
   return m_numberOfAdditionalWords;
}

void CTP_RDO::selectBunch(const uint32_t iBC)
{
   if (iBC < m_numberOfBunches) {
      m_activeBunch = iBC;
   } else {
      m_activeBunch = 0u;
   }
}

void CTP_RDO::selectNextBunch()
{
   ++m_activeBunch;
   m_activeBunch %= m_numberOfBunches;
}

void CTP_RDO::setNumberOfBunches(const uint32_t nBCs)
{
   if (nBCs > m_numberOfBunches) {
      m_numberOfBunches = nBCs;
      m_dataWords.resize(CTPdataformat::NumberTimeWords+(nBCs*CTPdataformat::DAQwordsPerBunch)+m_numberOfAdditionalWords);
   }
}

void CTP_RDO::setL1AcceptBunchPosition(const uint8_t pos)
{
   if (pos < m_numberOfBunches) {
      m_l1AcceptPosition = pos;
   }
}

void CTP_RDO::setTurnCounter(const uint32_t tc)
{
  m_turnCounter=tc;
}

void CTP_RDO::setNumberOfAdditionalWords(const uint32_t nExtraWords)
{
  if(nExtraWords > m_numberOfAdditionalWords) {
    m_dataWords.resize(m_dataWords.size()+nExtraWords-m_numberOfAdditionalWords);//Add the difference
  }
  m_numberOfAdditionalWords = nExtraWords;
}

void CTP_RDO::setPITWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*CTPdataformat::DAQwordsPerBunch) + CTPdataformat::PITpos, word);
}

void CTP_RDO::setTBPWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*CTPdataformat::DAQwordsPerBunch) + CTPdataformat::TBPpos, word);
}

void CTP_RDO::setTAPWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*CTPdataformat::DAQwordsPerBunch) + CTPdataformat::TAPpos, word);
}

void CTP_RDO::setTAVWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*CTPdataformat::DAQwordsPerBunch) + CTPdataformat::TAVpos, word);
}

std::vector<uint32_t> CTP_RDO::getPITWords() const 
{
   return getWords(PIT);
}

std::vector<uint32_t> CTP_RDO::getTBPWords() const 
{
   return getWords(TBP);
}

std::vector<uint32_t> CTP_RDO::getTAPWords() const 
{
   return getWords(TAP);
}

std::vector<uint32_t> CTP_RDO::getTAVWords() const 
{
   return getWords(TAV);
}

std::vector<uint32_t> CTP_RDO::getEXTRAWords() const 
{
  return getWords(EXTRA);
}

void CTP_RDO::setWord(const unsigned int i, const uint32_t word)
{
   if (i < m_dataWords.size()) {
      m_dataWords[i] = word;
   } else {
      std::cout << "WARNING: CTP_RDO: ignoring word " << word << " for position " << i << std::endl;
   }

   return;
}

std::vector<uint32_t> CTP_RDO::getWords(WordType type) const 
{
   unsigned int nWords = 0;
   unsigned int offset = 0;
   std::vector<uint32_t> vec;
   vec.clear();

   switch (type) {
   case PIT:
      nWords = CTPdataformat::PITwords;
      offset = CTPdataformat::PITpos;
      break;
   case TBP:
      nWords = CTPdataformat::TBPwords;
      offset = CTPdataformat::TBPpos;
      break;
   case TAP:
      nWords = CTPdataformat::TAPwords;
      offset = CTPdataformat::TAPpos;
      break;
   case TAV:
      nWords = CTPdataformat::TAVwords;
      offset = CTPdataformat::TAVpos;
      break;
   case EXTRA:
     for (size_t i(m_dataWords.size()-m_numberOfAdditionalWords); i < m_dataWords.size(); ++i) 
       vec.push_back(m_dataWords[i]);
     return vec;
     break;

   default:
      // error
      break;
   }

   for(unsigned int bunch = 0 ; bunch < m_numberOfBunches ; ++bunch) {
      for(unsigned int tbp = 0; tbp < nWords; ++tbp) {
	 // take offset of TBPwords into account
	 unsigned int index = offset + tbp;
	 // go to the correct bunch
	 index += (bunch * CTPdataformat::DAQwordsPerBunch);
	 if( index < m_dataWords.size() ) vec.push_back(m_dataWords[index]);
      }
   }
   // this is now a list of consecutive data words for all bunches
   return vec;
}

const std::string CTP_RDO::dump() const
{
   std::ostringstream s;

   s << getL1AcceptBunchPosition() << " " << LVL1CTP::convertToHex(this->getDataWords());

   return s.str();
}

const std::string CTP_RDO::print(const bool longFormat) const
{
   std::ostringstream s;

   // lvl1 accept position
   s << "\n L1APos:" << std::setw(3) << getL1AcceptBunchPosition();
   if (longFormat) s << std::endl;
   s << "\n Turn counter:" << std::setw(3) << getTurnCounter();
   if (longFormat) s << std::endl;

   // time is only stored once for the whole fragment
   for (size_t i(0); (i < CTPdataformat::NumberTimeWords) && (i < m_dataWords.size()); ++i) {
      if (i == 0 || longFormat) s << "\n Time";
      if (longFormat) s << std::setw(1) << i;
      s << " " << std::setw(8) << m_dataWords[i];
      if (longFormat) s << std::endl;
   }

   // return if data content is too short
   if (m_dataWords.size() < 2) return s.str();


   // loop over the rest of the data fragment
   for (unsigned int k(0); k < (m_dataWords.size()-2)/CTPdataformat::DAQwordsPerBunch; ++k) {

      // print single fragment

      // PIT
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::PITpos);
	   (i < CTPdataformat::PITwords) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n PIT";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

      // TBP
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TBPpos);
	   (i < CTPdataformat::TBPwords) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TBP";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

      // TAP
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TAPpos);
	   (i < CTPdataformat::TAPwords) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TAP";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

      // TAV
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TAVpos);
	   (i < CTPdataformat::TAVwords) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TAV";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

   }

   //EXTRA WORDS
   for (size_t i(m_dataWords.size()-m_numberOfAdditionalWords); i < m_dataWords.size(); ++i) {
     if (i == m_dataWords.size()-m_numberOfAdditionalWords || longFormat ) s << "\n EXTRA";
     if (longFormat) s << std::setw(1) << i;
     s << LVL1CTP::convertToHex(m_dataWords[i]);
     if (longFormat) s << std::endl;
   }
   
   return s.str();
}
