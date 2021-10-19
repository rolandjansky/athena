/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


//////////////////////////////////////////////////////////////////////
// Coding of the RDO
//////////////////////////////////////////////////////////////////////
// word0: LUCID-A - PMTs
// bits  0- 7: Connector A EPMC1 bits 0-7 (ordered)
// bits  8-15: Connector B EPMC1 bits 0-7 (ordered)
// bits 16-23: Connector C EPMC1 bits 0-7 (not used, always 0)
// bits 24-32: numer of bits set to 1: Counting of fired PMT side A
// word1: LUCID-C - PMTs
// bits  0- 7: Connector D EPMC1 bits 0-7 (ordered)
// bits  8-15: Connector E EPMC1 bits 0-7 (ordered)
// bits 16-23: Connector F EPMC1 bits 0-7 (not used, always 0)
// bits 24-32: numer of bits set to 1: Counting of fired PMT side C
// word2: LUCID-A - FIBERs
// bits  0- 7: Connector A EPMC1 bits 0-7 (ordered)
// bits  8-15: Connector B EPMC1 bits 0-7 (not used, always 0)
// bits 16-23: Connector C EPMC1 bits 0-7 (not used, always 0)
// bits 24-32: numer of bits set to 1: Counting of fired FIBERS side A
// word3: LUCID-C - FIBERs
// bits  0- 7: Connector D EPMC1 bits 0-7 (ordered)
// bits  8-15: Connector E EPMC1 bits 0-7 (not used, always 0)
// bits 16-23: Connector F EPMC1 bits 0-7 (not used, always 0)
// bits 24-32: numer of bits set to 1: Counting of fired FIBERS side C
//////////////////////////////////////////////////////////////////////

#ifndef LUCID_RAWDATA_H
#define LUCID_RAWDATA_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <sstream>

class LUCID_RawData {
  
 public:
  
  LUCID_RawData();
  LUCID_RawData(const unsigned int word0,
		const unsigned int word1,
		const unsigned int word2,
		const unsigned int word3,
		const unsigned int word0p,
		const unsigned int word1p,
		const unsigned int word2p,
		const unsigned int word3p,
		const unsigned int word0n,
		const unsigned int word1n,
		const unsigned int word2n,
		const unsigned int word3n,
		const unsigned int status);

  LUCID_RawData(std::vector<uint32_t>& data_block);
  
  virtual ~LUCID_RawData();
  
  inline int getWord0() const { return m_word0; }
  inline int getWord1() const { return m_word1; }
  inline int getWord2() const { return m_word2; }
  inline int getWord3() const { return m_word3; }

  inline int getWord0p() const { return m_word0p; }
  inline int getWord1p() const { return m_word1p; }
  inline int getWord2p() const { return m_word2p; }
  inline int getWord3p() const { return m_word3p; }

  inline int getWord0n() const { return m_word0n; }
  inline int getWord1n() const { return m_word1n; }
  inline int getWord2n() const { return m_word2n; }
  inline int getWord3n() const { return m_word3n; }

  inline int getStatus() const { return m_status; }
  
  inline void setWord0(uint32_t word) { m_word0 = word; }
  inline void setWord1(uint32_t word) { m_word1 = word; }
  inline void setWord2(uint32_t word) { m_word2 = word; }
  inline void setWord3(uint32_t word) { m_word3 = word; }

  inline void setWord0p(uint32_t word) { m_word0p = word; }
  inline void setWord1p(uint32_t word) { m_word1p = word; }
  inline void setWord2p(uint32_t word) { m_word2p = word; }
  inline void setWord3p(uint32_t word) { m_word3p = word; }

  inline void setWord0n(uint32_t word) { m_word0n = word; }
  inline void setWord1n(uint32_t word) { m_word1n = word; }
  inline void setWord2n(uint32_t word) { m_word2n = word; }
  inline void setWord3n(uint32_t word) { m_word3n = word; }

  inline void setStatus(uint32_t status) { m_status = status; }
  
  inline std::vector<uint32_t> getDataBlock() const {
    
    std::vector<uint32_t> returnDB;
    
    returnDB.push_back(m_word0);
    returnDB.push_back(m_word1);
    returnDB.push_back(m_word2);
    returnDB.push_back(m_word3);
    
    return returnDB;
  }
  
  void encodeLumatMapping  (void);
  void decodeLumatMapping  (void);
  static int  getLumatBitPosition (unsigned int tubeNumber) ;
  bool isTubeFired         (unsigned int tubeNumber, unsigned int bxID) const;

  inline int getNhitsPMTsideA      () const { return (m_word0>>24); }
  inline int getNhitsPMTsideC      () const { return (m_word1>>24); }
  inline int getNhitsFIBsideA      () const { return (m_word2>>24); }
  inline int getNhitsFIBsideC      () const { return (m_word3>>24); }
  
  inline int getNhitsPMTsideAprevBX() const { return (m_word0p>>24); }
  inline int getNhitsPMTsideCprevBX() const { return (m_word1p>>24); }
  inline int getNhitsFIBsideAprevBX() const { return (m_word2p>>24); }
  inline int getNhitsFIBsideCprevBX() const { return (m_word3p>>24); }
  
  inline int getNhitsPMTsideAnextBX() const { return (m_word0n>>24); }
  inline int getNhitsPMTsideCnextBX() const { return (m_word1n>>24); }
  inline int getNhitsFIBsideAnextBX() const { return (m_word2n>>24); }
  inline int getNhitsFIBsideCnextBX() const { return (m_word3n>>24); }

  std::string str() const {
  
    std::ostringstream ost;

    ost << std::endl 
	<< std::hex
	<< " word0 : " << m_word0  << std::endl
	<< " word1 : " << m_word1  << std::endl
	<< " word2 : " << m_word2  << std::endl
	<< " word3 : " << m_word3  << std::endl
	<< " word0p: " << m_word0p << std::endl
	<< " word1p: " << m_word1p << std::endl
	<< " word2p: " << m_word2p << std::endl
	<< " word3p: " << m_word3p << std::endl
	<< " word0n: " << m_word0n << std::endl
	<< " word1n: " << m_word1n << std::endl
	<< " word2n: " << m_word2n << std::endl
	<< " word3n: " << m_word3n << std::endl
	<< " status: " << m_status << std::endl
	<< std::dec
	<< " hitsPMTsideA: " << getNhitsPMTsideA() << std::endl
	<< " hitsPMTsideC: " << getNhitsPMTsideC() << std::endl
	<< " hitsFIBsideA: " << getNhitsFIBsideA() << std::endl
	<< " hitsFIBsideC: " << getNhitsFIBsideC() << std::endl
	<< " hitsPMTsideAprevBX: " << getNhitsPMTsideAprevBX() << std::endl
	<< " hitsPMTsideCprevBX: " << getNhitsPMTsideCprevBX() << std::endl
	<< " hitsFIBsideAprevBX: " << getNhitsFIBsideAprevBX() << std::endl
	<< " hitsFIBsideCprevBX: " << getNhitsFIBsideCprevBX() << std::endl
	<< " hitsPMTsideAnextBX: " << getNhitsPMTsideAnextBX() << std::endl
	<< " hitsPMTsideCnextBX: " << getNhitsPMTsideCnextBX() << std::endl
	<< " hitsFIBsideAnextBX: " << getNhitsFIBsideAnextBX() << std::endl
	<< " hitsFIBsideCnextBX: " << getNhitsFIBsideCnextBX() << std::endl;
    
    return ost.str();
  }

  std::ostream& operator<<(std::ostream& os) { os << str(); return os; }

 private:
  
  unsigned int m_word0;  // central BX
  unsigned int m_word1;  // central BX
  unsigned int m_word2;  // central BX
  unsigned int m_word3;  // central BX

  unsigned int m_word0p; // prevoius BX
  unsigned int m_word1p; // prevoius BX
  unsigned int m_word2p; // prevoius BX
  unsigned int m_word3p; // prevoius BX

  unsigned int m_word0n; // next BX
  unsigned int m_word1n; // next BX
  unsigned int m_word2n; // next BX
  unsigned int m_word3n; // next BX

  unsigned int m_status;
};

#endif // LUCID_RAWDATA_H
