/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonCandidate.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// STL include(s):
#include <iostream>

// Local include(s):
#include "MuonCandidate.h"

/*******************************************************************
 * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
 *
 * Implementation of class MuonCandidate
 * @author   Author: Thorsten Wengler
 * @version $Revision: 364083 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor of class MuonCandidate
   MuonCandidate::MuonCandidate()
      : m_bcid( 1 ), m_roi1( 0 ), m_roi2( 0 ), m_ovl1( 0 ), m_ovl2( 0 ), m_pt1( 7 ),
        m_pt2( 7 ), m_twoCand1( false ), m_twoCand2( false ), m_gtTwoCandInSec( false ) {

   }

   // overload for <<
   std::ostream& operator<<( std::ostream& out, const MuonCandidate& right ) {

      out << right.m_theSectorID;
      out << " Pt1: "  << right.m_pt1;
      out << " RoI1: "  << right.m_roi1;
      out << " Ovl1: "  << right.m_ovl1;
      out << " More1inPad1: " << right.m_twoCand1 << std::endl;
      if( right.m_pt2 < 7 ) {
         out << " Pt2: "  << right.m_pt2;
         out << " RoI2: "  << right.m_roi2;
         out << " Ovl2: "  << right.m_ovl2;
         out << " More1InPad2: "  << right.m_twoCand2 << std::endl;
      }
      out << " BCID: " << right.m_bcid;
      out << " More2inSec: " << right.m_gtTwoCandInSec << std::endl;

      return out;
   }

} // namespace LVL1MUCTPI
