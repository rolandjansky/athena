// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctData.h 439303 2011-05-25 15:20:17Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCTDATA_H
#define TRIGT1MUCTPI_MIOCTDATA_H

// STL include(s):
#include <vector>

// Local include(s):
#include "../Common/EventID.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

  // Forward declaration(s):
  class MioctID;
  class MioctSectorSet;
  class OverlapLogic;

  /**
   *******************************************************************
   *
   *    $Date: 2011-05-25 17:20:17 +0200 (Wed, 25 May 2011) $ 
   *
   *    @short Helper class for access of MIOCT data in MIBAK format
   *
   *           MioctData is a helper class to provide easy access 
   *           of the data of one MIOCT module in the format as seen
   *           on the MIBAK backplane by the Read Out Driver MIROD.
   *
   *      @see MioctModule
   *   @author  Author: Thorsten Wengler
   *  @version $Revision: 439303 $
   *
   *******************************************************************
   */
  class MioctData {

  public:
    /**
     * The constructor of MioctData
     */
    MioctData();

    /**
     * This method fills the pointer list and additional words
     * with the necessary data from its MIOCT module
     */
    void assignData( const MioctID&, const MioctSectorSet&, const OverlapLogic* );
    /**
     * This method sets the word count in the trailor word -
     * needs to be an extra method as this has to be set
     * at a later stage ( not in assignData), when the number
     * of sector words after zero suppression has been determined
     * @param theWordCount word count for this MIOCT, including
     *                     header and trailor word
     * @see MioctData::assignData()
     */
    void setTrailerWordCount( unsigned int theWordCount );
    /**
     * This method returns the start marker, which on the MIBAK
     * is contained in bit 32 - 35, and marks the beginning of
     * a MIOCT module in the data stream
     * @return the MIOCT start marker
     */
    unsigned int getStartMarker () const { return m_startMioct; }
    /**
     * This method returns the end marker, which on the MIBAK
     * is contained in bit 32 - 35, and marks the end of
     * a MIOCT module in the data stream
     * @return the MIOCT end marker
     */
    unsigned int getEndMarker () const { return m_endMioct; }
    /**
     * This method returns the header word
     * @return the header word with module number,
     *         event ID and BCID
     */
    unsigned int getHeader () const { return m_headerWord; }
    /**
     * This method returns the trailor word
     * @return the trailer word with Pt sums,
     *         flags and word count
     */
    unsigned int getTrailer () const { return m_trailerWord; }
    /**
     * This method returns a reference to the list of sector
     * data words of this MIOCT. Each sector gets a word, even
     * if empty, such that the position is equal to the sector
     * number
     */
    const std::vector< unsigned int >* getSectorWords() const { return &m_sectorWords; }
    const std::vector< unsigned int >* getSectorHighBits() const { return &m_sectorHighBits; }

  private:
    unsigned int                m_headerWord;
    static const unsigned int   m_startMioct;
    unsigned int                m_trailerWord;
    static const unsigned int   m_endMioct;
    unsigned int                m_flags;
    std::vector< unsigned int > m_sectorWords;
    std::vector< unsigned int > m_sectorHighBits;
    EventID                     m_eventID;
    static const unsigned int   m_sectorNumberMap[ SECTORS_IN_MIOCT ];

  }; // class MioctData

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCTDATA_H
