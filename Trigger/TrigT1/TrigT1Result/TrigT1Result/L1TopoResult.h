/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_L1TOPORESULT_H
#define TRIGT1RESULT_L1TOPORESULT_H


#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "L1TopoRDO/L1TopoRDO.h"

namespace ROIB {

  /**
   * @short L1TopoResult is the L1Topo part of the RoIBResult.
   *
   * L1TopoResult is created once per L1Topo board. 
   * It contains the L1TopoRDO unpacked from the data from the 'ROI' link of the L1Topo modules along with the standard Header and Trailer. 
   * The format is the same although the content is reduced to just the CTP ROI blocks
   * compared to the full 'DAQ' output.
   *
   *     @see L1TopoRDO
   *     @see ROIB::Header
   *     @see ROIB::Trailer
   *
   */

  class L1TopoResult {

  public:
    //! Constructor with header, trailer and RDO
    L1TopoResult( Header&&, Trailer&&, L1TopoRDO&&) noexcept;

    //! Get methods

    //! Member function returning the header
    const Header& header() const;
    //! Member function returning the trailer
    const Trailer& trailer() const;
    //! Member function returning the RDO
    const L1TopoRDO& rdo() const;

    //! print myself
    const std::string dump() const;

  private:
    //! Data members = header, trailer and RDO
    Header m_header;
    Trailer m_trailer;
    L1TopoRDO m_RDO;

  }; // class L1TopoResult

} // namespace ROIB

#endif //TRIGT1RESULT_L1TOPORESULT_H
