// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATA_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATA_H

// System include(s):
#include <iosfwd>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicData.h"

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
   class Lvl1MuBarrelSectorLogicData: public Lvl1MuSectorLogicData {

   public:
      Lvl1MuBarrelSectorLogicData();
      ~Lvl1MuBarrelSectorLogicData() {}

      Lvl1MuBarrelSectorLogicData& operator=( const Lvl1MuSectorLogicData& right );

      /// Get data in 32 bit word format
      SectorLogicWord getWordFormat() const;
      /// Convert SectorLogicData from 32 bit word data
      const Lvl1MuSectorLogicData& convertFromWordFormat( const SectorLogicWord& );

   protected:
      virtual void printOut( std::ostream& out ) const;

   }; // class Lvl1MuBarrelSectorLogicData

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUBARRELSECTORLOGICDATA_H
