// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1INTERFACES_LVL1MUCTPIINPUT_H
#define TRIGT1INTERFACES_LVL1MUCTPIINPUT_H

// System include(s):
#include <iosfwd>
#include <fstream>
#include <vector>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicData.h"
#include "TrigT1Interfaces/Lvl1MuBarrelSectorLogicData.h"
#include "TrigT1Interfaces/Lvl1MuEndcapSectorLogicData.h"
#include "TrigT1Interfaces/Lvl1MuForwardSectorLogicData.h"

namespace LVL1MUONIF {

   /**
    *  @short Class representing (part of) the input data to the MuCTPI
    *
    *         This class can represent the full input data coming from the
    *         SL boards to the MuCTPI, or it can just represent a part of it.
    *         (Let's say only the data coming from the RPC simulation.)
    *
    *    @see LVL1MUONIF::Lvl1MuBarrelSectorLogicData
    *    @see LVL1MUONIF::Lvl1MuEndcapSectorLogicData
    *    @see LVL1MUONIF::Lvl1MuForwardSectorLogicData
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class Lvl1MuCTPIInput {

   public:
      Lvl1MuCTPIInput();
      Lvl1MuCTPIInput( const Lvl1MuCTPIInput& );
      virtual ~Lvl1MuCTPIInput();

      Lvl1MuCTPIInput& operator=( const Lvl1MuCTPIInput& right );

   protected:
      // data from each sector logic is identifed by
      //  System Address     :  Barrel=0, Endcap=1, Forward=2
      //  Sub-system Address :  A_side(Z>0) =1, C_side(Z<0) =0
      //  Sector Address     :  Barrel=0-31, Endcap=0-47, Forward=0-23
      enum MuonSystem { Barrel = 0, Endcap = 1, Forward = 2 };
      enum MuonSubSystem { A_side = 1, C_side = 0};
      enum { NumberOfMuonSystem = 3 };
      enum { NumberOfMuonSubSystem = 2 };
      enum { NumberOfBarrelSector = 32 };
      enum { NumberOfEndcapSector = 48 };
      enum { NumberOfForwardSector = 32 };

   public:
      static size_t idBarrelSystem() { return Barrel; }
      static size_t numberOfBarrelSector() { return NumberOfBarrelSector; }
      static size_t idEndcapSystem() { return Endcap; }
      static size_t numberOfEndcapSector() { return NumberOfEndcapSector; }
      static size_t idForwardSystem() { return Forward; }
      static size_t numberOfForwardSector() { return NumberOfForwardSector; }
      static size_t idSideA() { return A_side; }
      static size_t idSideC() { return C_side; }

      const Lvl1MuSectorLogicData& getSectorLogicData( size_t systemAddress,
                                                       size_t subSystemAddress,
                                                       size_t sectorAddress ) const;

      void setSectorLogicData( const Lvl1MuSectorLogicData& data,
                               size_t systemAddress,
                               size_t subSystemAddress,
                               size_t sectorAddress );

      void setSectorLogicData( const unsigned int & sectorWord,
                               size_t systemAddress,
                               size_t subSystemAddress,
                               size_t sectorAddress );

      /**
       * Merge SectorLogic data of another object into this
       * data of a system (Barrel, Endcap or Forward) is not overwritten
       * if this object already has corresponding data
       */
      void merge( const Lvl1MuCTPIInput& another );

      void clear( size_t systemAddress );
      /// Clear Sector Logic data
      void clearAll();

      /// Return "true" if  data of specified system is empty
      bool isEmpty( size_t systemAddress ) const;

      friend std::ostream& operator<<( std::ostream&, const Lvl1MuCTPIInput& );

      /// To store object to file. (NOT TO BE USED ANYMORE...)
      bool storeInWordFormat( std::ofstream& fOut, bool ascii = true );
      /// To retrieve object from file. (NOT TO BE USED ANYMORE...)
      bool retrieveInWordFormat( std::ifstream& fIn, bool ascii = true );

   private:
      bool storeInWordFormat( std::ofstream& fOut,
                              bool ascii,
                              size_t systemAddress,
                              size_t subSystemAddress,
                              size_t sectorAddress );

      void reserve( size_t systemAddress );

      std::vector<Lvl1MuBarrelSectorLogicData> m_barrel[ NumberOfMuonSubSystem ];
      std::vector<Lvl1MuEndcapSectorLogicData> m_endcap[ NumberOfMuonSubSystem ];
      std::vector<Lvl1MuForwardSectorLogicData> m_forward[ NumberOfMuonSubSystem ];

   }; // class Lvl1MuCTPIInput

} // namespace LVL1MUONIF

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1MUONIF::Lvl1MuCTPIInput, 6102, 0 )

#endif // TRIGT1INTERFACES_LVL1MUCTPIINPUT_H
