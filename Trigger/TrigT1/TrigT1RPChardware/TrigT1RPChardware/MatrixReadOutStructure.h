/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_MatrixReadOutStructure_H
#define TrigT1RPChardware_MatrixReadOutStructure_H

#include "TrigT1RPChardware/RPCReadOut.h"
#include "TrigT1RPChardware/crc8.h"
#include "TrigT1RPChardware/Lvl1Def.h"

//class RPCReadOut;
class MatrixReadOutStructure : public RPCReadOut {
 public:
  MatrixReadOutStructure();
  MatrixReadOutStructure(ubit16 inputData);
  ~MatrixReadOutStructure() = default;
  void setInit();
//
  virtual ubit16 makeHeader(ubit16 *inputData);
  virtual ubit16 makeSubHeader();
  virtual ubit16 makeBody(ubit16 *inputData);
  virtual ubit16 makeFooter(ubit16 inputData);
  virtual ubit16 makeFooter(ubit16 *inputData);
//
  //**************************************************//
  // general user methods to decode ReadOut stream    //
  //**************************************************//
  ubit16 decodeFragment(ubit16 inputWord, char &field);
  ubit16 cmid()      {return m_cmid;};
  ubit16 fel1id()    {return m_fel1id;};
  ubit16 febcid()    {return m_febcid;};
  ubit16 bcid()      {return m_bcid;};
  ubit16 time()      {return m_time;};
  ubit16 ijk()       {return m_ijk;};
  ubit16 channel()   {return m_channel;}; //0-32
  ubit16 threshold() {return m_threshold;};
  ubit16 overlap()   {return m_overlap;};
  ubit16 status()    {return m_status;};
  ubit16 crc()       {return m_crc;};
//
  bool isLowPt();
  bool isHigPt();
  bool isEtaProj();
  bool isPhiProj();
  bool is0LocAdd();
  bool is1LocAdd(); 
//
  ubit16 getFooterVal() {return s_footerVal;};
  ubit16 getFooterPos() {return s_footerPos[0];};

  virtual bool isHeader();
          bool isSubHeader();
  virtual bool isBody();
  virtual bool isFooter();

  // create a 16 bits word of the CMA frame structure , starting from the fields:
  //  ubit16 set16Bits(const ubit16 num, const ubit16 *pos, const ubit16 *val);
  // get the fields of the 16 bits word of the CMA frame structure:
  //  ubit16 get16Bits(const ubit16 num, const ubit16  pos, const ubit16  val);

  //some optional methods:
  //get the real channel strip:
  ubit16 global_channel() ; //0-64
  ubit16 isPivot();
  bool isTrigger();

 private:
  //************************//
  // Matrix ReadOut decoded data
  //
  //  ubit16 m_word;
  //  char m_field;
  ubit16 m_cmid;
  ubit16 m_fel1id;
  ubit16 m_febcid;
  ubit16 m_bcid;
  ubit16 m_time;
  ubit16 m_ijk;
  ubit16 m_channel;
  ubit16 m_strip;
  ubit16 m_threshold;
  ubit16 m_overlap;
  ubit16 m_status;
  ubit16 m_crc;


  //some static parameter describing the structure
  static const ubit16 s_headerNum=3;
  static const ubit16 s_headerPos[s_headerNum];
  static const ubit16 s_headerLen[s_headerNum];
  static const ubit16 s_headerVal;
  static const ubit16 s_subHeaderNum=2;
  static const ubit16 s_subHeaderPos[s_subHeaderNum];
  static const ubit16 s_subHeaderLen[s_subHeaderNum];
  static const ubit16 s_subHeaderVal;
  static const ubit16 s_CMABodyNum=5;
  static const ubit16 s_CMABodyPos[s_CMABodyNum];
  static const ubit16 s_CMABodyLen[s_CMABodyNum];
  static const ubit16 s_CMABodyVal;
  static const ubit16 s_footerNum=3;
  static const ubit16 s_footerPos[s_footerNum];
  static const ubit16 s_footerLen[s_footerNum];
  static const ubit16 s_footerVal;



  //**********************************************************//
  // Header:    bit 00-08  FEL1ID
  // Header:    bit 09-11  CMcode
  // Header:    bit 12-12  0 - reserved
  // Header:    bit 13-13  0 - reserved
  // Header:    bit 14-14  1 - reserved
  // Header:    bit 15-15  1 - reserved
  // Header:    bit 00-08  FEL1ID
  //
  // Subheader: bit 00-11  FEBCID
  // Subheader: bit 12-12  0 - reserved
  // Subheader: bit 13-13  0 - reserved
  // Subheader: bit 14-14  0 - reserved
  // Subheader: bit 15-15  1 - reserved
  //
  // Body:      bit 00-04  strip address
  // Body:      bit 05-07  IJK
  // Body:      bit 08-10  strip time
  // Body:      bit 11-13  BCID
  // Body:      bit 14-15  0
  //
  // IJK coding:
  // IJK = 0 ==> pivot plane         layer 0   channels 00-31  (all channels)
  // IJK = 1 ==> pivot plane         layer 1   channels 00-31  (all channels)
  // IJK = 2 ==> coincidence plane   layer 0   channels 00-31  (first 32 channels)
  // IJK = 3 ==> coincidence plane   layer 0   channels 32-63  (secon 32 channels)
  // IJK = 4 ==> coincidence plane   layer 1   channels 00-31  (first 32 channels)
  // IJK = 5 ==> coincidence plane   layer 1   channels 32-63  (secon 32 channels)
  // IJK = 6 ==> trigger pattern               channels 00-31
  // IJK = 7 ==> highest threshold triggered + overlap flags     
  // 
  //   15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
  //  -----------------------------------------------------------------
  //  | 1 | 1 | 0 | 0 |  CMcode   |           F E L 1 I D             | HEADER
  //  -----------------------------------------------------------------
  //  | 1 | 0 | 0 | 0 |                 F E B C I D                   | SUBHEADER
  //  -----------------------------------------------------------------
  //  | 0 | 0 |   BCID    |   TIME    |    IJK    |       STRIP       | BODY-Hit
  //  -----------------------------------------------------------------
  //  | 0 | 0 |   BCID    |   TIME    |    IJK    |       STRIP       | BODY-Hit
  //  -----------------------------------------------------------------
  //  | 0 | 0 |   BCID    |   TIME    |    IJK    |       STRIP       | BODY-Hit
  //  .................................................................
  //  -----------------------------------------------------------------
  //  | 0 | 0 |   BCID    |   TIME    |    IJK    |       STRIP       | BODY-Hit
  //  -----------------------------------------------------------------
  //  | 0 | 1 | 0 | 0 |     STATUS    |             CRC               | FOOTER
  //  -----------------------------------------------------------------
  // 
  // CMcode:  bit 00  localAddress (0 or 1)
  // CMcode:  bit 01  etaPhi: 0=eta; 1=phi
  // CMcode:  bit 02  lowHig: 0=low; 1=high
  //
};
#endif
