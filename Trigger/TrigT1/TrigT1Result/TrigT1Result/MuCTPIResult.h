// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_MUCTPIRESULT_H
#define TRIGT1RESULT_MUCTPIRESULT_H

// STL include(s):
#include <vector>

// Local include(s):
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "TrigT1Result/MuCTPIRoI.h"

// Forward declaration(s):
class MsgStream;

namespace ROIB {

  /**
   *   $Date: 2008-02-01 17:04:19 $
   *
   *   @short Class holding the RoIs from the MuCTPI collected by the RoIB
   *
   *          The RoIBuilder receives input from the LVL1 calorimeter trigger,
   *          the CTP and the MuCTPI. In its output the RoIs coming from the
   *          MuCTPI are stored in one of these objects.
   *
   *     @see ROIB::RoIBuilder
   *     @see ROIB::RoIBResult
   *     @see ROIB::Header
   *     @see ROIB::Trailer
   *     @see ROIB::MuCTPIRoI
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author David Berge <David.Berge@cern.ch>
   *
   * @version $Revision: 1.7 $
   */
  class MuCTPIResult {

  public:
    /// Constructor with header, trailer and RoI vector
    MuCTPIResult( Header&& head, Trailer&& trail, std::vector< MuCTPIRoI > &&roIVector );
    /// Default constructor
    MuCTPIResult();
    /// Destructor
    ~MuCTPIResult() = default;

    MuCTPIResult(MuCTPIResult&&) noexcept = default;
    MuCTPIResult& operator=(MuCTPIResult&&) noexcept = default;
    
    /// Member function returning the header
    const Header& header() const;
    /// Member function returning the trailer
    const Trailer& trailer() const;
    /// Member function returning the RoI vector
    const std::vector< MuCTPIRoI >& roIVec() const;

    //! dump raw object content to string
    /** method used for read/write testing and T/P separation */
    const std::string dump() const;    
    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

    /// Function dumping the stored information to the message stream
    void dumpData() const;
    /// Function dumping the stored information to the message stream
    void dumpData( MsgStream& ) const;

  private:
    /// Variable storing the header for the MuCTPI block
    Header m_MuCTPIResultHeader;
    /// Variable storing the trailer for the MuCTPI block
    Trailer m_MuCTPIResultTrailer;
    /// Variable storing the MuCTPI RoIs collected by the RoIB
    std::vector< MuCTPIRoI > m_MuCTPIResultRoIVec;

  }; // class MuCTPIResult

} // namespace ROIB

#endif // TRIGT1RESULT_MUCTPIRESULT_H
