/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATA_P0_H
#define MUON_TGCRAWDATA_P0_H

/*
Persistent representation of the old TgcRawData class
Used by ROOT streamer
Author: Marcin Nowak
        CERN, January 2006
*/


class TgcRawData_p0
{
public:
  TgcRawData_p0() :
    m_bcTag(0), 
    m_subDetectorId(0), m_rodId(0), m_sswId(0), m_slbId(0), 
    m_l1Id(0), m_bcId(0), 
    m_bitpos(0), 
    m_isCoincidence(false), 
    m_type(0), 
    m_index(0), 
    m_pos(0), m_delta(0)
    {}
  virtual ~TgcRawData_p0() {}

  
   // return Information Type Hit:Coincidence
  bool isCoincidence() const {return m_isCoincidence;}

  /*********** common accessors
   */
  uint16_t bcTag() const {return m_bcTag;}
  uint16_t l1Id()  const {return m_l1Id;}
  uint16_t bcId()  const {return m_bcId;}

  uint16_t subDetectorId() const {return m_subDetectorId;}
  uint16_t rodId()         const {return m_rodId;}
  uint16_t sswId()         const {return m_sswId;}
  uint16_t slbId()         const {return m_slbId;}


  /*********** accessors for Hit
   */
  uint16_t bitpos() const {return m_bitpos;}


  /*********** accessors for Trigger Coincidence
   */
  uint16_t type()     const {return m_type;}
  uint16_t index()    const {return m_index;}
  uint16_t position() const {return m_pos;}
  uint16_t delta()    const {return m_delta;}

private:

  /********** common attributes
   */

  // BC tag (Central, Next, Previous)
  uint16_t m_bcTag;

  // online Identifiers
  uint16_t m_subDetectorId;
  uint16_t m_rodId;
  uint16_t m_sswId;
  uint16_t m_slbId;

  // BCID and L1ID on SLB
  uint16_t m_l1Id;
  uint16_t m_bcId;


  /********** Hit attributes
   */

  // bitmap position [40..199]
  uint16_t m_bitpos;


  /********** Coincidence attributes
   */

  // flag for Coincidence-information
  bool m_isCoincidence;

  //  Coincidence type (L-Pt, H-Pt, SL)
  uint16_t m_type;

  // Coincidence index
  uint16_t m_index;

  // Coincidence Data
  uint16_t m_pos;
  uint16_t m_delta; 

  
};

#endif













