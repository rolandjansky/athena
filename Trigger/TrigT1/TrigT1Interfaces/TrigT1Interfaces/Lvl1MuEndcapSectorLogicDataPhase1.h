// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATAPHASE1_H
#define TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATAPHASE1_H

// System include(s):
#include <iosfwd>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicDataPhase1.h"

namespace LVL1MUONIF {

   /**
    *  @short Class representing data from an endcap SL board
    *
    *         This class representds the data coming from a single
    *         TGC endcap SL board to the MuCTPI.
    *
    *    @see LVL1MUONIF::Lvl1MuSectorLogicDataPhase1
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class Lvl1MuEndcapSectorLogicDataPhase1 : public Lvl1MuSectorLogicDataPhase1 {

   public:
      Lvl1MuEndcapSectorLogicDataPhase1();
      ~Lvl1MuEndcapSectorLogicDataPhase1() {}

      Lvl1MuEndcapSectorLogicDataPhase1& operator=( const Lvl1MuSectorLogicDataPhase1& right );

      /// Get data in 32 bit word format
      SectorLogicWord getWordFormat() const;
      /// Convert SectorLogicDataPhase1 from 32 bit word data
      const Lvl1MuSectorLogicDataPhase1& convertFromWordFormat( const SectorLogicWord& );

   protected:
      virtual void printOut( std::ostream& out ) const;

   }; // class Lvl1MuEndcapSectorLogicDataPhase1

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATAPHASE1_H
