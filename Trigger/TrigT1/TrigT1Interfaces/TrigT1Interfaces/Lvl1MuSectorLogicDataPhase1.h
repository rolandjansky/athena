/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_LVL1MUSECTORLOGICDATAPHASE1_H
#define TRIGT1INTERFACES_LVL1MUSECTORLOGICDATAPHASE1_H

// System include(s):
#include <iosfwd>
#include <utility>
#include <vector>

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
   class Lvl1MuSectorLogicDataPhase1 {

   public:
      Lvl1MuSectorLogicDataPhase1();
      Lvl1MuSectorLogicDataPhase1( const Lvl1MuSectorLogicDataPhase1& right );
      virtual ~Lvl1MuSectorLogicDataPhase1();

      Lvl1MuSectorLogicDataPhase1& operator=( const Lvl1MuSectorLogicDataPhase1& right );

      // assume unsigned long int is 8 bytes, unsigned short int is 2 bytes:
      typedef std::pair<unsigned long int, unsigned short int> SectorLogicWord;

      bool is2candidatesInSector() const { return m_2candidatesInSector; } // MORE than two candidates per sector
      int bcid() const { return m_bcid;  }
      int nsw() const {return m_nsw; } // NSW monitoring bit
      int roi( size_t id ) const { return m_roi[ id ]; }
      int ovl( size_t id ) const { return m_ovl[ id ]; }
      int pt( size_t id ) const { return m_pt[ id ]; }
      int charge( size_t id ) const { return m_charge[ id ]; }
      bool is2candidates( size_t id ) const { return m_2candidates[ id ]; } // per ROI (not used for mult counting)
      int goodmf( size_t id) const { return m_goodmf[ id ]; }
      int innercoin( size_t id) const { return m_innercoin[ id ]; }
      int bw2or3( size_t id) const { return m_bw2or3[ id ]; }
      int veto( size_t id) const {return m_veto[ id ]; } // veto candidate for multiplicity counting due to overlap removal

      void set2candidatesInSector() { m_2candidatesInSector = true; }
      void clear2candidatesInSector() { m_2candidatesInSector = false;}
      void bcid( int value ) { m_bcid = value; }
      void nsw( int value) {m_nsw = value; }
      void roi( size_t id, int value ) { m_roi[ id ] = value; }
      void ovl( size_t id, int value ) { m_ovl[ id ] = value; }
      void pt( size_t id, int value ) { m_pt[ id ] = value; }
      void charge( size_t id, int value ) { m_charge[ id ] = value; }
      void set2candidates( size_t id ) { m_2candidates[ id ] = true; }
      void clear2candidates( size_t id ) { m_2candidates[ id ] = false; }
      void goodmf( size_t id, int value) {m_goodmf[ id ] = value; }
      void innercoin( size_t id, int value) {m_innercoin[ id ] = value; }
      void bw2or3( size_t id, int value) {m_bw2or3[ id ] = value; }
      void veto( size_t id, int value) {m_veto[ id ] = value; }

      void clear();

      friend std::ostream& operator<<( std::ostream&, const Lvl1MuSectorLogicDataPhase1& );

      /// Get data in 32 bit word format:
      virtual SectorLogicWord getWordFormat() const = 0;
      /// Convert SectorLogicDataPhase1 from 32 bit word data
      virtual const Lvl1MuSectorLogicDataPhase1& convertFromWordFormat( const SectorLogicWord& ) = 0;

      int getSystemId() const { return m_id_system; }

   protected:
      /// print out data
      virtual void printOut( std::ostream& out ) const=0;
      void initialize();

      bool m_2candidatesInSector;
      std::vector<int> m_roi; //[m_ncand]
      std::vector<int> m_ovl; //[m_ncand]
      std::vector<int> m_pt; //[m_ncand]
      std::vector<bool> m_2candidates; //[m_ncand]
      int m_bcid;
      int m_nsw;
      std::vector<int> m_charge; //[m_ncand]
      int m_id_system;
      std::vector<int> m_goodmf; //[m_ncand]
      std::vector<int> m_innercoin; //[m_ncand]
      std::vector<int> m_bw2or3; //[m_ncand]
      std::vector<int> m_veto; //[m_ncand]

   }; // class Lvl1MuSectorLogicDataPhase1

} // namespace LVL1MUONIF

#endif // TRIGT1INTERFACES_LVL1MUSECTORLOGICDATAPHASE1_H
