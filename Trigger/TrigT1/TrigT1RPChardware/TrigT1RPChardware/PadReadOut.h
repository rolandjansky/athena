/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_PadReadOut_H
#define TrigT1RPChardware_PadReadOut_H

#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/PadReadOutStructure.h"
#include "TrigT1RPChardware/Pad.h"
#include "TrigT1RPChardware/crc8.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class PadReadOut : public BaseObject {

public:
PadReadOut(ubit16 padID, MatrixReadOut *MROlist[8]);
~PadReadOut();
void reset();
void makeFragment();
void makeHeader();
void makeFooter();
void makeBody();
ubit16 readHeader(); 
ubit16 readFooter();
ubit16 readBody();
void   topPADBody(); 
ubit16 readPADWord();
ubit16 numberOfFragmentWords (){return m_numberOfWordsInFragment;};
ubit16 numberOfCMROFragments(){return m_numberOfCMFragments;};
void bytestream(std::ostream &stream);
void decodeBytestream();
PadReadOutStructure getHeader();
PadReadOutStructure getFooter();

private:
//
// control flags for Body scanninng ...
//
ubit16 m_addressOfWordScanned;
ubit16 m_numberOfCMFragments;
ubit16 m_numberOfWordsInFragment;
ubit16 m_newCMIndex;
ubit16 m_newCMRO;
ubit16 m_numberOfWordsRead;
ubit16 m_numberOfWordsInCMRO;
ubit16 m_endOfCMFragments;
MatrixReadOut *m_currentCMRO;
//
//
PadReadOutStructure m_PROS;
MatrixReadOut *m_MROlist[8];
ubit16 m_padID;
ubit16        m_Header;
MatrixReadOut *m_CMAFragment[8];
ubit16        m_Footer;
};
#endif
