/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_MatrixReadOut_H
#define TrigT1RPChardware_MatrixReadOut_H

#include <iostream>
#include "TrigT1RPChardware/Matrix.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/crc8.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class ReadOutManager;

class MatrixReadOut : public BaseObject {

public:
enum DataVersion {Simulation, Atlas};

MatrixReadOut(Matrix *p, ubit16 FEevent, DataVersion=MatrixReadOut::Atlas);
MatrixReadOut(ubit16 FEevent=0, DataVersion=MatrixReadOut::Atlas);
MatrixReadOut(ubit16 *v, ubit16 numWords, DataVersion=MatrixReadOut::Atlas);
MatrixReadOut(const MatrixReadOut &MROOrig); // copy constructor
MatrixReadOut& operator=(const MatrixReadOut &MROOrig) = delete;
~MatrixReadOut();
MatrixReadOutStructure getCMAHit(int index);
MatrixReadOutStructure getHeader();
MatrixReadOutStructure getSubHeader();
MatrixReadOutStructure getFooter();
void deleteCMABody();
void reset();
//***********************************************//
// following methods are used by Lvl1RpcTrig     //
//***********************************************//

void writeHeader(ubit16 CMcode);
void writeSubHeader();
void writeCMABody(ubit16 _BC, ubit16 _TIME, ubit16 IJK, ubit16 _STRIP);
void writeFooter();
void writeRecord(ubit16 thisRecord, bool last);
void overwriteHeader(ubit16 thisRecord){m_Header=thisRecord;};      //new method
void overwriteSubHeader(ubit16 thisRecord){m_SubHeader=thisRecord;};//new method
void reComputeFooter(){writeFooter();m_checkFooterNum--;};            //new method
ubit16 numberOfBodyWords() {return m_numberOfWordsInBody;};
ubit16 numberOfFragmentWords() {return m_numberOfWordsInFrag;};
//**************************************************//
// general user methods to read back ReadOut stream //
//**************************************************//
ubit16 readHeader();
ubit16 readSubHeader();
ubit16 readCMABodyCurrent();
void   readCMABody(ubit16 *Body);
ubit16 readFooter();
ubit16 readCMAWord();
ubit16 checkCRC8(ubit16 foot);
void topCMABody();
//*********************************************************//
// general user methods to display/print ReadOut stream    //
//*********************************************************//
void display(std::ostream &stream);
void displayHeader(std::ostream &stream);
void displaySubHeader(std::ostream &stream);
void displayBody(std::ostream &stream);
void displayFooter(std::ostream &stream);
void bytestream(std::ostream &stream);
//*********************************************************//
// general user methods to check ReadOut stream structure  //
//*********************************************************//
ubit16 checkBodyOrder();
ubit16 checkBodyOrderObsolete();
ubit16 checkFragment();
//*********************************************************//
// overload of "inseritore" operator                       //
//*********************************************************//
friend std::ostream &operator<<(std::ostream &stream, MatrixReadOut *p) { 
 p->display(stream);
 return stream;
}
friend std::ostream &operator<<(std::ostream &stream, MatrixReadOut &p) { 
 p.display(stream);
 return stream;
}
//*********************************************************//
void doMatrix(Matrix *CMpointer);
void makeTestPattern(ubit16 mode, ubit16 ktimes, int eventNum);

private:
//fp

 friend class ReadOutManager;
 ReadOutManager *m_myBoss;
 void setManager(ReadOutManager *boss);
 void makeFragment();

MatrixReadOut::DataVersion m_data_version;

MatrixReadOutStructure m_MROS;
ubit16 m_BunchFrom;
ubit16 m_BunchTo;
ubit16 m_FEL1ID;
ubit16 m_ROOffset;
ubit16 m_NDLLCYC;
ubit16 m_NBunch;
ubit16 m_nclock;
ubit16 m_nchan[2];
ubit16 m_timeSeparation;
Matrix *m_CM; // pointer to the CM
ubit16 *m_BS; // pointer to Fragment
//**************************************//
//                                      //
//    Matrix ReadOut structure          //
//                                      //
//**************************************//
struct CMROData {
 ubit16 hit;
 CMROData *next;
};
ubit16  m_Header;     // Header record
ubit16  m_Footer;     // Footer record
ubit16  m_SubHeader;  // Subheader record
CMROData *m_Body;     // Body structure record
//**********************************************************//
ubit16 m_addressOfWordScanned;
CMROData *m_BodyLast; // pointer to the last hit recorded
CMROData *m_BodyCurr; // pointer to the current hit scanned
//**********************************************************//
ubit16 m_first8bitsON;
ubit16 m_numberOfWordsInFrag;
ubit16 m_numberOfWordsInBody;
ubit16 m_checkHeaderPos;
ubit16 m_checkHeaderNum;
ubit16 m_checkSubHeaderPos;
ubit16 m_checkSubHeaderNum;
ubit16 m_checkFooterPos;
ubit16 m_checkFooterNum;
ubit16 m_checkCR;
ubit16 m_checkUnkown;
//************************//
void initialize();
void makeHeader();
void makeSubHeader();
void makeCMABody();
void makeFooter();
ubit16 makeCMABodyHit();
ubit16 makeCMABodyTrg();
ubit16 makeCMABodyTrgObsolete();
void makeNewHit(ubit16 newHit);
void makeNewHit(ubit16 newHit, CMROData *previous, CMROData *next);
void sortAndMakeNewHit(ubit16 newHit);
ubit16 computeCR();
};
#endif
