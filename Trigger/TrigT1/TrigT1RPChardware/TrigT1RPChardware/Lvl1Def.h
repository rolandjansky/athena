/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Lvl1Def_H
#define Lvl1Def_H
#include <inttypes.h>
//
//typedef unsigned long  int CMAword; // 32 bits for the elementary CMA integer
//typedef unsigned long  int RODword; // 32 bits for the elementary ROD integer
//typedef   signed long  int sbit32;  //
//typedef unsigned short int ubit16;  //
//typedef   signed short int sbit16;  //
//
// definitions compatible with 64-bit machines
//
typedef uint32_t CMAword; // 32 bits for the elementary CMA integer
typedef uint32_t RODword; // 32 bits for the elementary ROD integer
typedef  int32_t sbit32;  //
typedef uint16_t ubit16;  //
typedef  int16_t sbit16;  //
#endif
