// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CONFDBIF_H
#define CONFDBIF_H

//static char CONFDBIF_H_ID[] = "CVS: $Id: ConfDBif.h,v 1.1 2008-02-07 16:07:27 dwhittin Exp $";


#include <string>
#include <vector>


typedef struct {
  int ChipID;
  short Chip_Valid;
  short RODgroup;
  int RODinput;
  short TTC_Line_FE;
  short HW_addr_FE;
  short Thresh0_Low_FE;
  short Thresh0_High_FE;
  short Thresh1_Low_FE;
  short Thresh1_High_FE;
  short VT_DAC0_FE;
  short VT_DAC1_FE;
  int Mask_FE;
  short Pipe_Latency_FE;
  std::string  Clock_FE;

} DTMROCobj_t;

typedef struct {
  int VMEslot;
  int RODhalf;
  int EdgeSelect[4];
  int GolDelay[4];
  int DetID;
  //  std::string FEMode;
  std::vector<DTMROCobj_t> dtmroc_params;
} RODobj_t;

typedef struct {
  int Group;
  int finebx[10];
  int finedx[10];
  int TDMdelay[10];
  int EdgeSelect[10];
  int DutyCycle;
} TTCGroupobj_t;

typedef struct {
  int VMEslot;
  int Delay;
  short    ArShaping;
  short    SetSendID;
  int DetID;
  std::vector<TTCGroupobj_t> Groups;
  std::vector<DTMROCobj_t> dtmroc_params;
  std::string Comment;
} TTCobj_t;


typedef struct {
  int ttc_UID;
  long long ttc_iovfk;
  short ByteSwapping;
  int PhysicalAddress;
  int DetID;  
  int VMEslot;
  int Delay;
  short    ArShaping;
  short    SetSendID;
  std::string Name;
  std::string ClockSource;
  std::string InputFragmentType;
  std::string Comment;
  std::string ttc_tag;
} TTC_t;

typedef struct {
  int ttcgr_UID;
  long long ttcgr_iovfk;
  int Group;
  int DutyCycle;
  std::string EdgeSelect;
  std::string finebx;
  std::string finedx;
  std::string TDMdelay;
  std::string TTCGRNAME;
  std::string ttcgr_tag;
} TTCGroup_t;

typedef struct {
  int rod_UID;
  long long rod_iovfk;
  short RODByteSwapping;
  int RODPhysicalAddress;
  int RODDetID;
  int RODVMEslot;
  int RODhalf;
  int EdgeSelect0;
  int EdgeSelect1;
  int EdgeSelect2;
  int EdgeSelect3;
  int GolDelay0;
  int GolDelay1;
  int GolDelay2;
  int GolDelay3;
  short RODStatus;
  std::string RODInputFragmentType;
  std::string RODName;
  std::string Comment;
  std::string rod_tag;
} ROD_t;

typedef struct {
  int dtmroc_UID;
  long long dtmroc_iovfk;
  short DTMROCByteSwapping;
  int DTMROCPhysicalAddress;
  int ChipID;
  short Chip_Valid;
  short RODgroup;
  int RODinput;
  short HW_addr_FE;
  short TTC_Line_FE;
  short Thresh0_Low_FE;
  short Thresh0_High_FE;
  short Thresh1_Low_FE;
  short Thresh1_High_FE;
  short VT_DAC0_FE;
  short VT_DAC1_FE;
  int Mask_FE;
  short Pipe_Latency_FE;
  std::string DTMROCName;
  std::string DTMROCInputFragmentType;
  std::string  Clock_FE;
  std::string Comment;
  std::string roc_tag;
} DTMROC_t;

typedef struct {
  std::vector<TTCGroup_t> ttcgr;
  std::vector<ROD_t> rod;
  std::vector<DTMROC_t> roc;
} Barrel_t;

typedef struct {
  std::vector<TTC_t> ttc;
  std::vector<TTCGroup_t> ttcgr;
  std::vector<ROD_t> rod;
  std::vector<DTMROC_t> dtmroc;
} Detector_t;


typedef struct{
  int part;
  int crate;
  int slot;
  int phi;
} Mapping_t;


typedef struct {
  unsigned int phi;
  std::string side;
  std::string name;
} FE_Board;
    
typedef struct {
  unsigned int hwaddr;
  unsigned int lthresh0;
  unsigned int hthresh0;
  unsigned int lthresh1;
  unsigned int hthresh1;
  FE_Board board;
} DTMROC;

typedef struct{
  int phi;
  std::string side;
  int ttc;
  int group;
  int line;
  int bx;
  int dx;
  int strobe;
  int tdm;
  std::string name;
} TTC_Line;

typedef struct{
  std::string attr;
  std::string name;
  std::string type;
  std::string num;
  std::vector<int> values;
  std::string cattr;
} TTC_Line_Attr;

typedef struct {
  int phi;
  std::string side;
  unsigned int rod_id;
  unsigned int gol;
  unsigned int delay;
} GOL;



//RODobj_t *ConfDB_getROD( string UID );
//TTCobj_t *ConfDB_getTTC( string UID );

#endif // not CONFDBIF_H
