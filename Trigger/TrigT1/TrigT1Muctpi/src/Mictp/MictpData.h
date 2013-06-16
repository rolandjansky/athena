// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MictpData.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MICTPDATA_H
#define TRIGT1MUCTPI_MICTPDATA_H

// STL include(s):
#include <list>

namespace LVL1MUCTPI {

  /**
   *******************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short Helper class for access of MICTP data in MIBAK format
   *
   *           MictpData is a helper class to provide easy access 
   *           of the data of the MICTP module in the format as seen
   *           on the MIBAK backplane by the Read Out Driver MIROD.
   *
   *      @see MictpModule
   *   @author Thorsten Wengler
   *  @version $Revision: 362102 $
   *
   *******************************************************************
   */
  class MictpData {

  public:
    /**
     * The constructor of MictpData
     */
    MictpData();
    /**
     * This method sets the pointer to the highBits list in the
     * MictpModule
     * @see MictpModule
     * @param thehighBits Pointer to the list of highBits
     *                    in the MictpModule
     */
    void setHighBits( const std::list< unsigned int >* thehighBits ) { m_highBits = thehighBits; }
    /**
     * This method sets the pointer to the dataWords list in the
     * MictpModule
     * @see MictpModule
     * @param theDataWords Pointer to the list of dataWords
     *                     in the MictpModule
     */
    void setDataWords( const std::list< unsigned int >* theDataWords ) { m_dataWords = theDataWords; }
    /**
     * This method gets the pointer to the highBits list in the
     * MictpModule
     * @see MictpModule
     * @return Pointer to the list of highBits
     *         in the MictpModule
     */
    const std::list< unsigned int >* getHighBits() const { return m_highBits; }
    /**
     * This method gets the pointer to the dataWords list in the
     * MictpModule
     * @see MictpModule
     * @return Pointer to the list of dataWords
     *         in the MictpModule
     */
    const std::list< unsigned int >* getDataWords() const { return m_dataWords; }
    /**
     * The destructor of MictpData
     */
    ~MictpData();

  private:
    const std::list< unsigned int >* m_highBits;
    const std::list< unsigned int >* m_dataWords;

  }; // class MictpData

} // namespace LVL1MUCTPI

#endif  // TRIGT1MUCTPI_MICTPDATA_H
