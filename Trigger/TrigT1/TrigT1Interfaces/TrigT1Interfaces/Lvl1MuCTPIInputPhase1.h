/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_LVL1MUCTPIINPUTPHASE1_H
#define TRIGT1INTERFACES_LVL1MUCTPIINPUTPHASE1_H

// System include(s):
#include <iosfwd>
#include <vector>
#include <utility>
#include <memory>

// Local include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuBarrelSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuEndcapSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuForwardSectorLogicDataPhase1.h"

namespace LVL1MUONIF {

   /**
    *  @short Class representing (part of) the input data to the MuCTPI for Phase 1
    *
    *         This class can represent the full input data coming from the
    *         SL boards to the MuCTPI, or it can just represent a part of it.
    *         (Let's say only the data coming from the RPC simulation.)
    *
    *    @see LVL1MUONIF::Lvl1MuBarrelSectorLogicDataPhase1
    *    @see LVL1MUONIF::Lvl1MuEndcapSectorLogicDataPhase1
    *    @see LVL1MUONIF::Lvl1MuForwardSectorLogicDataPhase1
    *
    * $Revision: 782811 $
    * $Date: 2016-11-07 18:20:40 +0100 (Mon, 07 Nov 2016) $
    */
   class Lvl1MuCTPIInputPhase1 {

   public:
      Lvl1MuCTPIInputPhase1();
      Lvl1MuCTPIInputPhase1( const Lvl1MuCTPIInputPhase1& );
      virtual ~Lvl1MuCTPIInputPhase1();

      Lvl1MuCTPIInputPhase1& operator=( const Lvl1MuCTPIInputPhase1& right );


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
      enum { NumberOfForwardSector = 24 }; 


      static size_t idBarrelSystem();
      static size_t numberOfBarrelSector();
      static size_t idEndcapSystem();
      static size_t numberOfEndcapSector();
      static size_t idForwardSystem();
      static size_t numberOfForwardSector();
      static size_t idSideA();
      static size_t idSideC();
      static size_t numberOfSystems();
      static size_t numberOfSector(MuonSystem system);


      const Lvl1MuSectorLogicDataPhase1& getSectorLogicData( size_t systemAddress,
							     size_t subSystemAddress,
							     size_t sectorAddress,
							     int    bcid=0        ) const;
      std::shared_ptr<Lvl1MuSectorLogicDataPhase1> getSectorLogicDataPtr( size_t systemAddress,
									  size_t subSystemAddress,
									  size_t sectorAddress,
									  int    bcid=0        );
      
      void setSectorLogicData( const Lvl1MuSectorLogicDataPhase1& data,
                               size_t systemAddress,
                               size_t subSystemAddress,
                               size_t sectorAddress,
			       int    bcid=0        );

      void setSectorLogicData( const Lvl1MuSectorLogicDataPhase1::SectorLogicWord & sectorWord,
                               size_t systemAddress,
                               size_t subSystemAddress,
                               size_t sectorAddress,
			       int    bcid=0        );

      /**
       * Merge SectorLogic data of another object into this
       * data of a system (Barrel, Endcap or Forward) is not overwritten
       * if this object already has corresponding data
       */
      void merge( const Lvl1MuCTPIInputPhase1& another );

      void clear( size_t systemAddress );
      /// Clear Sector Logic data
      void clearAll();

      /// Return "true" if  data of specified system is empty
      bool isEmpty( size_t systemAddress,
		    int    bcid=0         ) const;

      /// Return "true" if  data of all systems for this bcid is empty
      bool isEmptyAll(int bcid=0) const;

      /// Return "true" if data of specified system is filled
      /// for bunches other than 'current' bunch 
      bool hasOutOfTimeCandidates(size_t systemAddress)  const;

     friend std::ostream& operator<<( std::ostream&, const Lvl1MuCTPIInputPhase1& );

   private:

      size_t reserve( size_t systemAddress ,
		      int    bcid=0         );
     
      size_t getSystemIndex(size_t systemAddress,
			    size_t subSystemAddress,
			    size_t sectorAddress ) const;

     size_t getBcidIndex(size_t systemAddress,
                         int    bcid=0) const;

     bool m_isFilledOutOfTimeCandidates[NumberOfMuonSystem];

     
     typedef std::vector<std::shared_ptr <Lvl1MuSectorLogicDataPhase1> > Lvl1MuVect;
     typedef std::pair<int, Lvl1MuVect>  Lvl1MuVectWithBC; 
     std::vector<Lvl1MuVectWithBC> m_data[ NumberOfMuonSystem ];
   }; // class Lvl1MuCTPIInputPhase1
   
   inline size_t Lvl1MuCTPIInputPhase1::idBarrelSystem() { return Barrel; }
   inline size_t Lvl1MuCTPIInputPhase1::numberOfBarrelSector() { return NumberOfBarrelSector; }
   inline size_t Lvl1MuCTPIInputPhase1::idEndcapSystem() { return Endcap; }
   inline size_t Lvl1MuCTPIInputPhase1::numberOfEndcapSector() { return NumberOfEndcapSector; }
   inline size_t Lvl1MuCTPIInputPhase1::idForwardSystem() { return Forward; }
   inline size_t Lvl1MuCTPIInputPhase1::numberOfForwardSector() { return NumberOfForwardSector; }
   inline size_t Lvl1MuCTPIInputPhase1::idSideA() { return A_side; }
   inline size_t Lvl1MuCTPIInputPhase1::idSideC() { return C_side; }
   inline size_t Lvl1MuCTPIInputPhase1::numberOfSystems() { return NumberOfMuonSystem; }
   inline size_t Lvl1MuCTPIInputPhase1::numberOfSector(MuonSystem system)
     {
       if (system == Barrel) return numberOfBarrelSector();
       else if (system == Endcap) return numberOfEndcapSector();
       else return numberOfForwardSector();
     }

   inline bool Lvl1MuCTPIInputPhase1::hasOutOfTimeCandidates(size_t systemAddress)  const
     {
       if ( systemAddress <  NumberOfMuonSystem) {
	 return m_isFilledOutOfTimeCandidates[systemAddress];
       } 
       return false;
     }

   inline size_t Lvl1MuCTPIInputPhase1::getSystemIndex(size_t systemAddress,
						       size_t subSystemAddress,
						       size_t sectorAddress ) const
     {
       if (systemAddress==0) {
	 return NumberOfBarrelSector*subSystemAddress+sectorAddress;
       } else if (systemAddress==1) {
	 return NumberOfEndcapSector*subSystemAddress+sectorAddress;
       } else {
	 return NumberOfForwardSector*subSystemAddress+sectorAddress;
       }
     }
   inline size_t Lvl1MuCTPIInputPhase1::getBcidIndex(size_t systemAddress,
						     int    bcid) const
     {
       for( size_t ip=0; ip<m_data[systemAddress].size(); ip++){
         int bc=((m_data[systemAddress]).at(ip)).first;
         if (bc == bcid) return ip;
       }
       return -1;
     }
   
} // namespace LVL1MUONIF

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LVL1MUONIF::Lvl1MuCTPIInputPhase1, 6112, 0 )

#endif // TRIGT1INTERFACES_LVL1MUCTPIINPUTPHASE1_H
