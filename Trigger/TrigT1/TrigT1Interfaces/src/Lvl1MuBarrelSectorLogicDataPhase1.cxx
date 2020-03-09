/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iomanip>
#include <iostream>

#include "TrigT1Interfaces/Lvl1MuBarrelSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"

namespace LVL1MUONIF {

  Lvl1MuBarrelSectorLogicDataPhase1::Lvl1MuBarrelSectorLogicDataPhase1()
      : Lvl1MuSectorLogicDataPhase1() {

    m_id_system = 0;
    initialize();
  }

  Lvl1MuBarrelSectorLogicDataPhase1& Lvl1MuBarrelSectorLogicDataPhase1::operator=( const Lvl1MuSectorLogicDataPhase1& right ) {

    if ( ( right.getSystemId() == m_id_system ) &&
         ( this != &right ) ) {

      m_bcid = right.bcid();
      m_2candidatesInSector = right.is2candidatesInSector();
      for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
        m_roi[ i ] = right.roi( i );
        m_ovl[ i ] = right.ovl( i );
        m_pt[ i ] = right.pt( i );
        m_charge[ i ] = right.charge( i );
        m_2candidates[ i ] = right.is2candidates( i );
      }
    }

    return *this;
  }

  Lvl1MuSectorLogicDataPhase1::SectorLogicWord Lvl1MuBarrelSectorLogicDataPhase1::getWordFormat() const {

    SectorLogicWord val(std::make_pair(0, 0));
    unsigned int len=0;
    unsigned int pos=0;
    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      //track current position and length of current data
      len=0;
      pos=0;
      bool noROI=0;

      // Candidate 'i' will be offset by 16*i bits
      //0-4: ROI - 5 bits
      len=NBITS_ROI[m_id_system];
      if (roi(i) < 0) 
      {
	val.first |= len << (i*16+pos);
	noROI=1;
      }
      else val.first |= (roi(i) & len) << (i*16+pos);
      pos += len;

      //5-7: Nothing - 3 bits
      len=3;
      pos += len;

      //8-10: pT - 3 bits
      len=NBITS_PT[m_id_system];
      if (pt(i) < 0) val.first |= len << (i*16+pos);
      else val.first |= (pt(i) & len) << (i*16+pos);
      pos += len;

      //11: Nothing - 1 bit
      len=1;
      pos += len;

      //12: 2 candidates in ROI
      len=NBITS_IS2CANDIDATES[m_id_system];
      val.first |= (is2candidates(i) & len) << (i*16+pos);
      pos += len;

      //13-14: overlap
      len=NBITS_OVL[m_id_system];
      if (!noROI || ovl(i) < 0) val.first |= len << (i*16+pos);
      else val.first |= (ovl(i) & len) << (i*16+pos);
      pos += len;

      //15: charge?
      len=NBITS_CHARGE[m_id_system];
      if (charge(i) > 0) val.first |= (1 >> (i*16+pos)) & len;
      pos += len;
    }



    // Global+BCID flags (bits 32-47)
    len=0;
    pos=0;

    //0-11: BCID
    len=NBITS_BCID[m_id_system];
    val.second |= ( bcid() & len);
    pos += len;

    //12-15: global flags
    len=NBITS_IS2CANDIDATESINSECTOR[m_id_system];
    if (is2candidatesInSector()) val.second |= (1 >> pos) & len;


    return val;
  }


  const Lvl1MuSectorLogicDataPhase1& Lvl1MuBarrelSectorLogicDataPhase1::convertFromWordFormat( const SectorLogicWord& value ) {

    unsigned int val;
    unsigned int pos=0;
    unsigned int len=0;
    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      //track current position and length of current data
      len=0;
      pos=0;
      bool noROI=0;

      // Candidate 'i' will be offset by 16*i bits
      //0-4: ROI - 5 bits
      len=NBITS_ROI[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val == len) 
      {
	noROI=1;
	roi(i, -1);
	ovl(i, -1);
      }
      else roi(i, val);
      pos += len;

      //5-7: Nothing - 3 bits
      len=3;
      pos += len;

      //8-10: pT - 3 bits
      len=NBITS_PT[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val == len) pt(i, -1);
      else pt(i, val);
      pos += len;

      //11: Nothing - 1 bit
      len=1;
      pos += len;

      //12: 2 candidates in ROI
      len=NBITS_IS2CANDIDATES[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val == len) set2candidates(i);
      else clear2candidates(i);
      pos += len;

      //13-14: overlap
      len=NBITS_OVL[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (!noROI) ovl(i, val);
      pos += len;

      //15: charge?
      len=NBITS_CHARGE[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val > 0) charge(i, +1);
      else charge(i, -1); // should this be -1 or 0?
      pos += len;
    }

    // Global+BCID flags (bits 32-47)
    len=0;
    pos=0;

    //0-11: BCID
    len=NBITS_BCID[m_id_system];    
    val = value.second & len;
    bcid(val);
    pos += len;

    //12: > 2 candidates
    len=NBITS_IS2CANDIDATESINSECTOR[m_id_system];
    val = (value.second >> pos) & len;
    if (val) set2candidatesInSector();
    else clear2candidatesInSector();
    pos += len;

    //13-15: other global flags


    return *this;
  }


  void Lvl1MuBarrelSectorLogicDataPhase1::printOut( std::ostream& out ) const {

    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      out << "Bit 0-4   " << " ROI<" << i+1 << ">                   ";
      if (roi(i) == -1) out << "NA" << std::endl;
      else out << std::setw(8) << roi(i) << std::endl;

      out << "Bit 8-10  "<< "Pt" << i+1 << "                      ";
      if (pt(i) == -1) out << "No Hit" << std::endl;
      else out << pt(i) << std::endl;

      out << "Bit 12    "<<"2 candidates in ROI<" << i+1 << "> ";
      out << std::setw(8) << is2candidates(i) << std::endl;

      out << "Bit 13-14 "<<"overlap<"<<i+1<<"> ";
      out << std::setw(8) << ovl(i) << std::endl;

      out << "Bit 15    "<<"charge<"<<i+1<<"> ";
      out << std::setw(8) << charge(i) << std::endl;
    }
    out << "Bit 0-11 " << " BCID                     ";
    out << std::setw( 8 ) << bcid() << std::endl;

    out << "Bit 12 " << " >2candidates in a sector ";
    out << std::setw( 8 ) << m_2candidatesInSector << std::endl;

  }

} // namespace LVL1MUONIF
