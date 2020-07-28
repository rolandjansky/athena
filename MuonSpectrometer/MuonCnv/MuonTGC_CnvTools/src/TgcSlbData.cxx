/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcSlbData.h"

Muon::TgcSlbData::TgcSlbData()
  : m_bcId(0), m_l1Id(0), 
    m_sswId(999), m_slbId(999),    
    m_sbLoc(999),  
    m_error(0),
    m_type(SLB_NONE)
{
  // init bit array
  for(int i=0; i<BITMAP_SIZE; ++i) {
    m_bitArray_C[i]=false;
    m_bitArray_P[i]=false;
    m_bitArray_N[i]=false;
  }
}


Muon::TgcSlbData::TgcSlbData(uint16_t bcId, uint32_t l1Id, 
			     uint16_t sswId, uint16_t slbId)
  : m_bcId(bcId), m_l1Id(l1Id), 
    m_sswId(sswId), m_slbId(slbId),    
    m_sbLoc(999),  
    m_error(0),
    m_type(SLB_HIT)
{
  // init bit array
  for(int i=0; i<BITMAP_SIZE; ++i) {
    m_bitArray_C[i]=false;
    m_bitArray_P[i]=false;
    m_bitArray_N[i]=false;
  }
}

Muon::TgcSlbData::~TgcSlbData(void) 
{}    

bool Muon::TgcSlbData::equal(uint16_t /* bcId */,
			     uint32_t l1Id,
			     uint16_t sswId,
			     uint16_t slbId) const
{
  return ((m_l1Id == l1Id) && (m_sswId == sswId) && (m_slbId == slbId));
}

const bool* Muon::TgcSlbData::getBitArray(const uint16_t bcTag) const 
{
  const bool* bitArray = 0; 
  switch (bcTag)
    {
    case BC_CENTRAL:
      bitArray = m_bitArray_C;
      break;
    case BC_PREVIOUS:
      bitArray = m_bitArray_P;
      break;
    case BC_NEXT:
      bitArray = m_bitArray_N;
      break;
    case BC_UNDEFINED:
      bitArray = m_bitArray_C; // for backward compatibility
      break;
    default:
      bitArray = 0;
      break;
    }
  return bitArray;
} 

// This function avoids using const cast internally
bool* Muon::TgcSlbData::getBitArray(const uint16_t bcTag)
{
  bool* bitArray = 0;
  switch (bcTag)
    {
    case BC_CENTRAL:
      bitArray = m_bitArray_C;
      break;
    case BC_PREVIOUS:
      bitArray = m_bitArray_P;
      break;
    case BC_NEXT:
      bitArray = m_bitArray_N;
      break;
    case BC_UNDEFINED:
      bitArray = m_bitArray_C; // for backward compatibility           
      break;
    default:
      bitArray = 0;
      break;
    }
  return bitArray;
}
  
// store bitmap
void Muon::TgcSlbData::setBitmap(const uint16_t bcTag,
				 const uint32_t cellAddr, const uint32_t cellData)
{
  // Padding Word for 32-bit alignment
  if(cellAddr == PADDING_WORD) return;

  // get bit array
  bool *bitArray = getBitArray(bcTag);

  // assign
  int indexMap = BITMAP_SIZE - CELL_SIZE*(cellAddr+1);
  if(indexMap<0) return;

  for(int iBit=0; iBit<CELL_SIZE; ++iBit) {
    // index check
    if(indexMap+iBit >= BITMAP_SIZE) break;

    if(cellData & (1<<iBit)) {
      *(bitArray+indexMap+iBit)=true;
    }
  }
  return;
}

// set a bit
void Muon::TgcSlbData::setBit(const uint16_t bcTag,
			      uint32_t iBit)
  // set a bit
{
  if(iBit >= BITMAP_SIZE) return;

  // get bit array
  bool *bitArray =  getBitArray(bcTag);
  *(bitArray+iBit)=true;
}

int Muon::TgcSlbData::getType() const 
{
  return m_type;
}

void Muon::TgcSlbData::setType(int vType) 
{
  m_type = vType;
}

bool Muon::TgcSlbData::isCoincidence(void) const 
{
  return (m_type != SLB_HIT);
}

bool Muon::TgcSlbData::isLowPt(void) const 
{
  return ((m_type >= SLB_LPT_D_W)&&(m_type<=SLB_LPT_I_S));
}

bool Muon::TgcSlbData::isHighPt(void) const
{
  return ((m_type == SLB_SL_E)||(m_type==SLB_SL_F));
}

bool Muon::TgcSlbData::isSL(void) const 
{
  return ((m_type == SLB_SL_E)||(m_type==SLB_SL_F));
}

bool Muon::TgcSlbData::isInner(void) const
{
  return ((m_type == SLB_LPT_I_W)||(m_type==SLB_LPT_I_S));
}

void Muon::TgcSlbData::setBcId(uint16_t bcId) { 
  m_bcId = bcId; 
}

void Muon::TgcSlbData::setL1Id(uint32_t l1Id) { 
  m_l1Id = l1Id; 
}

void Muon::TgcSlbData::setSswId(uint16_t sswId) { 
  m_sswId = sswId; 
}

void Muon::TgcSlbData::setSlbId(uint16_t slbId) { 
  m_slbId = slbId; 
}

void Muon::TgcSlbData::setSbLoc(uint16_t sbLoc) { 
  m_sbLoc = sbLoc; 
}

void Muon::TgcSlbData::setError(uint16_t error) { 
  m_error = error; 
}

uint16_t Muon::TgcSlbData::getBcId() const { 
  return m_bcId; 
}

uint32_t Muon::TgcSlbData::getL1Id() const { 
  return m_l1Id; 
}

uint16_t Muon::TgcSlbData::getSswId() const { 
  return m_sswId; 
}

uint16_t Muon::TgcSlbData::getSlbId() const { 
  return m_slbId; 
}

uint16_t Muon::TgcSlbData::getSbLoc() const { 
  return m_sbLoc; 
}

uint16_t Muon::TgcSlbData::getError() const { 
  return m_error; 
}
