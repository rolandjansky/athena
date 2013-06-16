// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Verifyer.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_VERIFYER_H
#define TRIGT1MUCTPI_VERIFYER_H

// STL include(s):
#include <list>
#include <vector>

// Local include(s):
#include "../Mirod/MirodFiFoContent.h"

namespace LVL1MUCTPI {

  /**
  *******************************************************************
  *
  *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
  *
  *    @short Utility class to verify simulation/hardware results
  *
  *           This class offers methods to compare the output of
  *           of hardware and simulation
  *
  *      @see
  *   @author  Author: Thorsten Wengler
  *  @version $Revision: 362102 $
  *
  *******************************************************************
  */
  class Verifyer {

  public:
    /**
     * The constructor of Verifyer
     */
    Verifyer();
    /**
     * The destructor of Verifyer
     */
    ~Verifyer();

    /**
     * Compare the output intended for the RoI Builder as produced
     * by the simulation and the hardware
     * @param simOut Output of the simulation
     * @param hardwareOut Output of the hardware
     * @param printToScreen if true does printout line by line
     */
    unsigned int lvl2OutIsEqual( const std::list<unsigned int> & simOut,
                                 const std::list<unsigned int> & hardwareOut,
                                 bool printToScreen ) const;
    /**
     * set/get lvl2Header masks
     * @param headerWord  which of the 0 ... 7 header words
     * @param maskValue   value to which the mask is set
     */
    void lvl2HeaderMask( unsigned int headerWord, unsigned int maskValue ) { m_lvl2HeaderMask[ headerWord ] = maskValue; }
    unsigned int lvl2HeaderMask( unsigned int headerWord ) const { return m_lvl2HeaderMask[ headerWord ]; }
    /**
     * set/get lvl2Trailo masks
     * @param trailerWord  which of the 0 ... 4 trailor words
     * @param maskValue   value to which the mask is set
     */
    void lvl2TrailerMask( unsigned int trailerWord, unsigned int maskValue ) { m_lvl2TrailerMask[ trailerWord ] = maskValue; }
    unsigned int lvl2TrailerMask( unsigned int trailerWord ) const { return m_lvl2TrailerMask[ trailerWord ]; }
    /**
     * set/get lvl2Candidate mask
     * @param maskValue   value to which the mask is set
     */
    void lvl2CandidateMask( unsigned int maskValue ) { m_lvl2CandidateMask = maskValue; }
    unsigned int lvl2CandidateMask() const { return m_lvl2CandidateMask; }
    /**
     * set/get lvl2Header masks
     * @param headerWord  which of the 0 ... 7 header words
     * @param maskValue   value to which the mask is set
     */
    void daqHeaderMask( unsigned int headerWord, unsigned int maskValue ) { m_daqHeaderMask[ headerWord ] = maskValue; }
    unsigned int daqHeaderMask( unsigned int headerWord ) const { return m_daqHeaderMask[ headerWord ]; }
    /**
     * set/get daqTrailo masks
     * @param trailerWord  which of the 0 ... 4 trailor words
     * @param maskValue   value to which the mask is set
     */
    void daqTrailerMask( unsigned int trailerWord, unsigned int maskValue ) { m_daqTrailerMask[ trailerWord ] = maskValue; }
    unsigned int daqTrailerMask( unsigned int trailerWord ) const { return m_daqTrailerMask[ trailerWord ]; }
    /**
     * set/get daqMictp mask
     * @param maskValue   value to which the mask is set
     */
    void daqMictpMask( unsigned int maskValue ) { m_daqMictpMask = maskValue; }
    unsigned int daqMictpMask() const { return m_daqMictpMask; }
    /**
     * set/get daqCandidate mask
     * @param maskValue   value to which the mask is set
     */
    void daqCandidateMask( unsigned int maskValue ) { m_daqCandidateMask = maskValue; }
    unsigned int daqCandidateMask() const { return m_daqCandidateMask; }

    /**
     * Compare the output intended for the DAQ as produced
     * by the simulation and the hardware
     * @param simOut Output of the simulation
     * @param hardwareOut Output of the hardware
     * @param printToScreen if true does printout line by line
     */
    unsigned int daqOutIsEqual( const std::list<unsigned int> & simOut,
                                const std::list<unsigned int> & hardwareOut,
                                bool printToScreen ) const;

    bool mirodFiFosAreEqual( MirodFiFoContent & mirodFiFos,
                             MirodFiFoContent & simMirodFiFos ) const;

  private:
    std::vector< unsigned int >	m_lvl2HeaderMask;
    unsigned int	m_lvl2CandidateMask;
    std::vector< unsigned int >	m_lvl2TrailerMask;

    std::vector< unsigned int >	m_daqHeaderMask;
    unsigned int	m_daqMictpMask;
    unsigned int	m_daqCandidateMask;
    std::vector< unsigned int >	m_daqTrailerMask;

  }; // class Verifyer

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_VERIFYER_H
