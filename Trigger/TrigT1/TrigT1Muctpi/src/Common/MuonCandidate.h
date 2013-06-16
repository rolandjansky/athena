// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonCandidate.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_MUONCANDIDATE_H
#define TRIGT1MUCTPI_MUONCANDIDATE_H

// Local include(s):
#include "../Mioct/SectorID.h"

namespace LVL1MUCTPI {

   /**
    *  @short Information for one sector logic muon candidate word
    *
    *         This class combines the information to form one word
    *         as delivered by the muon sector logic. It may
    *         contain 2 actual muon candidates
    *
    *  @author Thorsten Wengler
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class MuonCandidate {

   public:
      /**
       * The constructor of MuonCandidate
       */
      MuonCandidate();
      /**
       * set or get the SectorID
       */
      SectorID secID() const { return m_theSectorID; }
      void secID( SectorID theID ) { m_theSectorID = theID; }
      /**
       * set or get the BCID
       */
      unsigned int bcid() const { return m_bcid; }
      void bcid( unsigned int theBCID ) { m_bcid = theBCID; }
      /**
       * set or get RoI for candidate 1
       */
      unsigned int roi1() const { return m_roi1; }
      void roi1( unsigned int theRoI1 ) { m_roi1 = theRoI1; }
      /**
       * set or get RoI for candidate 2
       */
      unsigned int roi2() const { return m_roi2; }
      void roi2( unsigned int theRoI2 ) { m_roi2 = theRoI2; }
      /**
       * set or get Overlap for candidate 1
       */
      unsigned int ovl1() const { return m_ovl1; }
      void ovl1( unsigned int theOvl1 ) { m_ovl1 = theOvl1; }
      /**
       * set or get Overlap for candidate 2
       */
      unsigned int ovl2() const { return m_ovl2; }
      void ovl2( unsigned int theOvl2 ) { m_ovl2 = theOvl2; }
      /**
       * set or get pt for candidate 1
       */
      unsigned int pt1() const { return m_pt1; }
      void pt1( unsigned int thePt1 ) { m_pt1 = thePt1; }
      /**
       * set or get pt for candidate 2
       */
      unsigned int pt2() const { return m_pt2; }
      void pt2( unsigned int thePt2 ) { m_pt2 = thePt2; }
      /**
       * set or get two cand. in pad flag candidate 1
       */
      bool twoCand1() const { return m_twoCand1; }
      void twoCand1( bool theTwoCand1 ) { m_twoCand1 = theTwoCand1; }
      /**
       * set or get two cand. in pad flag candidate 2
       */
      bool twoCand2() const { return m_twoCand2; }
      void twoCand2( bool theTwoCand2 ) { m_twoCand2 = theTwoCand2; }
      /**
       * set or get two cand. in sector flag
       */
      bool gtTwoCandInSec() const { return m_gtTwoCandInSec; }
      void gtTwoCandInSec( bool theGtTwoCandInSec ) { m_gtTwoCandInSec = theGtTwoCandInSec; }
      /**
       * overload the << operator to have easy printout
       */
      friend std::ostream& operator<<( std::ostream&, const MuonCandidate& );

   private:
      SectorID m_theSectorID;
      unsigned int m_bcid;
      unsigned int m_roi1;
      unsigned int m_roi2;
      unsigned int m_ovl1;
      unsigned int m_ovl2;
      unsigned int m_pt1;
      unsigned int m_pt2;
      bool m_twoCand1;
      bool m_twoCand2;
      bool m_gtTwoCandInSec;

   }; // class MuonCandidate

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MUONCANDIDATE_H
