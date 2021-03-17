/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iomanip>
#include <iostream>

#include "TrigT1Interfaces/Lvl1MuForwardSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"

namespace LVL1MUONIF {

  Lvl1MuForwardSectorLogicDataPhase1::Lvl1MuForwardSectorLogicDataPhase1()
      : Lvl1MuSectorLogicDataPhase1() {
    m_id_system = 2;
    initialize();
  }

  Lvl1MuForwardSectorLogicDataPhase1& Lvl1MuForwardSectorLogicDataPhase1::operator=( const Lvl1MuSectorLogicDataPhase1& right ) {
    if ( ( right.getSystemId() == m_id_system ) &&
         ( this != &right ) ) {
      m_bcid = right.bcid();
      m_nsw = right.nsw();
      m_2candidatesInSector = right.is2candidatesInSector();
      for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
        m_roi[ i ] = right.roi( i );
        m_ovl[ i ] = right.ovl( i );
        m_pt[ i ] = right.pt( i );
        m_charge[ i ] = right.charge( i );
        m_2candidates[ i ] = right.is2candidates( i );
        m_goodmf[ i ] = right.goodmf(i);
        m_innercoin[ i ] = right.innercoin(i);
        m_bw2or3[ i ] = right.bw2or3(i);
        m_veto[ i ] = right.veto(i);
      }
    }
    return *this;
  }

  Lvl1MuSectorLogicDataPhase1::SectorLogicWord Lvl1MuForwardSectorLogicDataPhase1::getWordFormat() const {

    SectorLogicWord val(std::make_pair(0, 0));
    unsigned int len=0;
    unsigned int pos=0;
    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      //track current position and length of current data
      len=0;
      pos=0;

      // Candidate 'i' will be offset by 16*i bits
      //0-7: ROI - 8 bits
      len=NBITS_ROI[m_id_system];
      if (roi(i) < 0) val.first |= len << (i*16+pos);
      else val.first |= (roi(i) & len) << (i*16+pos);
      pos += len;

      //8-11: pT - 4 bits
      len=NBITS_PT[m_id_system];
      val.first |= (pt(i) & len) << (i*16+pos);
      pos += len;

      //12: charge
      len=NBITS_CHARGE[m_id_system];
      val.first |= (charge(i) & len) << (i*16+pos);
      pos += len;

      //13: 2 or 3 hits in BW
      len=NBITS_BW2OR3[m_id_system];
      val.first |= (bw2or3(i) & len) << (i*16+pos);
      pos += len;

      //14: inner coincidence
      len=NBITS_CHARGE[m_id_system];
      if (charge(i) > 0) val.first |= (1 >> (i*16+pos)) & len;
      pos += len;

      //15: GoodMF
      len=NBITS_GOODMF[m_id_system];
      val.first |= (goodmf(i) & len) << (i*16+pos);
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
    //12: >1cand/sector
    len=NBITS_IS2CANDIDATESINSECTOR[m_id_system];
    if (is2candidatesInSector()) val.second |= (1 >> pos) & len;
    pos += len;

    //13: nsw monitoring
    len=NBITS_NSWMON[m_id_system];
    if (nsw()) val.second |= (1 >> pos) & len;
    pos += len;


    return val;
  }


  const Lvl1MuSectorLogicDataPhase1& Lvl1MuForwardSectorLogicDataPhase1::convertFromWordFormat( const SectorLogicWord& value ) {

    unsigned int val;
    unsigned int pos=0;
    unsigned int len=0;
    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      //track current position and length of current data
      len=0;
      pos=0;

      // Candidate 'i' will be offset by 16*i bits
      //0-7: ROI - 8 bits
      len=NBITS_ROI[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val == len) roi(i, -1);
      else roi(i, val);
      pos += len;

      //8-11: pT - 4 bits
      len=NBITS_PT[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val == len) pt(i, -1);
      else pt(i, val);
      pos += len;

      //12: charge
      len=NBITS_CHARGE[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      if (val > 0) charge(i, +1);
      else charge(i, -1);
      pos += len;

      //13: 2 or 3 hits in BW
      len=NBITS_BW2OR3[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      bw2or3(i, val);
      pos += len;

      //14: inner coincidence
      len=NBITS_INNERCOIN[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      innercoin(i, val);
      pos += len;

      //15: GoodMF
      len=NBITS_GOODMF[m_id_system];
      val = (value.first >> (i*16+pos)) & len;
      goodmf(i, val);
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

    //13: nsw monitoring
    len=NBITS_NSWMON[m_id_system];
    val = (value.second >> pos) & len;
    nsw(val);

    //14-15: other global flags

    return *this;
  }


  void Lvl1MuForwardSectorLogicDataPhase1::printOut( std::ostream& out ) const {

    for (size_t i=0;i<NCAND[m_id_system];i++)
    {
      out << "Bit 0-7   " << " ROI<" << i+1 << ">                   ";
      if (roi(i) == -1) out << "NA" << std::endl;
      else out << std::setw(8) << roi(i) << std::endl;

      out << "Bit 8-11  "<< "Pt" << i+1 << "                      ";
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

    out << "Bit 13 " << " NSW monitoring ";
    out << std::setw( 8 ) << m_nsw << std::endl;
  }

} // namespace LVL1MUONIF
