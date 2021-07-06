/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorLogicRXReadOut_H
#define TrigT1RPChardware_SectorLogicRXReadOut_H

#include <iostream>
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/SectorLogicRXReadOutStructure.h"
#include "TrigT1RPChardware/Pad.h"
#include "TrigT1RPChardware/crc8.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class SectorLogicRXReadOut : public BaseObject {

public:

  SectorLogicRXReadOut();
  ~SectorLogicRXReadOut();
  void initialize();
  void reset();
 
  void writeRecord(ubit16 thisRecord, bool last);

  uint16_t numberOfFragmentWords() {return m_numberOfWordsInFrag;};
  uint16_t numberOfInputWords() {return m_numberOfInputWords;};
  
  void topSLBody() {m_BodyCurr = m_BodyFirst;};
  uint16_t readSLHitCurrent();
  //
  void display(std::ostream &stream);
  //bool checkFragment();
//
//*********************************************************//
// overload of "inseritore" operator                       //
//*********************************************************//
  friend std::ostream &operator<<(std::ostream &stream, SectorLogicRXReadOut *p) {
    p->display(stream);
    return stream;
  }
  friend std::ostream &operator<<(std::ostream &stream, SectorLogicRXReadOut &p) {
    p.display(stream);
    return stream;
  }
//*********************************************************//

private:

 void deleteSLBody();
 void makeNewHit(ubit16 newHit);
 
 struct SLRXROData {
   uint16_t hit;
   SLRXROData *next;
 };
 
 uint16_t m_Header;
 uint16_t m_SubHeader;
 uint16_t m_Footer;
 SLRXROData *m_BodyFirst;
 uint16_t m_numberOfWordsInFrag;
 uint16_t m_numberOfInputWords;
 
 //*******************************************************//
 SLRXROData *m_BodyLast; // pointer to the last hit recorded
 SLRXROData *m_BodyCurr; // pointer to the current hit scanned
 //*******************************************************//
// uint16_t m_lastHit;

};
#endif
