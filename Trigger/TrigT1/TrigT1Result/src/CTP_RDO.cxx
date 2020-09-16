/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/CTP_RDO.h"

// STL includes:
#include <iostream>
#include <iomanip>
#include <sstream>

// tdaq-common includes for format definition
//#include "CTPfragment/CTPdataformat.h"

// local includes
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

CTP_RDO::CTP_RDO() :
   m_ctpVersionNumber(0),
   m_ctpDataFormat(0),
   m_numberOfBunches(0), 
   m_numberOfAdditionalWords(0)
{}

CTP_RDO::CTP_RDO(unsigned int ctpVersionNumber, const uint32_t nBCs, uint32_t nExtraWords)
   : m_ctpVersionNumber(ctpVersionNumber),
     m_ctpDataFormat(ctpVersionNumber),
     m_numberOfBunches(nBCs), 
     m_numberOfAdditionalWords(nExtraWords)
{
    
   // create correct length, zero filled data member
   m_dataWords.resize(m_ctpDataFormat.getNumberTimeWords()+(nBCs*m_ctpDataFormat.getDAQwordsPerBunch() )+nExtraWords);
}

CTP_RDO::CTP_RDO(unsigned int ctpVersionNumber, std::vector<uint32_t>&& data, uint32_t nExtraWords)
  : m_ctpVersionNumber(ctpVersionNumber),
    m_ctpDataFormat(ctpVersionNumber),
    m_dataWords(std::move(data))    
{
    
   m_numberOfAdditionalWords=nExtraWords;
   if(!m_dataWords.size()) {
      m_numberOfBunches = 0u;
   } else {
      uint32_t dataWords = static_cast<uint32_t>(m_dataWords.size()) - m_ctpDataFormat.getNumberTimeWords() - nExtraWords;
      m_numberOfBunches = dataWords / m_ctpDataFormat.getDAQwordsPerBunch();
   }
}

CTP_RDO::~CTP_RDO(){}

const CTPdataformatVersion &
CTP_RDO::getCTPVersion() const { 
   if(m_ctpVersionNumber==0) {                                          \
      std::cout << "CTP_RDO              WARNING CTPVersion has not been set, no information about data format available, please fix your code" << std::endl;
   }
   return m_ctpDataFormat;
}


void
CTP_RDO::setCTPVersionNumber( unsigned int ctpVersion ) {
   m_ctpVersionNumber = ctpVersion;
}

void CTP_RDO::setTimeSec(const uint32_t sec) 
{
   if(getCTPVersion().getTimeSecondsPos() < m_dataWords.size()) {
      m_dataWords[getCTPVersion().getTimeSecondsPos()] = sec;
   }
}

void CTP_RDO::setTimeNanoSec(const uint32_t nano)
{
   if(getCTPVersion().getTimeNanosecondsPos() < m_dataWords.size()) {
      m_dataWords[getCTPVersion().getTimeNanosecondsPos()] = 
	 (nano/getCTPVersion().getTimeNanosecondsTicks()) << getCTPVersion().getTimeNanosecondsOffset();
   }
}

uint32_t CTP_RDO::getTimeSec() const 
{
   if(m_dataWords.size() <= getCTPVersion().getTimeSecondsPos()) return 0;  
   return m_dataWords.at(getCTPVersion().getTimeSecondsPos() );
}

uint32_t CTP_RDO::getTimeNanoSec() const 
{
   if(m_dataWords.size() <= getCTPVersion().getTimeNanosecondsPos()) return 0;  
   return (m_dataWords.at(getCTPVersion().getTimeNanosecondsPos() ) >> getCTPVersion().getTimeNanosecondsOffset() ) * getCTPVersion().getTimeNanosecondsTicks();
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
      m_dataWords.resize(getCTPVersion().getNumberTimeWords()+(nBCs*getCTPVersion().getDAQwordsPerBunch() )+m_numberOfAdditionalWords);
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

//void CTP_RDO::setPITWord(const unsigned int i, const uint32_t word)
//{
//   setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch())  + getCTPVersion().getPITpos(), word);
//}

//void CTP_RDO::setFPIWord(const unsigned int i, const uint32_t word)
//{
//    setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch())  + getCTPVersion().getFPIpos(), word);
//}

void CTP_RDO::setTIPWord(const unsigned int i, const uint32_t word)
{
    setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch())  + getCTPVersion().getTIPpos(), word);
}

void CTP_RDO::setTBPWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch() )  + getCTPVersion().getTBPpos(), word);
}

void CTP_RDO::setTAPWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch() )  + getCTPVersion().getTAPpos(), word);
}

void CTP_RDO::setTAVWord(const unsigned int i, const uint32_t word)
{
   setWord(i + (m_activeBunch*getCTPVersion().getDAQwordsPerBunch())  + getCTPVersion().getTAVpos(), word);
}

//std::vector<uint32_t> CTP_RDO::getPITWords() const 
//{
//   return getWords(PIT);
//}
//
//std::vector<uint32_t> CTP_RDO::getFPIWords() const 
//{
//    return getWords(FPI);
//}

std::vector<uint32_t> CTP_RDO::getTIPWords() const 
{
    return getWords(TIP);
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
   //case PIT:
//      nWords = getCTPVersion().getPITwords();
//      offset = getCTPVersion().getPITpos();
//      break;
//   case FPI:
//      nWords = getCTPVersion().getFPIwords();
//      offset = getCTPVersion().getFPIpos();
//      break;
   case TIP:
      nWords = getCTPVersion().getTIPwords();
      offset = getCTPVersion().getTIPpos();
      break;
   case TBP:
      nWords = getCTPVersion().getTBPwords();
      offset = getCTPVersion().getTBPpos();
      break;
   case TAP:
      nWords = getCTPVersion().getTAPwords();
      offset = getCTPVersion().getTAPpos();
      break;
   case TAV:
      nWords = getCTPVersion().getTAVwords();
      offset = getCTPVersion().getTAVpos();
      break;
   case EXTRA:
     vec.assign(m_dataWords.begin() + (m_dataWords.size()-m_numberOfAdditionalWords), m_dataWords.end());
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
          index += (bunch * getCTPVersion().getDAQwordsPerBunch());
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
   for (size_t i(0); (i < getCTPVersion().getNumberTimeWords() ) && (i < m_dataWords.size()); ++i) {
      if (i == 0 || longFormat) s << "\n Time";
      if (longFormat) s << std::setw(1) << i;
      s << " " << std::setw(8) << m_dataWords[i];
      if (longFormat) s << std::endl;
   }

   // return if data content is too short
   if (m_dataWords.size() < 2) return s.str();


   // loop over the rest of the data fragment
   for (unsigned int k(0); k < (m_dataWords.size()-2)/getCTPVersion().getDAQwordsPerBunch(); ++k) {

      // print single fragment

      // TIP
      for (size_t i(0), p(k*getCTPVersion().getDAQwordsPerBunch() + getCTPVersion().getTIPpos());
	   (i < getCTPVersion().getTIPwords()) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TIP";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }
       

      // TBP
      for (size_t i(0), p(k*getCTPVersion().getDAQwordsPerBunch() + getCTPVersion().getTBPpos());
	   (i < getCTPVersion().getTBPwords()) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TBP";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

      // TAP
      for (size_t i(0), p(k*getCTPVersion().getDAQwordsPerBunch() + getCTPVersion().getTAPpos());
	   (i < getCTPVersion().getTAPwords()) && (p < m_dataWords.size()); 
	   ++i, ++p) {
	 if (i == 0 || longFormat) s << "\n TAP";
	 if (longFormat) s << std::setw(1) << i;
	 s << LVL1CTP::convertToHex(m_dataWords[p]);
	 if (longFormat) s << std::endl;
      }

      // TAV
      for (size_t i(0), p(k*getCTPVersion().getDAQwordsPerBunch() + getCTPVersion().getTAVpos() );
	   (i < getCTPVersion().getTAVwords()) && (p < m_dataWords.size()); 
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
