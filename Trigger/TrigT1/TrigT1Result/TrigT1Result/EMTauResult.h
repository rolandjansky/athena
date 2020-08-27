/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* EMTauResult is the EMTau part of the L1 RDO.
* Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */
#ifndef TRIGT1RESULT_EMTAURESULT_H
#define TRIGT1RESULT_EMTAURESULT_H

// STL include(s):
#include <vector>
#include <string>

// Local include(s):
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "TrigT1Result/EMTauRoI.h"

namespace ROIB {

  class EMTauResult {

  public:
    /* Constructor with header, trailer and RoI vector */
    EMTauResult( Header&&, Trailer&&, std::vector< EMTauRoI >&& );
    /* Default constructor */
    EMTauResult();
    ~EMTauResult();

    EMTauResult(EMTauResult&&) noexcept = default;
    EMTauResult(const EMTauResult&) = default;
    EMTauResult& operator=(EMTauResult&&) noexcept =default;
    EMTauResult& operator=(const EMTauResult&) =default;

    /* Member function returning the header */
    const Header& header() const;
    /* Member function returning the trailer */
    const Trailer& trailer() const;
    /* Member function returning the RoI vector */
    const std::vector< EMTauRoI >& roIVec() const;

    //! dump raw object content to string
    /** method used for read/write testing and T/P separation */
    const std::string dump() const;    
    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

  private:
    /* Data members = header, trailer and RoI vector */
    Header m_EMTauResultHeader;                               //!< header fragment in LVL1 eformat
    Trailer m_EMTauResultTrailer;                             //!< trailer fragment in LVL1 eformat
    std::vector< EMTauRoI > m_EMTauResultRoIVec;              //!< raw data content (RoIs)

  }; // class EMTauResult

} // namespace ROIB

#endif // TRIGT1RESULT_EMTAURESULT_H
