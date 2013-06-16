// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenTestPattern.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_GENTESTPATTERN_H
#define TRIGT1MUCTPI_GENTESTPATTERN_H

// STL include(s):
#include <string>
#include <list>

// Local include(s):
#include "MuonCandidate.h"
#include "RandomCandidateRange.h"

// Forward declaration(s):
namespace LVL1MUONIF {
  class Lvl1MuCTPIInput;
}

namespace LVL1MUCTPI {

  // Forward declaration(s):
  class SectorID;

  /**
   *******************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short Generate Sector Logic test input
   *
   *           This class is used to generate test input data using 
   *           the Lvl1MuCTPIInput interface class from the 
   *           Sector Logic simulation. The constructor receives
   *           a reference to a Lvl1MuCTPIInput object, which is
   *           then filled using the Lvl1MuCTPIInput member 
   *           functions.
   *
   *      @see Lvl1MuCTPIInput
   *   @author  Author: Thorsten Wengler
   *  @version $Revision: 362102 $
   *
   *******************************************************************
   */
  class GenTestPattern {

  public:
    /**
     * The only constructor of the GenTestPattern class takes a 
     * reference to an Lvl1MuCTPIInput object, which is to be filled
     * by the test pattern
     * @param theLvl1Input The Sector Logic input object to be filled with a test pattern
     * @see Lvl1MuCTPIInput
     */
    GenTestPattern( LVL1MUONIF::Lvl1MuCTPIInput* theLvl1Input );
    /**
     * Set the sector by hand. The internal sector list is not used here
     * but the sector is written to the Lvl1MuCTPIInput directly. There
     * is no need to call putEvent after this.
     * 
     * @param theSectorID  The ID of the sector to be filled
     * @param bcid         The BC ID
     * @param roi1         Id of RoI candidate 1
     * @param roi2         Id of RoI candidate 1
     * @param ovl11        Overlay candidate 1
     * @param ovl12        Overlay candidate 2
     * @param pt1          momentum candidate 1
     * @param pt2          momentum candidate 2
     * @param twoCand1     more than one cand in pad candidate 1
     * @param twoCand2     more than one cand in pad candidate 2
     * @param twoCandInSec more than two candiates in sector
     *
     * @see GenTestPattern::putEvent()
     */
    void fillSectorDirect( SectorID theSectorID,
			   unsigned int bcid,
			   unsigned int roi1,
			   unsigned int roi2,
			   unsigned int ovl11,
			   unsigned int ovl12,
			   unsigned int pt1,
			   unsigned int pt2,
			   bool twoCand1,
			   bool twoCand2,
			   bool twoCandInSec );
    /**
     * Set the event by reading a list of single muon candidates from a
     * file. This is usefule for tests which require frequent changes in
     * candidate settings, as one does not need to recompile. The
     * internal sector list is not used here but the sector is written
     * to the Lvl1MuCTPIInput directly. There is no need to call
     * putEvent after this. The file has the following format for each
     * candidate (one line per candidate):
     * sys-ID subSys-ID Sector-No bcid roi1 roi2 ovl1 ovl2 pt1 pt2
     *   twoCand1 twoCand2 twoCandinSector
     *
     * @param fileName name of the file holding the candidates
     */
    void fillEventFromFile( std::string fileName );
    /**
     * second version of direct filler with a MuonCandidate Object
     * as input
     * @param muonCand the input candidate
     */
    void fillSectorDirect( const MuonCandidate& muonCand );
    /**
     * method to return the object holding the Range definitions
     * for the random generation of muon candidates
     * @return m_RandomMuonCandidateRange;
     */
    RandomCandidateRange& rndRange() { return m_RandomMuonCandidateRange; }
    /**
     * Reinitialize the random number generator with a new seed
     * @param newSeed The new random number seed
     */
    static void reSeedRandomNumbers( unsigned int newSeed ){ srand( newSeed ); }
    /**
     * method to randomly generate a muon candidate. 
     */
    MuonCandidate randomCandidate();
    /**
     * Check if a given Muon candidate already exists in the list 
     * of candidates data memeber
     * @param muonCand the candidate to be checked
     * @return true if the candidate already exists
     */
    bool notUniqueCandidate( MuonCandidate & muonCand );
    /**
     * Generate one random Event and fill it into the candidate list
     * data memeber
     * @param lowCand at least this many candidates
     * @param highCand at most this many candidates
     */
    void randomEvent( unsigned int lowCand, unsigned int highCand );
    /**
     * this is the main event generation method
     */
    void generateEvent( unsigned int lowCand, unsigned int highCand );
    /**
     * print the event generated by randomEvent
     */
    void printRandomEvent();
    /**
     * Clear the input object of the sector logic of all
     * entries
     */
    void clearEvent();
    /**
     * The destructor of the GenTestPattern class -
     * nothing to destruct
     */
    ~GenTestPattern();

  protected:

    /**
     * method to generate a random integer number between the lower and
     * the upper bound
     * @param lower Lower bound of Random number range
     * @param upper Upper bound of Random number range
     * @return random number
     */
    static unsigned int randomNumber( unsigned int lower, unsigned int upper );

  private:

    LVL1MUONIF::Lvl1MuCTPIInput* m_input;
    std::list< MuonCandidate > m_candidateList;

    RandomCandidateRange m_RandomMuonCandidateRange;

  }; // class GenTestPattern

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_GENTESTPATTERN_H
