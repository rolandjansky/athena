/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PadReadOut_H
#define PadReadOut_H

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
ubit16 numberOfFragmentWords (){return numberOfWordsInFragment;};
ubit16 numberOfCMROFragments(){return numberOfCMFragments;};
void bytestream(std::ostream &stream);
void decodeBytestream();
PadReadOutStructure getHeader();
PadReadOutStructure getFooter();

private:
//
// control flags for Body scanninng ...
//
ubit16 addressOfWordScanned;
ubit16 numberOfCMFragments;
ubit16 numberOfWordsInFragment;
ubit16 newCMIndex;
ubit16 newCMRO;
ubit16 numberOfWordsRead;
ubit16 numberOfWordsInCMRO;
ubit16 endOfCMFragments;
MatrixReadOut *currentCMRO;
//
//
PadReadOutStructure PROS;
MatrixReadOut *m_MROlist[8];
ubit16 m_padID;
ubit16        m_Header;
MatrixReadOut *m_CMAFragment[8];
ubit16        m_Footer;
};
#endif
