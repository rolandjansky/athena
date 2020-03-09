/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATAPHASE1_H
#define TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATAPHASE1_H

// System include(s):
#include <iosfwd>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicDataPhase1.h"

namespace LVL1MUONIF {

   /**
    *  @short Class representing data from a barrel SL board
    *
    *         This class represents the data coming from a single
    *         RPC SL board to the MuCTPI.
    *
    *    @see LVL1MUONIF::Lvl1MuSectorLogicData
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class Lvl1MuBarrelSectorLogicDataPhase1: public Lvl1MuSectorLogicDataPhase1 {

   public:
      Lvl1MuBarrelSectorLogicDataPhase1();
      virtual ~Lvl1MuBarrelSectorLogicDataPhase1() {}

      Lvl1MuBarrelSectorLogicDataPhase1& operator=( const Lvl1MuSectorLogicDataPhase1& right );

      /// Get data in 32 bit word format
      virtual SectorLogicWord getWordFormat() const override;
      /// Convert SectorLogicDataPhase1 from 32 bit word data
      virtual const Lvl1MuSectorLogicDataPhase1& convertFromWordFormat( const SectorLogicWord& ) override;

   protected:
      virtual void printOut( std::ostream& out ) const override;

   }; // class Lvl1MuBarrelSectorLogicDataPhase1

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATAPHASE1_H
