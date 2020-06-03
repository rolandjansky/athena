// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_LVL1MUSECTORLOGICDATA_H
#define TRIGT1INTERFACES_LVL1MUSECTORLOGICDATA_H

// System include(s):
#include <iosfwd>

/**
 *  @short Namespace for the LVL1 muon interface classes
 *
 *         The classes used in the communication between the different parts
 *         of the LVL1 muon trigger simulation are put into this namespace.
 *
 * $Revision: 187728 $
 * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
 */
namespace LVL1MUONIF {

   /**
    *  @short Base class for the data coming from one SL board
    *
    *         Each muon SL board sends 32 bits of information to the MuCTPI.
    *         While the format of these 32 bits is a bit different for the
    *         different sector types, the information stored in them is the
    *         same. So the derived classes only have to implement the functions
    *         decoding the 32-bit words into the variables in this class.
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class Lvl1MuSectorLogicData {

   public:
      Lvl1MuSectorLogicData();
      Lvl1MuSectorLogicData( const Lvl1MuSectorLogicData& right );
      virtual ~Lvl1MuSectorLogicData() {}

      Lvl1MuSectorLogicData& operator=( const Lvl1MuSectorLogicData& right );

      // assume int is 4byte:
      typedef unsigned int SectorLogicWord;

      bool is2candidatesInSector() const { return m_2candidatesInSector; }
      int bcid() const { return m_bcid;  }
      int roi( size_t id ) const { return m_roi[ id ]; }
      int ovl( size_t id ) const { return m_ovl[ id ]; }
      int pt( size_t id ) const { return m_pt[ id ]; }
      int charge( size_t id ) const { return m_charge[ id ]; }
      bool is2candidates( size_t id ) const { return m_2candidates[ id ]; }

      void set2candidatesInSector() { m_2candidatesInSector = true; }
      void clear2candidatesInSector() { m_2candidatesInSector = false;}
      void bcid( int value ) { m_bcid = value; }
      void roi( size_t id, int value ) { m_roi[ id ] = value; }
      void ovl( size_t id, int value ) { m_ovl[ id ] = value; }
      void pt( size_t id, int value ) { m_pt[ id ] = value; }
      void charge( size_t id, int value ) { m_charge[ id ] = value; }
      void set2candidates( size_t id ) { m_2candidates[ id ] = true; }
      void clear2candidates( size_t id ) { m_2candidates[ id ] = false; }

      void clear();

      friend std::ostream& operator<<( std::ostream&, const Lvl1MuSectorLogicData& );

      /// Get data in 32 bit word format:
      virtual SectorLogicWord getWordFormat() const = 0;
      /// Convert SectorLogicData from 32 bit word data
      virtual const Lvl1MuSectorLogicData& convertFromWordFormat( const SectorLogicWord& ) = 0;

      int getSystemId() const { return m_id_system; }

   protected:
      /// print out data
      virtual void printOut( std::ostream& out ) const;

      bool m_2candidatesInSector;
      int m_roi[ 2 ];
      int m_ovl[ 2 ];
      int m_pt[ 2 ];
      bool m_2candidates[ 2 ];
      int m_bcid;
      int m_charge[ 2 ];
      int m_id_system;

   }; // class Lvl1MuSectorLogicData

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUSECTORLOGICDATA_H
