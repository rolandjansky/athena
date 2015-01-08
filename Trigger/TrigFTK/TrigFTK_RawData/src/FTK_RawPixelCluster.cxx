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
  m_layer(-1),
  m_barcode(0)
{
  // nothing to do
}

FTK_RawPixelCluster::FTK_RawPixelCluster(int i):
  m_word_a(0),
  m_word_b(0),
  m_barcode(0)
{
  m_layer = i;
}

FTK_RawPixelCluster::FTK_RawPixelCluster(uint32_t word_a, uint32_t word_b, int layer){
  m_word_a = word_a;
  m_word_b = word_b;
  m_layer  = layer;
  m_barcode = 0;
}

FTK_RawPixelCluster::~FTK_RawPixelCluster()
{
  // nothing to do
}

void FTK_RawPixelCluster::setColWidth(unsigned int col_width){

  float range_top  = 9.; 
  float range_bot  = 0.;

  if(col_width > range_bot && col_width < range_top){
    uint32_t width = col_width;
    width = width << 26;
    m_word_b = width | m_word_b;
  }

  return;
}

void FTK_RawPixelCluster::setColCoord(float col_coord){

  if (m_layer == -1){
    std::cout << "ERROR: You have not set the layer for this Pixel Cluster, col coord not set" << std::endl;
    return;
  }
  float multiplier = 10.;
  float range_top  = 144.;
  float range_bot  = 0.;
  int   position   = 14;
  if (m_layer == 0){ // IBL has higher precision
    multiplier = 10.;
    range_top  = 166.;
    range_bot  = 0.;
  }

  uint32_t coord = 0;
  if(col_coord > range_bot && col_coord < range_top){
    coord = NINT( multiplier*col_coord);
    coord = coord << position;
    m_word_b = (coord | m_word_b);
  }

  return;
}

void FTK_RawPixelCluster::setRowWidth(unsigned int row_width){

  float multiplier = 1.; // Range multiplier
  float range_top  = 5.; 
  float range_bot  = 0.;
  int   offset     = -1; // If you want to include -ve numbers it should be 2^n-1 where n is the number of bits
  int   position   = 12; // Position along the 32-bit word

  uint32_t width = 0;
  if(row_width > range_bot && row_width < range_top)
    width = NINT( multiplier*row_width + offset);
  
  width = width << position;
  m_word_b = (width | m_word_b);

  return;
}

void FTK_RawPixelCluster::setRowCoord(float row_coord){

  if (m_layer == -1){
    std::cout << "ERROR: You have not set the layer for this Pixel Cluster, row coord not set" << std::endl;
    return;
  }
  float multiplier = 10.;
  int  range_top   = 328;
  int  range_bot   = 0;
  int   offset     = 0;
  int   position   = 0;
  if (m_layer == 0){ // IBL has higher precision
    multiplier = 10.;
    range_top  = 336;
    range_bot  = 0.;
    offset     = 0;
  }

  uint32_t coord = 0;
  if( (row_coord > range_bot) && (row_coord < range_top)){
    coord = NINT( multiplier*row_coord + offset);
  }

  coord = coord << position;
  m_word_b = (coord | m_word_b);

  return;
}

unsigned int FTK_RawPixelCluster::getModuleID() const{

  uint32_t id = (m_word_a << 20) >> 20;
  return id;
}

void FTK_RawPixelCluster::setModuleID(unsigned int module_id){

  uint32_t id = module_id;
  id = (id << 20) >> 20;
  m_word_a = (id | m_word_a);

  return;
}

float FTK_RawPixelCluster::getColCoord() const{ 

  float multiplier = 10.;

  uint32_t col = 0;
  col = col | ((m_word_b << 6) >> 20);
  float col_coord = ((float)(col)/multiplier);

  return col_coord;
}

float FTK_RawPixelCluster::getRowCoord() const { 

  float multiplier = 10.;
  int   offset     = 0;
  int   position   = 0;

  uint16_t row = 0;
  row = row | (m_word_b >> position);
  row = row << 4;
  row = row >> 4; // last bit should always be 0 so we probably don't need this  
  float row_coord = (((float)(row - offset))/multiplier);

  return row_coord;

}

int FTK_RawPixelCluster::getColWidth() const { 

  uint32_t col = ((m_word_b << 3) >> 29);
  return (int)col;
}

int FTK_RawPixelCluster::getRowWidth() const { 

  float multiplier = 1.; // Range multiplier
  //float range_top  = 5.; 
  //float range_bot  = 1.;
  int   offset     = -1; // If you want to include -ve numbers it should be 2^n-1 where n is the number of bits
  int   position   = 12; // Position along the 32-bit word}

  uint16_t row = 0;
  row = row | (m_word_b >> position);
  row = row << 14;
  row = row >> 14; 
  int row_width = (int)((row - offset)/multiplier);

  return row_width;
}

#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawPixelCluster;
#endif
