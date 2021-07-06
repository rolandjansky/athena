/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorLogicReadOut_H
#define TrigT1RPChardware_SectorLogicReadOut_H

#include <iostream>
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/SectorLogicReadOutStructure.h"
#include "TrigT1RPChardware/Pad.h"
#include "TrigT1RPChardware/crc8.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class SectorLogicReadOut : public BaseObject {

public:
SectorLogicReadOut();
SectorLogicReadOut(const SectorLogicReadOut &SLROOrig);
~SectorLogicReadOut();
void initialize();
void reset();
void overwriteHeader(ubit16 thisRecord){m_Header=thisRecord;};
void overwriteFooter(ubit16 thisRecord){m_Footer=thisRecord;};
void writeRecord(ubit16 newHit, bool last);
ubit16 numberOfFragmentWords() {return m_numberOfWordsInFrag;};
ubit16 numberOfBodyWords() {return m_numberOfWordsInBody;};
ubit16 numberOfHitWords() {return m_numberOfWordsInSLHits;};
ubit16 numberOfCounterWords() {return m_numberOfWordsInCounters;};
void topSLBody() {m_BodyCurr = m_Body;};
void topSLBodyCounters() {m_BodyCurr = m_BodyCounter;};
ubit16 readSLHitCurrent();
ubit16 readSLCounterCurrent();
RODword getCounter32(ubit16 index);
float padTriggerRate(ubit16 padAddress);
//
ubit16 cmadd(ubit16 indexLink, ubit16 indexGate);
ubit16 ptid (ubit16 indexLink, ubit16 indexGate);
ubit16 opl  (ubit16 indexLink, ubit16 indexGate);
ubit16 ovphi(ubit16 indexLink, ubit16 indexGate);
ubit16 oveta(ubit16 indexLink, ubit16 indexGate);
ubit16 res  (ubit16 indexLink, ubit16 indexGate);
ubit16 bcid (ubit16 indexLink, ubit16 indexGate);
void display(std::ostream &stream);
bool checkFragment();
//
//*********************************************************//
// overload of "inseritore" operator                       //
//*********************************************************//
friend std::ostream &operator<<(std::ostream &stream, SectorLogicReadOut *p) {
 p->display(stream);
 return stream;
}
friend std::ostream &operator<<(std::ostream &stream, SectorLogicReadOut &p) {
 p.display(stream);
 return stream;
}
//*********************************************************//

private:
void deleteSLBody();
void doHit();
void doCounter32();
void makeNewHit(ubit16 newHit);

struct SLROData {
 ubit16 hit;
 SLROData *next;
};
ubit16 m_Header;
ubit16 m_Footer;
SLROData *m_Body;
SLROData *m_BodyCounter; // pointer to the SL Counters
ubit16 m_numberOfWordsInFrag;
ubit16 m_numberOfWordsInBody;
ubit16 m_numberOfWordsInCounters;
ubit16 m_numberOfWordsInSLHits;
//*******************************************************//
SLROData *m_BodyLast; // pointer to the last hit recorded
SLROData *m_BodyCurr; // pointer to the current hit scanned
//*******************************************************//
bool m_counter32ok;
bool m_hitok;
static const ubit16 s_nGates                  = 7;
static const ubit16 s_nLinks                  = 8;
ubit16 m_hit[s_nLinks][s_nGates];
static const ubit16 s_numberOfDataCounters    =21;
static const ubit16 s_numberOfDecodedCounters = 7;
ubit16  m_counter16[s_numberOfDataCounters];
RODword m_counter32[s_numberOfDecodedCounters];
//
// 14+21 words
// first 14 words
// nella finestra di lettura SL si da nei 12 bit meno significativi
// la mappa sotto riportatata
// word   tower BCID
//   1      0     0
//   2      1     0
//   3      0     1
//   ..     ..    ..
//  14      1     6
//
// map of counter32
//
// counter32[ 0]  ----> pad 0 trigger data enable=0 counter
// counter32[ 1]  ----> pad 0 trigger data enable=1 counter
// counter32[ 2]  ----> pad 1 trigger data enable=0 counter
// counter32[ 3]  ----> pad 1 trigger data enable=1 counter
// ...
// counter32[n-3]  ----> sector logic valid trigger counter
// counter32[n-2]  ----> selected lvl1a counter
// counter32[n-1]  ----> lost lvl1a counter 
//
//
// map of the hit section of the SectorLogic Body
//
// bit from  0  to  1   --> cmadd
// bit from  2  to  4   --> ptcode
// bit from  5  to  5   --> outer plane flag
// bit from  6  to  6   --> overlap-in-phi flag
// bit from  7  to  7   --> overlap-in-eta flag
// bit from  8  to  8   --> (reserved)
// bit from  9  to 11   --> bcid
//
};
#endif
