/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/CTP_Decoder.h"

// tdaq-common includes for format definition
#include "CTPfragment/CTPdataformat.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

CTP_Decoder::CTP_Decoder() :
  AthMessaging(Athena::getMessageSvc(), "CTP_Decoder")
{
}

void CTP_Decoder::setRDO(const CTP_RDO* rdo)
{
   m_rdo = rdo;
   unsigned int ctpVersionNumber = m_rdo->getCTPVersionNumber();
   CTPdataformatVersion ctpVersion(ctpVersionNumber);
  
   unsigned int nBunches = rdo->getNumberOfBunches();
   ATH_MSG_DEBUG("setRDO> #bunches: " << nBunches << ", ctp version (found in RDO): " << ctpVersionNumber);
   ATH_MSG_VERBOSE(ctpVersion.dump());

   m_BCs.clear();
   m_BCs.resize(nBunches);
   for (std::vector<CTP_BC>::iterator bcIter=m_BCs.begin(); bcIter!=m_BCs.end(); bcIter++) {
      bcIter->setCTPVersion(ctpVersionNumber);
   }
  
   //std::vector<uint32_t> PITs = rdo->getPITWords();
   //std::vector<uint32_t> FPIs = rdo->getFPIWords();
   const std::vector<uint32_t> & TIPs = rdo->getTIPWords();
   const std::vector<uint32_t> & TBPs = rdo->getTBPWords();
   const std::vector<uint32_t> & TAPs = rdo->getTAPWords();
   const std::vector<uint32_t> & TAVs = rdo->getTAVWords();
  
   if( nBunches * ctpVersion.getTIPwords() != TIPs.size() ) {
     ATH_MSG_FATAL("Expected " << nBunches * ctpVersion.getTIPwords() << " TIP words, but TIP vector has size " << TIPs.size());
   } else {
     ATH_MSG_DEBUG(TIPs.size() << " TIP words as expected");
   }
   if( nBunches * ctpVersion.getTBPwords() != TBPs.size() ) {
     ATH_MSG_FATAL(nBunches * ctpVersion.getTBPwords() << " TBP words, but TBP vector has size " << TBPs.size());
   } else {
     ATH_MSG_DEBUG(TBPs.size() << " TBP words as expected");
   }
   if( nBunches * ctpVersion.getTAPwords() != TAPs.size() ) {
     ATH_MSG_FATAL(nBunches * ctpVersion.getTAPwords() << " TAP words, but TAP vector has size " << TAPs.size());
   } else {
     ATH_MSG_DEBUG(TAPs.size() << " TAP words as expected");
   }
   if( nBunches * ctpVersion.getTAVwords() != TAVs.size() ) {
     ATH_MSG_FATAL(nBunches * ctpVersion.getTAVwords() << " TAV words, but TAV vector has size " << TAVs.size());
   } else {
     ATH_MSG_DEBUG(TAVs.size() << " TAV words as expected");
   }
  

   for(unsigned int i = 0 ; i < nBunches ; ++i) {

      ATH_MSG_DEBUG(i << " from CTP_RDO to internal bitsets");

      for(unsigned int tip = 0; tip < ctpVersion.getTIPwords(); ++tip) {
         unsigned int index = i*ctpVersion.getTIPwords() + tip;
         if(index >= TIPs.size()) {
            ATH_MSG_FATAL("Invalid TIP position " << index);
            return;
         }
         m_BCs[i].setTIPWord(TIPs[index],tip); //sets m_tip
      }
    
      for(unsigned int tbp = 0; tbp < ctpVersion.getTBPwords(); ++tbp) {
         unsigned int index = i * ctpVersion.getTBPwords() + tbp;
         if(index >= TBPs.size()) {
            ATH_MSG_FATAL("Invalid TBP position " << index);
            return;
         }
         m_BCs[i].setTBPWord(TBPs[index],tbp); //sets m_tbp
      }
    
      for(unsigned int tap = 0; tap < ctpVersion.getTAPwords(); ++tap) {
         unsigned int index = i * ctpVersion.getTAPwords() + tap;
         if(index >= TAPs.size()) {
            ATH_MSG_FATAL("Invalid TAP position " << index);
            return;
         }
         m_BCs[i].setTAPWord(TAPs[index],tap); //sets m_tap
      }
    
      for(unsigned int tav = 0; tav < ctpVersion.getTAVwords(); ++tav) {
         unsigned int index = i * ctpVersion.getTAVwords() + tav;
         if(index >= TAVs.size()) {
            ATH_MSG_FATAL("Invalid TAV position " << index);
            return;
         }
         m_BCs[i].setTAVWord(TAVs[index],tav); //set m_tav
      }
   }
}

void CTP_Decoder::dumpData() const
{
   unsigned int nBunches = m_rdo->getNumberOfBunches();
   unsigned int vecSize = m_BCs.size();
  
   if(nBunches == 0) {
      ATH_MSG_DEBUG("CTP_RDO empty");
      return;
   }
  
   if(nBunches != vecSize) {
     ATH_MSG_ERROR("mismatch: " << nBunches << " bunches, but vector size is " << vecSize);
   }
  
   if( msgLvl(MSG::DEBUG) ){
      msg() << MSG::DEBUG << "=================================================" << endmsg;
      msg() << MSG::DEBUG << "Event dump" << endmsg;
      msg() << MSG::DEBUG << "Time " << m_rdo->getTimeSec() << "s "
            << std::setw(10) << std::setiosflags(std::ios_base::right) << std::setfill(' ')
            << m_rdo->getTimeNanoSec() << std::resetiosflags(std::ios_base::right)
            << "ns" << endmsg;
      msg() << MSG::DEBUG << "Number of bunches: " <<  nBunches
            << " (BC vector size " << vecSize << ")" << endmsg;
      msg() << MSG::DEBUG << "L1A position: " << m_rdo->getL1AcceptBunchPosition() << endmsg;
   }
   
   for(unsigned int i = 0; i<vecSize; ++i) {
      ATH_MSG_DEBUG("Now dumping BC " << i);
      m_BCs[i].dumpData(msg());
   }

   ATH_MSG_DEBUG("=================================================");
}



bool CTP_Decoder::checkTrigger(unsigned int itemNo,unsigned int pos)
{
  if(pos >= m_BCs.size()) {
    ATH_MSG_WARNING("Trying to access bunch crossing no "
                    << pos << ", but in the event are only " << m_BCs.size());
  }
  if(itemNo >= getBunchCrossing(pos).getTAV().size()) {
    ATH_MSG_WARNING("Checking item no " << itemNo
                    << ", which is more than the maximum : "
                    << getBunchCrossing(pos).getTAV().size());
  }
  return getBunchCrossing(pos).getTAV().test(itemNo);
}

bool CTP_Decoder::checkTriggerAfterPrescale(unsigned int itemNo,unsigned int pos) {
   if(pos >= m_BCs.size()) {
     ATH_MSG_WARNING("Trying to access bunch crossing no "
                     << pos << ", but in the event are only " << m_BCs.size());
   }
   if(itemNo >= getBunchCrossing(pos).getTAP().size()) {
     ATH_MSG_WARNING("Checking item no " << itemNo
                     << ", which is more than the maximum : "
                     << getBunchCrossing(pos).getTAP().size());
   }
   return getBunchCrossing(pos).getTAP().test(itemNo);
}



std::vector<unsigned int> CTP_Decoder::getAllTriggers(unsigned int pos) {
   if(pos >= m_BCs.size()) {
      ATH_MSG_WARNING("Trying to access bunch crossing no "
                      << pos << ", but in the event are only " << m_BCs.size());
      return std::vector<unsigned int>();
   }
   std::vector<unsigned int> triggers;
   const CTP_BC& bc = getBunchCrossing(pos);
   if(bc.getTAV().any()) {
      for(unsigned int i = 0; i < bc.getTAV().size() ; ++i) {
         if(bc.getTAV().test(i)) {
            triggers.push_back(i+1);
         }
      }  
   }
   return triggers;
}




/**
 *
 *    class CTP BC
 *
 */

void CTP_BC::dumpData(MsgStream& msglog) const
{
   if( msglog.level() > MSG::DEBUG ) 
      return;

   msglog << MSG::DEBUG << "-------------- BC dump for " << getBCID() << " ------------------------------------" << endmsg;
   msglog << MSG::DEBUG << "PITWordAux      : " << printPITWordAux() << endmsg;
   msglog << MSG::DEBUG << "BCID            : " << getBCID() << endmsg;
   msglog << MSG::DEBUG << "Random trig     : " << getRandomTrig() << " (binary: " << printRandomTrig() << ")" << endmsg;
   msglog << MSG::DEBUG << "Prescaled clock : " << getPrescaledClock() << " (binary: " << printPrescaledClock() << ")" << endmsg;

   if(m_tip.any()) {
      int count(0);
      std::ostringstream outstream;
      for(unsigned int i = 0; i<m_tip.size() ; ++i) {
         if(m_tip.test(i)) {
            outstream << std::setw(3) << std::setfill('0') << (i+1) << " ";
            ++count;
         }
      }
      msglog << MSG::DEBUG << "TIP with input (" << count << " items): " << outstream.str() << endmsg;
   } else {
      msglog << MSG::DEBUG << "No TIP!" << endmsg;
   }


   if(m_tbp.any()) {
      int count(0);
      std::ostringstream outstream;
      for(unsigned int i = 0; i<m_tbp.size() ; ++i) {
         if(m_tbp.test(i)) {
            outstream << i << " ";
            ++count;
         }
      }
      msglog << MSG::DEBUG << "Fired TBP (" << count << " items): " << outstream.str() << endmsg;
   } else {
      msglog << MSG::DEBUG << "No TBP fired!" << endmsg;
   }

  
   if(m_tap.any()) {
      int count(0);
      std::ostringstream outstream;
      for(unsigned int i = 0; i<m_tap.size() ; ++i) {
         if(m_tap.test(i)) {
            outstream << i << " ";
            ++count;
         }
      }
      msglog << MSG::DEBUG << "Fired TAP (" << count << " items): " << outstream.str() << endmsg;
   } else {
      msglog << MSG::DEBUG << "No TAP fired!" << endmsg;
   }
   

   if(m_tav.any()) {
      int count(0);
      std::ostringstream outstream;
      for(unsigned int i = 0; i<m_tav.size() ; ++i) {
         if(m_tav.test(i)) {
            outstream << i << " ";
            ++count;
         }
      }
      msglog << MSG::DEBUG << "Fired TAV (" << count << " items): " << outstream.str() << endmsg;
   } else {
      msglog << MSG::DEBUG << "No TAV fired!" << endmsg;
   }

   msglog << MSG::VERBOSE << "TIP - total size: " << m_tip.size() << ", with input: " 
          << m_tip.count() << ", pattern:" << std::endl << printTIP() << endmsg;
   msglog << MSG::VERBOSE << "TBP " << std::endl << printTBP() << endmsg;
   msglog << MSG::VERBOSE << "TAP " << std::endl << printTAP() << endmsg;
   msglog << MSG::VERBOSE << "TAV " << std::endl << printTAV() << endmsg;
}


std::bitset<32> CTP_BC::getBCIDBitSet() const
{
   std::bitset<32> bcid = (m_pitAux >> m_ctpVersion.getBcidShift());

   // the bcid mask is wrong in CTPfragment/CTPdataformatVersion.h (0xF000) 
   // so we set it here
   uint32_t bcidMask = 0xFFF;
   if(m_ctpVersion.getVersionNumber()>=1 && m_ctpVersion.getVersionNumber()<=3) {
      bcidMask = 0xF;
   }
   //bcid &= m_ctpVersion.getBcidMask();
   bcid &= bcidMask;
   return bcid;
}

uint32_t CTP_BC::getBCID() const
{
   return getBCIDBitSet().to_ulong();
}

std::string CTP_BC::printBCID() const
{
   std::bitset<32> bcid = getBCIDBitSet();
   return bcid.to_string<char, 
      std::char_traits<char>, std::allocator<char> >();
}

std::bitset<32> CTP_BC::getRandomTrigBitSet() const
{
   std::bitset<32> rnd = (m_pitAux >> m_ctpVersion.getRandomTrigShift());
   rnd &= m_ctpVersion.getRandomTrigMask();
   return rnd;
}

uint32_t CTP_BC::getRandomTrig() const
{
   return getRandomTrigBitSet().to_ulong();
}

std::string CTP_BC::printRandomTrig() const
{
   std::bitset<32> rnd = getRandomTrigBitSet();
   return rnd.to_string<char, 
      std::char_traits<char>, std::allocator<char> >();
}

std::bitset<32> CTP_BC::getPrescaledClockBitSet() const
{
   std::bitset<32> prcl;
	
   if (!m_ctpVersion.getNumPrescaledClocks()) {
      return prcl;
   }
	
   prcl = (m_pitAux >> m_ctpVersion.getPrescaledClockShift());
   prcl &= m_ctpVersion.getPrescaledClockMask();
   return prcl;
}

uint32_t CTP_BC::getPrescaledClock() const
{
   return getPrescaledClockBitSet().to_ulong();
}

std::string CTP_BC::printPrescaledClock() const
{
   std::bitset<32> prcl = getPrescaledClockBitSet();
   return prcl.to_string<char, 
      std::char_traits<char>, std::allocator<char> >();
}

std::string CTP_BC::printPITWordAux() const
{
   return m_pitAux.to_string<char, 
      std::char_traits<char>, std::allocator<char> >();
}

void CTP_BC::setTIPWord( uint32_t word, uint32_t pos)
{
   if(pos >= m_ctpVersion.getTIPwords()) {
      MsgStream log(Athena::getMessageSvc(), "CTP_Decoder");
      log << MSG::ERROR <<"Invalid TIP position " << pos <<endmsg;
      return;
   }

   std::bitset<512> bs = word;
  
   bs <<= (pos * CTP_RDO::SIZEOF_WORDS);
  
   if( pos < (m_ctpVersion.getTIPwords()-1) ) m_tip |= bs;
   else setPITWordAux(word);
}

void CTP_BC::setTIP(std::vector<uint32_t> words)
{
  for(uint32_t i = 0; i<words.size();++i) setTIPWord(words[i],i);
}

std::string CTP_BC::printTIP() const
{
  return m_tip.to_string<char, 
  std::char_traits<char>, std::allocator<char> >();
}



void CTP_BC::setTBPWord( uint32_t word, uint32_t pos) 
{
  if(pos >= m_ctpVersion.getTBPwords()) {
    MsgStream log(Athena::getMessageSvc(), "CTP_Decoder");
    log << MSG::ERROR <<"Invalid TBP position " << pos <<endmsg;
    return;
  }

  std::bitset<512> bs = word;
  bs <<= (pos * CTP_RDO::SIZEOF_WORDS);
  m_tbp |= bs;
}

void CTP_BC::setTBP(std::vector<uint32_t> words)
{
  for(uint32_t i = 0; i<words.size();++i) setTBPWord(words[i],i);
}

std::string CTP_BC::printTBP() const
{
  return m_tbp.to_string<char, 
  std::char_traits<char>, std::allocator<char> >();
}

void CTP_BC::setTAPWord( uint32_t word, uint32_t pos) 
{
  if(pos >= m_ctpVersion.getTAPwords()) {
    MsgStream log(Athena::getMessageSvc(), "CTP_Decoder");
    log << MSG::ERROR <<"Invalid TAP position " << pos <<endmsg;
    return;
  }
  std::bitset<512> bs = word;
  bs <<= (pos * CTP_RDO::SIZEOF_WORDS);
  m_tap |= bs;
}

void CTP_BC::setTAP(std::vector<uint32_t> words)
{
  for(uint32_t i = 0; i<words.size();++i)
    setTAPWord(words[i],i);
}

std::string CTP_BC::printTAP() const
{
  return m_tap.to_string<char, 
  std::char_traits<char>, std::allocator<char> >();
}

void CTP_BC::setTAVWord( uint32_t word, uint32_t pos) 
{
  if(pos >= m_ctpVersion.getTAVwords()) {
    MsgStream log(Athena::getMessageSvc(), "CTP_Decoder");
    log << MSG::ERROR <<"Invalid TAV position " << pos <<endmsg;
    return;
  }
  std::bitset<512> bs = word;
  bs <<= (pos * CTP_RDO::SIZEOF_WORDS);
  m_tav |= bs;
}

void CTP_BC::setTAV(std::vector<uint32_t> words)
{
  for(uint32_t i = 0; i<words.size();++i)
    setTAVWord(words[i],i);
}

std::string CTP_BC::printTAV() const
{
  return m_tav.to_string<char, 
  std::char_traits<char>, std::allocator<char> >();
}
