/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
#include <iostream>
#include <cstdlib>
#include <set>
#include <algorithm>


//
//_____________________________________________________________
TileCalibDrawerOfc* TileCalibDrawerOfc::getInstance(coral::Blob&       blob
                                                    , uint16_t           objVersion
                                                    , uint32_t           nSamples
                                                    , int32_t            nPhases
                                                    , uint16_t           nChans
                                                    , uint16_t           nGains
                                                    , const std::string& author
                                                    , const std::string& comment
                                                    , uint64_t           timeStamp)
{
  TileCalibDrawerOfc* calibDrawer = new TileCalibDrawerOfc(blob);
  calibDrawer->init(objVersion, nSamples, nPhases, nChans, nGains, author, comment, timeStamp);
  return calibDrawer;
}

//
//_____________________________________________________________
const TileCalibDrawerOfc* TileCalibDrawerOfc::getInstance(const coral::Blob& blob) {

  //=== const Blob needs to be correctly formated
  if(blob.size() < static_cast<long>(m_hdrSize32)) {
    throw TileCalib::InvalidBlob("TileCalibDrawerOfc::getInstance");
  }
  return (new TileCalibDrawerOfc(blob));
}

//
//_______________________________________________________________
TileCalibDrawerOfc::TileCalibDrawerOfc(const coral::Blob& blob)
  : TileCalibDrawerBase(blob)
{
  initCheck();
}

//
//_______________________________________________________________
TileCalibDrawerOfc::TileCalibDrawerOfc(coral::Blob& blob)
  : TileCalibDrawerBase(blob)
{
  initCheck();
}

//
//_______________________________________________________________
void TileCalibDrawerOfc::initCheck()
{
  if(getBlobSize()){

    //=== check for correct blob type
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerOfc::Ctor", getObjType(), getType());
    }
    
    //=== check for correct object size
    unsigned int blobObjSize = getObjSizeByte();
    unsigned int thisObjSize = 0;
    if(     getObjVersion() == 1){ 
      //=== set object size to base unit (4 bytes) 
      thisObjSize = sizeof(uint32_t);
    }
    else if(getObjVersion() == 2){ 
      //=== set object size to base unit (4 bytes) 
      thisObjSize = sizeof(uint32_t);
    }
    else if(getObjVersion() == 3){ 
      //=== set object size to base unit (4 bytes) 
      thisObjSize = sizeof(uint32_t);
    }
    if(blobObjSize != thisObjSize){
      throw TileCalib::SizeConflict("TileCalibDrawerOfc::Ctor", thisObjSize, blobObjSize);
    }
    
  }
}

//
//_______________________________________________________________
void TileCalibDrawerOfc::init(uint16_t objVersion
                              , uint32_t nSamples
                              , int32_t nPhases
                              , uint16_t nChans
                              , uint16_t nGains
                              , const std::string& author
                              , const std::string& comment
                              , uint64_t timeStamp)
{
  const uint16_t extraHeaderSize(2);
  uint16_t objType  = getType();
  uint32_t objSize  = 1; // everything is either uint32_t or float
  uint32_t objCount = extraHeaderSize; 

  //=== if nPhases<0, use one phase table for ADCs
  //=== otherwise use one table per ADC
  unsigned int nPhasesAbs = std::abs(nPhases);
  int          nPhasesSgn = nPhases < 0 ? -1 : 1;
  unsigned int phasesSize = nPhasesAbs;
  if(nPhasesSgn > 0){ phasesSize *= nChans*nGains; }
  objCount += phasesSize;
  
  uint32_t nFields = getNFields(objVersion);
  objCount += ( nSamples * nFields * nGains * nPhasesAbs * nChans );
  
  //=== create blob of correct type and size
  uint32_t blobLengthUint32 = 
    createBlob(objType, objVersion, objSize, objCount, nChans, nGains, author, comment, timeStamp);
  
  //=== fill header
  uint32_t* pu = static_cast<uint32_t*>(getAddress(0));
  *pu = nSamples; 
  int32_t* pi = reinterpret_cast<int32_t*>(++pu);
  *pi = nPhases;
  //=== initialize phase field to zero
  for(unsigned int iPhase = 0; iPhase < phasesSize; ++iPhase){
    *(++pi) = int32_t(0);
  }
  
  //=== initialize rest of blob to zero
  union {
    unsigned int ii;
    float ff;
  } zero;
  zero.ff = 0;
  for(unsigned int i = 0; i < blobLengthUint32 - (m_hdrSize32 + extraHeaderSize+phasesSize); ++i){
    *(++pi) = zero.ii;
  }
}


//
//______________________________________________________________
void TileCalibDrawerOfc::setPhases(unsigned int channel, unsigned int adc, const std::vector<float>& phases) {

  std::set<int32_t> phaseSet;
  for (const float phase : phases) {
    phaseSet.insert((int32_t) std::round(phase * (1 / PHASE_PRECISION))); // Phases are stored as int(10*phase) in DB  
  }

  if(int(phaseSet.size()) != std::abs(getNPhases())){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::setPhases", phaseSet.size(), std::abs(getNPhases()));
  } 

  unsigned int idx(0);
  for (const int32_t phase : phaseSet) {
    *(getPhaseStartAddress(channel, adc, idx)) = phase;
    ++idx;
  }

}

//
//______________________________________________________________
void TileCalibDrawerOfc::dump(std::ostream& stm) const {
  TileCalibDrawerBase::dumpHeader(stm);
  unsigned int channel = 0;
  for(int iPhase = 0; iPhase < std::abs(getNPhases()); ++iPhase){
    for(unsigned int sample = 0; sample < getNSamples(); ++sample){
      for(unsigned int adc = 0; adc < getNGains(); ++adc){
	float phase = getPhase(channel, adc, iPhase);
	stm << channel << "/" << phase << "/" << sample << "/" << adc << "\t";
	for(unsigned int field = 0; field < getNFields(getObjVersion()); ++field){
	  stm << getOfc(field,channel, adc, phase, sample) << "\t";
	}
	stm << std::endl;
      }
    }
  }
}
