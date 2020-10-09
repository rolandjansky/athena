/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1RESULT_TRAILER_H
#define TRIGT1RESULT_TRAILER_H

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
   *   @short ROIB::Trailer models the LVL1 ROD Trailer
   *
   *          Trailer is a class equivalent to the ROD trailer 
   *          from the eformat lib, just much simpler and adapted 
   *          to the LVL1 needs.
   *
   *     @see ROIB::Header
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *
   * @version \$Id: Trailer.h,v 1.10 2008-12-18 13:35:05 dquarrie Exp $
   *
   */

  class Trailer {

  public:
    // constants:
    /** a ROD trailer as used in the LVL1 has 5 words, not counting the one
       hardware word in the end */
    static constexpr unsigned int wordsPerTrailer = 5;	    //!< number of words in trailer (Ref: ATL-DA-ES-0029.)
    
    //! default constructor: empty object
    Trailer()
      : m_trailer() {}

    Trailer(Trailer&&) noexcept = default;
    Trailer(const Trailer&) = default;
    Trailer& operator=(Trailer&&) noexcept =default;
    Trailer& operator=(const Trailer&) =default;

    //! constructor which fully specifies the initialiser values
    Trailer( std::vector< uint32_t > &&v)
      : m_trailer( std::move(v) ) {}

    //! constructor specifying the number of data words and the error status
    Trailer( const uint32_t, const uint32_t );
    //! constructor specifying the number of data words
    Trailer( const uint32_t );

    //! empty default destructor
    ~Trailer() = default;


    //! get full trailer
    const std::vector<uint32_t>& trailer() const;

    //! get trailer/data size (should always return 5)
    int size() const;

    /* 'get' methods for the various entries in the trailer */

    //! get error status word (LVL1 trailer assumes only on word)
    uint32_t errorStatus() const;
    //! get info status word (LVL1 trailer assumes only on word)
    uint32_t statusInfo() const;
    //! get number of status words in trailer
    uint32_t numStatusWords() const;
    //! get number of data words
    uint32_t numDataWords() const;
    //! get position of status information in ROD (LVL1 assumes 1)
    uint32_t statusPosition() const;

    /* 'set' methods for the various entries in the trailer */
    
    //! set error status to 0xffff0000 for ROD was not found in SG
    void setSimulationError();
    //! set number of data words
    void setNumDataWords( const unsigned int );

    //! dump raw object content to string
    const std::string dump() const;
    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

    //! print object content to default message stream
    void dumpData() const;
    //! print object content to given message stream
    void dumpData(MsgStream&) const;

  private:
    /** vector of unsigned ints - the only data member - hopefully 5 */
    std::vector< uint32_t > m_trailer;

  }; // class Trailer

} // namespace ROIB

// include inline implementations
#include "TrigT1Result/Trailer.icc"

#endif // TRIGT1RESULT_TRAILER_H
