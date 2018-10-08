/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include <iostream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

FTK_RawPixelCluster::FTK_RawPixelCluster():
  m_word_a(0), 
  m_word_b(0),
  m_barcode(0)
{
  // nothing to do
}


FTK_RawPixelCluster::FTK_RawPixelCluster(uint32_t word_a, uint32_t word_b){
  m_word_a = word_a;
  m_word_b = word_b;
  m_barcode = 0;
}

FTK_RawPixelCluster::FTK_RawPixelCluster(uint32_t word_a, uint32_t word_b, long int barcode){
  m_word_a = word_a;
  m_word_b = word_b;
  m_barcode = barcode;
}

FTK_RawPixelCluster::~FTK_RawPixelCluster()
{
  // nothing to do
}

void FTK_RawPixelCluster::setColWidth(unsigned int col_width){

  uint32_t width = col_width;

  // Store width starting with 0
  if (width>0) width-=1; //transform from width=1 to 8 to packed width 0-7

  if (width>7) width=7;

  width = width << 28;  // fill bits 28-30
  m_word_b = width | m_word_b;

  return;
}

void FTK_RawPixelCluster::setColCoord(float col_coord){

  //  float multiplier = 10.;
  ////////  float range_top  = 166; // 144 for non-IBL layers
  float range_top = 4096;
  float range_bot  = 0.;
  int   position   = 16;

  uint32_t coord = 0;
  if(col_coord > range_bot && col_coord < range_top){
    //    coord = NINT( multiplier*col_coord);
    coord = NINT(col_coord);
    coord = coord << position;
    m_word_b = (coord | m_word_b);
  }

  return;
}

void FTK_RawPixelCluster::setRowWidth(unsigned int row_width){

  uint32_t width = row_width;  

  // Store width starting with 0  
  if (width>0) width-=1; //transform from width=1 to 8 to packed width 0-7


  if (width>7) width=7; 

  width = width << 12;  // fill bits 12-14
  m_word_b = width | m_word_b;

  return;
}

void FTK_RawPixelCluster::setRowCoord(float row_coord){

  //  int multiplier = 10;
  int  range_top   = 4096; // 328 for non-IBL layers
  //  int  range_top   = 336; // 328 for non-IBL layers
  int  range_bot   = 0;
  int   offset     = 0;
  int   position   = 0;

  uint32_t coord = 0;
  if( (row_coord > range_bot) && (row_coord < range_top)){
    coord = row_coord + offset;
    //    coord = multiplier*row_coord + offset;
  }

  coord = coord << position;
  m_word_b = (coord | m_word_b);

  return;
}

unsigned int FTK_RawPixelCluster::getModuleID() const{

  uint32_t id = m_word_a & 0xfff; // first 12 bits
  return id;
}

void FTK_RawPixelCluster::setModuleID(unsigned int module_id){

  uint32_t id = module_id;
  id = id & 0xfff; // first 12 bits
  m_word_a = (id | m_word_a);

  return;
}


float FTK_RawPixelCluster::getColCoord() const{ 

  //  float multiplier = 10.;

  uint32_t col = 0;
  col = col | (m_word_b & 0xfff0000); // bits 16-27
  col = col >> 16;

  //  float col_coord = ((float)(col)/multiplier);
  //  return col_coord;

  return col;
}

float FTK_RawPixelCluster::getRowCoord() const { 

  ///  float multiplier = 10.;
  int   offset     = 0;
  uint16_t row = 0;
  row = m_word_b & 0xfff; // first 12 bits
  //  float row_coord = (((float)(row - offset))/multiplier);
  float row_coord = (float)(row - offset);

  return row_coord;

}

unsigned int FTK_RawPixelCluster::getColWidth() const { 

  uint32_t col = (m_word_b & 0x70000000) >> 28; //bits 28-30
  return (unsigned int) (col+1); // transform from packed width=0 to 7 to width 1-8
}

unsigned int FTK_RawPixelCluster::getRowWidth() const { 

  uint16_t row = (m_word_b & 0x7000)>>12; // select bits 12-14
  return (unsigned int) (row+1);  // transform from packed width=0 to 7 to width 1-8
}

#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawPixelCluster;
#endif
