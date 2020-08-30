/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/CTPResult.h"

// gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// STL includes
#include <sstream>
#include <iomanip>

// Local include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"


namespace ROIB {



   CTPResult::CTPResult(unsigned int ctpVersion,  Header&& head, Trailer &&trail, std::vector<CTPRoI>&& rois) :
      m_CTPResultHeader( std::move(head) ),
      m_CTPResultTrailer( std::move(trail) ),
      m_CTPResultRoIVec( std::move(rois) ),
      //m_ctpVersionNumber( ctpVersion ),
      m_ctpDataformat( ctpVersion ),
      m_useRoIB(false),
      m_l1aBunch(0)
   {
      m_useRoIB = (m_CTPResultHeader.sourceID() == 0x770001);
      if (!m_useRoIB) m_l1aBunch = (m_CTPResultHeader.formatVersion() >> m_ctpDataformat.getL1APositionShift()) & m_ctpDataformat.getL1APositionMask();
   }


   CTPResult::CTPResult(unsigned int ctpVersion, Header&& head, Trailer&& trail, const std::vector<uint32_t>& v ) :
      m_CTPResultHeader( std::move(head) ),
      m_CTPResultTrailer( std::move(trail) ),
      //m_ctpVersionNumber( ctpVersion ),
      m_ctpDataformat( ctpVersion ),
      m_useRoIB(false),
      m_l1aBunch(0)
   {
      m_useRoIB = (m_CTPResultHeader.sourceID() == 0x770001);

      std::copy(v.begin(), v.end(), back_inserter(m_CTPResultRoIVec));
      if (!m_useRoIB) m_l1aBunch = (m_CTPResultHeader.formatVersion() >> m_ctpDataformat.getL1APositionShift()) & m_ctpDataformat.getL1APositionMask();
   }


   const std::string CTPResult::dump() const
   {
      std::ostringstream s;

      s << " [" << this->header().dump() << "] ";
      s << " [";
      for (std::vector<CTPRoI>::size_type i(0); i < roIVec().size(); ++i) {
         s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      }
      s << "] ";
      s << " [" << this->trailer().dump() << "] ";
    
      return s.str();
   }

   const std::string CTPResult::print(const bool longFormat) const
   {
      std::ostringstream s;

      s << "header  [" << this->header().print(longFormat) << "] ";
      if (longFormat) s << std::endl;
      s << "data [" << convert(this->roIVec(), longFormat) << "] ";
      if (longFormat) s << std::endl;
      s << "trailer [" << this->trailer().print(longFormat) << "] ";

      return s.str();
   }

   const std::string CTPResult::convert(std::vector<CTPRoI> data, const bool longFormat) const
   {
      std::ostringstream s;

      // time
      for (size_t i(0); (i < m_ctpDataformat.getNumberTimeWords()) && (i < data.size()); ++i) {
         if (longFormat) s << "\n";
         if (i == 0 || longFormat) s << " Time";
         if (longFormat) s << std::setw(1) << i;
         s << " " << std::setw(8) << data[i].roIWord();
         if (longFormat) s << std::endl;
      }

      // TIP
      for (size_t i(0), p(((m_useRoIB) ? m_ctpDataformat.getTIPpos():m_l1aBunch*m_ctpDataformat.getDAQwordsPerBunch())+m_ctpDataformat.getTIPpos());
           (i < ((m_useRoIB) ? m_ctpDataformat.getRoIBTIPwords():m_ctpDataformat.getTIPwords())) && (p < data.size()); 
           ++i, ++p) {
         if (longFormat) s << "\n";
         if (i == 0 || longFormat) s << " TIP";
         if (longFormat) s << std::setw(1) << i;
         s << LVL1CTP::convertToHex(data[p].roIWord());
         if (longFormat) s << std::endl;
      }
    
      // FPI
      // for (size_t i(0), p(m_ctpDataformat.getFPIpos());
      //         (i < m_ctpDataformat.getFPIwords()) && (p < data.size()); 
      //         ++i, ++p) {
      //      if (longFormat) s << "\n";
      //      if (i == 0 || longFormat) s << " FPI";
      //      if (longFormat) s << std::setw(1) << i;
      //      s << LVL1CTP::convertToHex(data[p].roIWord());
      //      if (longFormat) s << std::endl;
      //    }

      // TBP
      for (size_t i(0), p(((m_useRoIB) ? m_ctpDataformat.getRoIBTBPpos():m_l1aBunch*m_ctpDataformat.getDAQwordsPerBunch()+m_ctpDataformat.getTBPpos()));
           (i < m_ctpDataformat.getTBPwords()) && (p < data.size()); 
           ++i, ++p) {
         if (longFormat) s << "\n";
         if (i == 0 || longFormat) s << " TBP";
         if (longFormat) s << std::setw(1) << i;
         s << LVL1CTP::convertToHex(data[p].roIWord());
         if (longFormat) s << std::endl;
      }

      // TAP
      for (size_t i(0), p(((m_useRoIB) ? m_ctpDataformat.getTAPpos():m_l1aBunch*m_ctpDataformat.getDAQwordsPerBunch())+m_ctpDataformat.getTAPpos());
           (i < ((m_useRoIB) ? m_ctpDataformat.getRoIBTAPwords():m_ctpDataformat.getTAPwords())) && (p < data.size()); 
           ++i, ++p) {
         if (longFormat) s << "\n";
         if (i == 0 || longFormat) s << " TAP";
         if (longFormat) s << std::setw(1) << i;
         s << LVL1CTP::convertToHex(data[p].roIWord());
         if (longFormat) s << std::endl;
      }

      // TAV
      for (size_t i(0), p(((m_useRoIB) ? m_ctpDataformat.getRoIBTAVpos():m_l1aBunch*m_ctpDataformat.getDAQwordsPerBunch()+m_ctpDataformat.getTAVpos()));
           (i < m_ctpDataformat.getTAVwords()) && (p < data.size()); 
           ++i, ++p) {
         if (longFormat) s << "\n";
         if (i == 0 || longFormat) s << " TAV";
         if (longFormat) s << std::setw(1) << i;
         s << LVL1CTP::convertToHex(data[p].roIWord());
         if (longFormat) s << std::endl;
      }
    
      return s.str();
   }


   void CTPResult::dumpData() const
   {
      IMessageSvc*  msgSvc;
      ISvcLocator* svcLoc = Gaudi::svcLocator( );
      StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
      if ( sc.isFailure() ) {
         return;
      }
      MsgStream log(msgSvc, "CTPResult");
      dumpData(log);
   }

   void CTPResult::dumpData(MsgStream& log) const
   {
      log << MSG::DEBUG << "*BEGIN* CTPResult" << endmsg;
      m_CTPResultHeader.dumpData(log);
      int counter = 0;
      std::vector< CTPRoI >::const_iterator it = m_CTPResultRoIVec.begin();
      for(; it != m_CTPResultRoIVec.end(); ++it,++counter) {
         log << MSG::DEBUG << "RoI word[" << counter << "]             : 0x"
             << MSG::hex << it->roIWord() << MSG::dec << endmsg;
      }
      m_CTPResultTrailer.dumpData(log);
      log << MSG::DEBUG << "*END* CTPResult" << endmsg;
   }

   // convert vector of ints into bitset
   std::bitset<512> convertToBitset(const std::vector<uint32_t>& words) 
   {
      std::bitset<512> bitset;

      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<512> bs = words[i];
         bs <<= (i * 32);
         bitset |= bs;
      }

      return bitset;
   }

   // convert vector of CTPRoIs into bitset
   std::bitset<512> convertToBitset(const std::vector<CTPRoI>& words) 
   {  
      std::bitset<512> bitset;

      for (size_t i(0); i < words.size(); ++i) {
         std::bitset<512> bs = words[i].roIWord();
         bs <<= (i * 32);
         bitset |= bs;
      }

      return bitset;
   }

} // namespace ROIB
