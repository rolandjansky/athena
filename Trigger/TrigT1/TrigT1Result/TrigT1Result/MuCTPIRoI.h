// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_MUCTPIROI_H
#define TRIGT1RESULT_MUCTPIROI_H

// System include(s):
#include <stdint.h>

// Local include(s):
#include "TrigT1Result/MuCTPI_RDO.h"

namespace ROIB {

  /**
   *   $Date: 2007-07-04 17:28:03 $
   *
   *   @short Class for storing the 32-bit muon RoI word
   *
   *          This class is used to store one muon RoI as collected by the RoI Builder.
   *          The RoI itself is a simple 32-bit unsigned integer, so the whole class
   *          rather exists for its access methods only.
   *
   *          For a detailed description of the data format of the MuCTPI, see
   *          the MIROD documentation (https://edms.cern.ch/file/248757/1/mirod.pdf).
   *
   *     @see ROIB::MuCTPIResult
   *     @see ROIB::RoIBResult
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author David Berge <David.Berge@cern.ch>
   *
   * @version $Revision: 1.4 $
   */
  class MuCTPIRoI {

  public:
    /// Constructor with the actual RoI word
    MuCTPIRoI( uint32_t );
    /// Default constructor
    MuCTPIRoI();
    /// Destructor
    ~MuCTPIRoI()= default;

    /// Method returning the RoI word
    uint32_t roIWord() const;
    /// Method returning the p<sub>T</sub> of the RoI candidate
    unsigned int pt() const;
    /// Method returning the "electronics address" of the candidate
    unsigned int getSectorAddress() const;
    /// Method returning the hemisphere of the candidate
    uint16_t getHemisphere() const;
    /// Method telling whether there were more than two candidates in this candidate's sector
    uint16_t getSectorOverflow() const;
    /// Method telling whether there were more than one candidates in this candidate's RoI
    uint16_t getRoiOverflow() const;
    /// Method returning the RoI in which the candidate was detected.
    uint16_t getRoiNumber() const;
    /// Method returning the "overlap bits" of the candidate
    uint16_t getOverlapBits() const;
    /// Method returning the name of the system that produced the RoI
    MuCTPI_RDO::SectorLocation getSectorLocation() const;
    /// Method returning the number of the trigger sector producing the candidate
    uint16_t getSectorID() const;
    /// Method telling whether the candidate had the highest p<sub>T</sub> in the sector
    uint16_t getCandidateIsHighestPt() const;

    /// Position of the sector address in the RoI word
    static const uint32_t ROI_CAND_SECTOR_ADDRESS_SHIFT = 14;
    /// Position of the "highest p<sub>T</sub> bit" in the RoI word
    static const uint32_t ROI_CAND_HIGHEST_PT_SHIFT = 22;

  private:
    /// The RoI word itself
    uint32_t m_roIWord;

  }; // class MuCTPIRoI

} // namespace ROIB

#endif // TRIGT1RESULT_MUCTPIROI_H
