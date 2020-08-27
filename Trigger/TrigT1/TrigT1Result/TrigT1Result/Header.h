/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1RESULT_HEADER_H
#define TRIGT1RESULT_HEADER_H

// std include(s):
#include <stdint.h>

// STL include(s):
#include <string>
#include <vector>

// Forward declaration(s):
class MsgStream;

namespace ROIB {

  /**
   * 
   *   @short Header models the LVL1 ROD Header
   *
   *          Header is a class equivalent to the ROD header 
   *          from the eformat lib, just much simpler and adapted 
   *          to the LVL1 needs.
   *
   *     @see ROIB::Header
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *
   * @version \$Id: Header.h,v 1.11 2008-12-18 13:35:00 dquarrie Exp $
   *
   */

  class Header {

  public:
    // constants:
    /** a ROD header as used in the LVL1 has 8 words, not counting the one
       hardware word in the beginning */
    static constexpr unsigned int wordsPerHeader = 8;             //!< number of words in header (Ref: ATL-DA-ES-0029)

    //! default constructor: empty object 
    Header()
      : m_header() {}

    //! constructor which fully specifies the initialiser values 
    Header( std::vector<uint32_t>&& v)
      : m_header( std::move(v) ) {}

    Header(Header&&) noexcept = default;
    Header(const Header&) = default;
    Header& operator=(Header&&) noexcept =default;
    Header& operator=(const Header&) =default;

    //! constructor specifying subdet ID, event ID and version 
    Header( const uint32_t, const uint32_t, const uint32_t );
    //! constructor specifying subdet ID, event ID 
    Header( const uint32_t, const uint32_t );
    //! constructor specifying subdet ID only 
    Header( const uint32_t );


    //! empty default desctructor 
    ~Header() = default;

    //! get full header 
    const std::vector<uint32_t>& header() const;
    //! get header(data) size 
    int size() const;

    /* 'get' methods for the various entries in the header */

    //! get header marker word 
    uint32_t headerMarker() const;
    //! get number of words in header 
    uint32_t headerSize() const;
    //! get version of header format 
    uint32_t formatVersion() const;
    //! get sub detector source ID 
    uint32_t sourceID() const;
    //! get run number 
    uint32_t runNumber() const;
    //! get extended LVL1 ID 
    uint32_t L1ID() const;
    //! get bunch crossing ID 
    uint32_t BCID() const;
    //! get LVL1 trigger type 
    uint32_t triggerType() const;
    //! get LVL1 event type 
    uint32_t eventType() const;

    /* Method setting various properties */

    //! set run number 
    void setRunNumber( const uint32_t );
    //! set extended LVL1 ID 
    void setL1ID( const uint32_t );
    //! set bunch crossing ID 
    void setBCID( const uint32_t i );
    //! set LVL1 trigger type 
    void setTriggerType( const uint32_t i );
    //! set LVL1 event type 
    void setEventType( const uint32_t i );

    //! dump raw object content to string
    const std::string dump() const;
    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

    //! print object content to default message stream 
    void dumpData() const;
    //! print object content to given message stream 
    void dumpData(MsgStream&) const;

  private:
    /** vector of unsigned ints - the only data member */
    std::vector< uint32_t > m_header;

  }; // class Header

} // namespace ROIB

// inline implementations

#include "TrigT1Result/Header.icc"

#endif // TRIGT1RESULT_HEADER_H
