// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATA_H
#define TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATA_H

// System include(s):
#include <iosfwd>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicData.h"

namespace LVL1MUONIF {

   /**
    *  @short Class representing data from an endcap SL board
    *
    *         This class representds the data coming from a single
    *         TGC endcap SL board to the MuCTPI.
    *
    *    @see LVL1MUONIF::Lvl1MuSectorLogicData
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class Lvl1MuEndcapSectorLogicData : public Lvl1MuSectorLogicData {

   public:
      Lvl1MuEndcapSectorLogicData();
      ~Lvl1MuEndcapSectorLogicData() {}

      Lvl1MuEndcapSectorLogicData& operator=( const Lvl1MuSectorLogicData& right );

      /// Get data in 32 bit word format
      SectorLogicWord getWordFormat() const;
      /// Convert SectorLogicData from 32 bit word data
      const Lvl1MuSectorLogicData& convertFromWordFormat( const SectorLogicWord& );

   protected:
      virtual void printOut( std::ostream& out ) const;

   }; // class Lvl1MuEndcapSectorLogicData

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUENDCAPSECTORLOGICDATA_H
