/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1RESULT_CTPRESULT_H
#define TRIGT1RESULT_CTPRESULT_H

// std include(s):
#include <stdint.h>

// STL include(s):
#include <vector>
#include <bitset>
#include <string>

// Local include(s):
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "TrigT1Result/CTPRoI.h"

#include "CTPfragment/CTPdataformatVersion.h"

// Forward declaration(s):
class MsgStream;

namespace ROIB {

  /**
   * 
   *   @short Class holding the LVL1 CTP result used by the RoIBuilder
   *
   *          CTPResult holds the LVL1 CTP information (one bunch 
   *          crossing only)send from the CTP hardware to the RoIBuilder. 
   *          This is the trigger result for each item before prescale, 
   *          after prescale and after veto. 
   *
   *          CTPResult is the CTP part of the L1 RDO (ROIB::RoIBResult).
   *
   *     @see ROIB::Header
   *     @see ROIB::Trailer
   *     @see ROIB::CTPRoI
   *     @see ROIB::RoIBResult
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *  @author Wolfgang Ehrenfeld <wolfgang.ehrenfeld@cern.ch>
   *
   * @version \$Id: CTPResult.h,v 1.12 2009-01-06 21:46:55 ssnyder Exp $
   *
   */

  class CTPResult {

  public:
     //! default constructor: empty object
     CTPResult() : m_ctpDataformat(0),
      m_useRoIB(false),
      m_l1aBunch(0) {}

     //! full constructor with CTP version, header, trailer and vector of data
     CTPResult(unsigned int ctpVersion,  Header&&, Trailer&&, std::vector<CTPRoI>&& );

     //! full constructor with CTP version, header, trailer and vector of data
     CTPResult(unsigned int ctpVersion, Header&&, Trailer&&, const std::vector<uint32_t>& );

     /*
      * Methods: 
      *
      * */

     /* status information */

     //! returns true if header and trailer exist, data can be empty
     bool isValid() const;
     //! returns true if object isValid() and data has the expected length
     bool isComplete() const;

     /* raw data access */

     //! return header
     const Header& header() const;
     //! return trailer
     const Trailer& trailer() const;
     //! return the RoI vector */
     const std::vector<CTPRoI>& roIVec() const;

     /* access the trigger information */

     //! get trigger result before prescale
     const std::vector<CTPRoI> TBP() const;
     //! get trigger result after prescale
     const std::vector<CTPRoI> TAP() const;
     //! get trigger result after veto
     const std::vector<CTPRoI> TAV() const;
     //! get trigger accept before prescale
     bool acceptBP() const;
     //! get trigger accept after prescale
     bool acceptAP() const;
     //! get trigger accept after veto
     bool acceptAV() const;
     //! get standard trigger accept
     bool accept() const { return acceptAV(); }

     /* debug object content */

     //! dump raw object content to string
     /** method used for read/write testing and T/P separation */
     const std::string dump() const;    
     //! print object content in a human readable format to string
     const std::string print(const bool longFormat = false) const;

     //! print object content to default message stream
     void dumpData() const;
     //! print object content to given message stream
     void dumpData(MsgStream&) const;

  private:

     /* Data members = header, trailer and RoI vector */
     Header m_CTPResultHeader;                             //!< header fragment in LVL1 eformat
     Trailer m_CTPResultTrailer;                           //!< trailer fragment in LVL1 eformat
     std::vector<CTPRoI> m_CTPResultRoIVec;                //!< raw data content (RoIs)

     //CTP version
     //unsigned int m_ctpVersionNumber;
     CTPdataformatVersion m_ctpDataformat;
    
     //! convert data content into string (used by dump and print)
     const std::string convert(std::vector<CTPRoI> data, const bool longFormat = false) const;

     // flag to tell if this is a RoIB result or a DAQ result
     bool m_useRoIB;

     // the L1 accept bunch for the DAQ CTP result
     unsigned int m_l1aBunch;

  }; // class CTPResult

  // converter functions

  //! convert vector of unsigned int into bitset
  std::bitset<512> convertToBitset(const std::vector<uint32_t>&);
  //! convert vector of ROIB::CTPRoI into bitset
  std::bitset<512> convertToBitset(const std::vector<CTPRoI>&);

} // namespace ROIB

// include inline implementations

#include "TrigT1Result/CTPResult.icc"

#endif // TRIGT1RESULT_CTPRESULT_H
