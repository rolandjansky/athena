/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RXReadOut_H
#define TrigT1RPChardware_RXReadOut_H

#include<iostream>
#include "TrigT1RPChardware/PadReadOut.h"
#include "TrigT1RPChardware/RXReadOutStructure.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class RXReadOut : public BaseObject {

public:
RXReadOut(ubit16 sectorID, PadReadOut **PRvector);
~RXReadOut();
void reset();
void makeFragment();
void makeHeader();
void makeFooter();
void makeBody();
ubit16 readHeader(); 
ubit16 readFooter();
ubit16 readBody();
void   topRXBody(); 
ubit16 readRXWord();
RXReadOutStructure getHeader();
RXReadOutStructure getFooter();
ubit16 numberOfFragmentWords (){return m_numberOfWordsInFragment;};
void bytestream(std::ostream &stream);
//void decodeBytestream();

private:
//
static const ubit16 s_numberOfPads=8;
//
// control flags for Body scanninng ...
//
ubit16 m_addressOfWordScanned;
ubit16 m_numberOfPDFragments;
ubit16 m_numberOfWordsInFragment;
ubit16 m_newPDIndex;
ubit16 m_newPDRO;
ubit16 m_numberOfWordsRead;
ubit16 m_numberOfWordsInPDRO;
ubit16 m_endOfPDFragments;
PadReadOut *m_currentPDRO;
//
//
RXReadOutStructure m_RROS;
PadReadOut **m_PROlist;
ubit16 m_sectorID;
ubit16 m_Header;
PadReadOut *m_PADFragment[8];
ubit16 m_Footer;
};
#endif
