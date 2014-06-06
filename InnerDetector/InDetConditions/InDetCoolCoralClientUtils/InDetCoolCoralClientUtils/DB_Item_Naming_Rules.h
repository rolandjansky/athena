/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

// Barrel Sides
#define FRONT  2
#define BACK   1

// ROD Half
#define TOP    1
#define BOTTOM 0

// Barrel or Endcap
#define BARREL 0
#define ENDCAP 1


#define COMPUTER_NAME( side, phi ) (side == BACK ? (phi > 16 ? "sbc-trt-rcc-b-02":"sbc-trt-rcc-b-01"):(phi > 16 ? "sbc-trt-rcc-b-04":"sbc-trt-rcc-b-03"))
#define COMPUTER_NAME_ENDCAP( side, phi ) (side == BACK ? (phi > 10 ? (phi > 20 ? "sbc-trt-rcc-e-03":"sbc-trt-rcc-e-02"):"sbc-trt-rcc-e-01"):(phi > 10 ? (phi > 20 ? "sbc-trt-rcc-e-06":"sbc-trt-rcc-e-05"):"sbc-trt-rcc-e-04"))
//#define COMPUTER_INT( detector, side, phi ) detector == BARREL ? (side == BACK ? (phi > 16 ? 2:1):(phi > 16 ? 4:3)):(side == BACK ? ((phi>0 && phi<11) ? 1:((phi>10 && phi<21) ? 2:3)):((phi>0 && phi<11) ? 4:((phi>10 && phi<21) ? 5:6)))
#define COMPUTER_INT( side, phi ) (side == BACK ? (phi > 16 ? 2:1):(phi > 16 ? 4:3))
#define COMPUTER_INT_ENDCAP( side, phi ) (side == BACK ? (phi > 10 ? (phi > 20 ? 3:2):1):(phi > 10 ? (phi > 20 ? 6:5):4))

#define SEGMENT( computer ) "B-0" << computer 
#define SEGMENT_ENDCAP( computer ) "E-0" << computer 
#define PORT( computer, side ) "1000" << dec << computer*side
#define PORT_ENDCAP( computer, side ) "2000" << dec << computer*side

#define TTC_DET_ID( computer, ttc_addr, side ) hex << (side==FRONT ? 0x36:0x35) << "0" << computer << (ttc_addr < 0x10 ? "0":"") << hex << ttc_addr
#define TTC_DET_ID_ENDCAP( computer, ttc_addr, side ) hex << (side==FRONT ? 0x38:0x37) << "0" << computer << (ttc_addr < 0x10 ? "0":"") << hex << ttc_addr
#define TTC_NAME( computer, ttc_addr, side ) "ttc" << TTC_DET_ID(computer, ttc_addr, side)
#define TTC_NAME_ENDCAP( computer, ttc_addr, side ) "ttc" << TTC_DET_ID_ENDCAP(computer, ttc_addr, side)

#define ROD_DET_ID( side, phi, numb ) hex << (side==FRONT ? 0x32:0x31) << hex << (phi<0x10 ? "0":"")  << phi << "0" << numb 
#define ROD_DET_ID_ENDCAP( side, phi, numb ) hex << (side==FRONT ? 0x34:0x33) << hex << (phi<0x10 ? "0":"")  << phi << "0" << numb 
#define ROD_NAME( side, phi, numb ) "rod" << ROD_DET_ID(side,phi,numb)
#define ROD_NAME_ENDCAP( side, phi, numb ) "rod" << ROD_DET_ID_ENDCAP(side,phi,numb)

#define SECTOR(phi) (phi < 0x10 ? "0":"") << hex << phi
#define DATA_FILE(side, phi) "Stack" << (phi<10 ? "0":"") << dec << phi << "_Side" << ((side==FRONT) ? "C":"A") << ".data.xml";
//#define DATA_FILE_ENDCAP(side, phi) "Stack" << (phi<10 ? "0":"") << phi << "_Side" << ((side==FRONT) ? "C":"A") << "_Endcap.data.xml";

#define DTMROC_ID( partition, phi, addr ) hex << (partition*0x10000 + phi*0x100 + addr)
