/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* JetEnergyResult is the JetEnergy part of the L1 RDO.
* Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */
#ifndef TRIGT1RESULT_JETENERGYRESULT_H
#define TRIGT1RESULT_JETENERGYRESULT_H

// STL include(s):
#include <vector>

// Local include(s):
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "TrigT1Result/JetEnergyRoI.h"

namespace ROIB {

  class JetEnergyResult {

  public:
    /* Constructor with header, trailer and RoI vector */
    JetEnergyResult( Header&&, Trailer&&, std::vector< JetEnergyRoI >&& );
    /* Empty constructor */
    JetEnergyResult();

    /* Member function returning the header */
    const Header& header() const;
    /* Member function returning the trailer */
    const Trailer& trailer() const;
    /* Member function returning the RoI vector */
    const std::vector< JetEnergyRoI >& roIVec() const;

    //! dump raw object content to string
    /** method used for read/write testing and T/P separation */
    const std::string dump() const;    
    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

  private:
    /* Data members - header, trailer and RoI vector */
    Header m_JetEnergyResultHeader;                             //!< header fragment in LVL1 eformat
    Trailer m_JetEnergyResultTrailer;                           //!< trailer fragment in LVL1 eformat
    std::vector< JetEnergyRoI > m_JetEnergyResultRoIVec;        //!< raw data content (RoIs)

  }; // class JetEnergyResult

} // namespace ROIB

#endif // TRIGT1RESULT_JETENERGYRESULT_H
